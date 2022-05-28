#include "stdafx.h"
#include "Player.h"
#include "World.h"
#include "Server.h"
#include "TimerEvent.h"
#include "Character.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////

void Player::HpRegen()
{
	auto maxHp = MaxHP(Level_);
	Hp_ += maxHp / 10;

	if (maxHp <= Hp_)
		Hp_ = maxHp;
	else
		EventManager::Get().AddEvent({ [this]() { this->HpRegen(); } ,5s });

	sc_set_hp set_hp;
	set_hp.id = Id_;
	set_hp.hp = Hp_;
	Server::Get().GetClients()[Id_].DoSend(&set_hp);
}

bool Player::Move(Position diff)
{
	bool moved = DynamicObj::Move(diff);
	if (moved) { UpdateViewList(); }
	return moved;
}

bool Player::Enable()
{
	if (!DynamicObj::Enable()) return false;

	return true;
}

void Player::Update()
{

}

bool Player::Disable()
{
	if (!DynamicObj::Disable()) return false;

	{
		auto& sector = World::Get().GetSector(GetSectorIdx());
		sector.EraseObjFromSector(this);
	}

	{
		unique_lock lck{ ViewLock };
		ViewList_.clear();
	}

	SetPos(Position{ 0 });
	return true;
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
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto& p : players)
			{
				// ������ ������� Ÿ�Խ����������� processpacket���� ó��.
				if (this == p)
					continue;

				auto otherPos = p->GetPos();
				auto otherId = p->GetId();
				if (IsInSightAndEnable(otherPos))
				{
					// in the sight
					if (nearList.insert(otherId).second)
					{
						sc_set_position setPositioon;
						setPositioon.id = otherId;
						setPositioon.pos = CharacterManager::Get().GetPosition(otherId);
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
		MostersIteratorCouldDangling:
			shared_lock lck{ ns->MonsterLock };
			auto& monsters = ns->GetMonsters();
			for (auto& m : monsters)
			{
				auto otherPos = m->GetPos();
				auto otherId = m->GetId();
				if (IsInSight(otherPos))
				{
					lck.unlock();
					if (m->Enable())
					{
						goto MostersIteratorCouldDangling;
					}
					lck.lock();
					if (nearList.insert(otherId).second)
					{
						sc_set_position setPositioon;
						setPositioon.id = otherId;
						setPositioon.pos = CharacterManager::Get().GetPosition(otherId);
						Server::Get().GetClients()[Id_].DoSend(&setPositioon);
					}
					/*
					
					*/
				}
				else
				{
					if (nearList.unsafe_erase(otherId))
					{
						sc_remove_obj remove;
						remove.id = otherId;
						Server::Get().GetClients()[Id_].DoSend(&remove);
					}
				}
			}
		}
	}


	for (auto& characterId : nearList)
	{
		// �������� �̵��ϸ�, �̺�Ʈ�� ������ ��������. ����� �������� ����ǹǷ�
		// �� �̵��� ���� �̺�Ʈ�� ��� �̵��� ��ģ ���� ��ġ���� �ѹ��� ���� �� �� �ִ�.
		// MOVE�� ��Ƽ������� ���ư� �� �ִ� �Լ��̹Ƿ�..

		if (MAX_PLAYER <= characterId && characterId < MAX_MONSTER + MAX_PLAYER)
		{
			Monster* m = reinterpret_cast<Monster*>(CharacterManager::Get().GetCharacters()[characterId].get());
			EventManager::Get().AddEvent(
				{ [id = Id_, L = m->GetScripts()[eScriptType::AI], &mutex = m->ScriptLock[eScriptType::AI]] ()
				{
					lock_guard lck{ mutex };
					lua_getglobal(L, "EventPlayerMove");
					lua_pushnumber(L, id);
					lua_pcall(L, 1, 0, 0);
				}, 0s });
		}
	}
	
	{
		unique_lock lck{ ViewLock };
		ViewList_ = nearList;
	}
}

bool Player::EraseFromViewList(ID Id)
{
	bool erased = false;
	bool contain = false;

	{
		shared_lock lck{ ViewLock };
		contain = ViewList_.count(Id);
	}

	if (contain)
	{
		unique_lock lck{ ViewLock };
		erased = ViewList_.unsafe_erase(Id);
	}

	if (erased)
	{
		sc_remove_obj remove;
		remove.id = Id;
		Server::Get().GetClients()[Id_].DoSend(&remove);
	}

	return erased;
}

bool Player::InsertToViewList(ID Id)
{
	bool inserted = false;

	{
		shared_lock lck{ ViewLock };
		inserted = ViewList_.insert(Id).second;
	}

	sc_set_position setPositioon;
	setPositioon.id = Id;
	setPositioon.pos = CharacterManager::Get().GetPosition(Id);
	Server::Get().GetClients()[Id_].DoSend(&setPositioon);

	return inserted;
}
