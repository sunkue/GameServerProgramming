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
	auto sector = GetSectorByPosition(get_pos());
	set_sector(sector);
	enable_ = true;
	World::get().ChangeSector(this, sector);
}

void Player::Disable()
{
	enable_ = false;

	{
		auto& sector = World::get().GetSector(get_sector());
		sector.EraseObjFromSector(this);
	}

	{
		unique_lock lck{ viewLock };
		sc_remove_obj remove;
		remove.id = id_;

		for (auto& p : viewList_)
		{
			Server::get().get_clients()[p].do_send(&remove);
		}

		viewList_.clear();
	}

	id_ = -1;
	set_pos(Position{ 0 });
}

void Player::UpdateViewList()
{
	auto pos = get_pos();
	auto nearSectors = World::get().GetNearSectors4(pos, get_sector());

	decltype(viewList_) nearList;

	{
		shared_lock lck{ viewLock };
		nearList = viewList_;
	}

	for (auto& ns : nearSectors)
	{
		shared_lock lck{ ns->playerLock };

		auto& players = ns->get_players();
		for (auto& p : players)
		{
			// 나한테 보내기는 타입스탬프때문에 processpacket에서 처리.
			if (this == p)
				continue;
			
			auto otherPos = p->get_pos();
			auto diff = otherPos - pos;
			auto otherId = p->get_id();
			if (IsInSight(otherPos))
			{
				// in the sight
				if (nearList.insert(otherId).second)
				{
					sc_set_position setPositioon;
					setPositioon.id = otherId;
					setPositioon.pos = PlayerManager::get().GetPosition(otherId);
					Server::get().get_clients()[id_].do_send(&setPositioon);
				}

				p->InsertToViewList(id_);
			}
			else
			{
				// out of the sight
				if (nearList.unsafe_erase(otherId))
				{
					sc_remove_obj remove;
					remove.id = otherId;
					Server::get().get_clients()[id_].do_send(&remove);

					p->EraseFromViewList(id_);
				}
			}
		}
	}

	{
		unique_lock lck{ viewLock };
		viewList_ = nearList;
	}

}

bool Player::EraseFromViewList(ID id)
{
	bool erased = false;
	bool contain = false;

	{
		shared_lock lck{ viewLock };
		contain = viewList_.count(id);
	}

	if (contain)
	{
		unique_lock lck{ viewLock };
		erased = viewList_.unsafe_erase(id);
	}

	if (erased)
	{
		sc_remove_obj remove;
		remove.id = id;
		Server::get().get_clients()[id_].do_send(&remove);
	}

	return erased;
}

bool Player::InsertToViewList(ID id)
{
	bool inserted = false;

	{
		shared_lock lck{ viewLock };
		inserted = viewList_.insert(id).second;
	}

	sc_set_position setPositioon;
	setPositioon.id = id;
	setPositioon.pos = PlayerManager::get().GetPosition(id);
	Server::get().get_clients()[id_].do_send(&setPositioon);

	return inserted;
}



/////////////////////////////////
// 
//			PlayerManager
// 
/////////////////////////////////

bool PlayerManager::Move(ID id, move_oper oper)
{
	const Position MOVE_OPER_TABLE[4]
	{ { 0, -1 }, { 0, 1 } ,{ 1, 0 } ,{ -1, 0 } };
	auto moved = players_[id].Move(MOVE_OPER_TABLE[static_cast<int>(oper)]);
	if (moved) { players_[id].UpdateViewList(); }
	return moved;
}

bool PlayerManager::Move(ID id, Position to)
{
	auto moved = players_[id].Move(to);
	if (moved) { players_[id].UpdateViewList(); }
	return moved;
}

