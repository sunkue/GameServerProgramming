#include "stdafx.h"
#include "Player.h"
#include "World.h"
#include "Server.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////

void Player::Enable()
{
	auto sector = GetSectorByPosition(GetPos());
	SetSectorIdx(sector);
	Enable_ = true;
	World::Get().ChangeSector(this, sector);
}

void Player::Disable()
{
	Enable_ = false;

	{
		auto& sector = World::Get().GetSector(GetSectorIdx());
		sector.EraseObjFromSector(this);
	}

	{
		unique_lock lck{ ViewLock };
		sc_remove_obj remove;
		remove.id = Id_;

		for (auto& p : ViewList_)
		{
			Server::Get().GetClients()[p].DoSend(&remove);
		}

		ViewList_.clear();
	}

	Id_ = -1;
	SetPos(Position{ 0 });
}

void Player::UpdateViewList()
{
	auto pos = GetPos();
	auto nearSectors = World::Get().GetNearSectors4(pos, GetSectorIdx());

	decltype(ViewList_) nearList;

	{
		shared_lock lck{ ViewLock };
		nearList = ViewList_;
	}

	for (auto& ns : nearSectors)
	{
		shared_lock lck{ ns->PlayerLock };

		auto& players = ns->GetPlayers();
		for (auto& p : players)
		{
			// 나한테 보내기는 타입스탬프때문에 processpacket에서 처리.
			if (this == p)
				continue;
			
			auto otherPos = p->GetPos();
			auto diff = otherPos - pos;
			auto otherId = p->GetId();
			if (IsInSight(otherPos))
			{
				// in the sight
				if (nearList.insert(otherId).second)
				{
					sc_set_position setPositioon;
					setPositioon.id = otherId;
					setPositioon.pos = PlayerManager::Get().GetPosition(otherId);
					Server::Get().GetClients()[Id_].DoSend(&setPositioon);
				}

				p->InsertToViewList(Id_);
			}
			else
			{
				// out of the sight
				if (nearList.unsafe_erase(otherId))
				{
					sc_remove_obj remove;
					remove.id = otherId;
					Server::Get().GetClients()[Id_].DoSend(&remove);

					p->EraseFromViewList(Id_);
				}
			}
		}
	}

	{
		unique_lock lck{ ViewLock };
		ViewList_ = nearList;
	}

}

bool Player::EraseFromViewList(ID Id_)
{
	bool erased = false;
	bool contain = false;

	{
		shared_lock lck{ ViewLock };
		contain = ViewList_.count(Id_);
	}

	if (contain)
	{
		unique_lock lck{ ViewLock };
		erased = ViewList_.unsafe_erase(Id_);
	}

	if (erased)
	{
		sc_remove_obj remove;
		remove.id = Id_;
		Server::Get().GetClients()[Id_].DoSend(&remove);
	}

	return erased;
}

bool Player::InsertToViewList(ID Id_)
{
	bool inserted = false;

	{
		shared_lock lck{ ViewLock };
		inserted = ViewList_.insert(Id_).second;
	}

	sc_set_position setPositioon;
	setPositioon.id = Id_;
	setPositioon.pos = PlayerManager::Get().GetPosition(Id_);
	Server::Get().GetClients()[Id_].DoSend(&setPositioon);

	return inserted;
}



/////////////////////////////////
// 
//			PlayerManager
// 
/////////////////////////////////

bool PlayerManager::Move(ID Id_, move_oper oper)
{
	const Position MOVE_OPER_TABLE[4]
	{ { 0, -1 }, { 0, 1 } ,{ 1, 0 } ,{ -1, 0 } };
	auto moved = players_[Id_].Move(MOVE_OPER_TABLE[static_cast<int>(oper)]);
	if (moved) { players_[Id_].UpdateViewList(); }
	return moved;
}

bool PlayerManager::Move(ID Id_, Position to)
{
	auto moved = players_[Id_].Move(to);
	if (moved) { players_[Id_].UpdateViewList(); }
	return moved;
}

