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
	HpIncrease(Id_, MaxHp(Level_) / 10);

	if (Hp_ < MaxHp(Level_))
		EventManager::Get().AddEvent({ [this]() { this->HpRegen(); } ,5s });
}

bool Player::Move(Position diff)
{
	bool moved = Character::Move(diff);
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

void Player::Regen()
{
	HpIncrease(Id_, 9999);
	ExpSum(Id_, -Exp_ / 2);
	Move(StartPosition_ - GetPos());
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

	sc_set_position setPositioon;
	setPositioon.id = Id_;
	setPositioon.pos = pos;
	Server::Get().GetClients()[Id_].DoSend(&setPositioon);

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
		// 연속으로 이동하면, 이벤트가 여러번 보내진다. 결과는 마지막에 실행되므로
		// 각 이동에 대한 이벤트는 모든 이동을 마친 최종 위치에서 한번에 실행 될 수 있다.
		// MOVE가 멀티쓰레드로 돌아갈 수 있는 함수이므로..

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

void Player::ExpSum(ID agent, int amount)
{
	auto requireExp = RequireExp(Hp_);
	Exp_ += amount;
	if (requireExp <= Exp_)
	{
		//LevelUp
		Level_++; Exp_ -= requireExp;
		sc_set_level set_level;
		set_level.id = Id_;
		set_level.level = Level_;
		Server::Get().GetClients()[Id_].DoSend(&set_level);
		HpIncrease(Id_, 9999);
	}

	Exp_ -= Exp_ * (Exp_ < 0);

	sc_set_exp set_exp;
	set_exp.id = Id_;
	set_exp.exp = Exp_;
	Server::Get().GetClients()[Id_].DoSend(&set_exp);
}

void Player::HpDecrease(ID agent, int dmg)
{
	Hp_ -= dmg;
	if (Hp_ <= 0)
	{
		Regen();
	}
}

void Player::HpIncrease(ID agent, int amount)
{
	Hp_ = std::max(Hp_ + amount, MaxHp(Level_));

	sc_set_hp set_hp;
	set_hp.id = Id_;
	set_hp.hp = Hp_;
	Server::Get().GetClients()[Id_].DoSend(&set_hp);
}
