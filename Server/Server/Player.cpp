#include "stdafx.h"
#include "Player.h"
#include "World.h"
#include "Server.h"
#include "TimerEvent.h"
#include "Character.h"
#include "DataBase.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////

void Player::HpRegen()
{
	HpIncrease(Id_, MaxHp() / 10);
	if (Hp_ < MaxHp())
		EventManager::Get().AddEvent({ [this]() { this->HpRegen(); } ,5s });
}

bool Player::Move(Position diff)
{
	bool moved = Character::Move(diff);
	if (moved) { UpdateViewList(); }
	return moved;
}

bool Player::MoneySum(ID agent, int amount)
{
	Money_ += amount;
	if (Money_ < 0)
	{
		Money_ -= amount;
		return false;
	}

	sc_set_money set_money;
	set_money.id = Id_;
	set_money.money = Money_;
	Server::Get().GetClients()[Id_].DoSend(&set_money);
	return true;
}

bool Player::Equip(eItemType item)
{
	auto itemLevel = ItemBase::GetItemLevel(item);

	switch (EquimentState_.Equip(item))
	{
	case eEquimentablePart::_Unable:
	{
		return false;
	}
	CASE eEquimentablePart::head :
	{
		ArmorPoint_ = itemLevel * 2;
	}
	CASE eEquimentablePart::body :
	{
		AdditionalHp_ = itemLevel * 100;
	}
	CASE eEquimentablePart::shoes :
	{
		Character::MovementCooltime = 1000ms / (itemLevel + 1);
	}
	CASE eEquimentablePart::weapon :
	{
		AttackPoint_ = itemLevel * 30;
	}
	break; default: return false;
	}

	{
		sc_equip_item equipItem;
		equipItem.type = item;
		Server::Get().GetClients()[Id_].DoSend(&equipItem);
	}

	return true;
}

bool Player::MoveForce(Position diff)
{
	bool moved = Character::MoveForce(diff);
	UpdateViewList();
	return moved;
}

bool Player::Enable()
{
	if (!DynamicObj::Enable()) return false;
	HpRegen();
	return true;
}

void Player::Update()
{

}

void Player::Regen()
{
	HpIncrease(Id_, MaxHp());
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

	{
		auto pos = GetPos();
		QueryRequest q;
		q.Query = L"EXEC UpdateCharacterDataOnLogout "s + to_wstring(DbId_);
		q.Query += L","s + to_wstring(Hp_);
		q.Query += L","s + to_wstring(pos.x);
		q.Query += L","s + to_wstring(pos.y);
		q.Query += L","s + to_wstring(Money_);
		q.Query += L","s + to_wstring(Exp_);
		q.Query += L","s + to_wstring(Level_);
		q.Targets = make_shared<vector<any>>();
		q.Func = [](const vector<any>& t) {};
		DataBase::Get().AddQueryRequest(q);
	}

	for (auto& item : Inventory_.GetItems())
	{
		auto type = item.second->GetType();
		auto num = item.second->GetNum();
		auto itemName = ItemTypeDecoder::toString(type);
		QueryRequest q;
		q.Query = L"EXEC UpdateItemData "s + to_wstring(DbId_);
		q.Query += L","s + wstring{ itemName.begin(), itemName.end() };
		q.Query += L","s + to_wstring(num);
		q.Targets = make_shared<vector<any>>();
		q.Func = [=](const vector<any>& t) {};
		DataBase::Get().AddQueryRequest(q);
	}

	SetPos(Position{ 0 });
	Inventory_.Clear();
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

						EventManager::Get().AddEvent(
							{ [id = GetId(), L = m->GetScripts()[eScriptType::UpdateAI], &mutex = m->ScriptLock[eScriptType::UpdateAI]] ()
							{
								lock_guard lck{ mutex };
								lua_getglobal(L, "EventPlayerEnterSight");
								lua_pushnumber(L, id);
								lua_pcall(L, 1, 0, 0);
							}, 0s });
					}
					else
					{
						EventManager::Get().AddEvent(
							{ [id = GetId(), L = m->GetScripts()[eScriptType::UpdateAI], &mutex = m->ScriptLock[eScriptType::UpdateAI]] ()
							{
								lock_guard lck{ mutex };
								lua_getglobal(L, "EventPlayerStaySight");
								lua_pushnumber(L, id);
								lua_pcall(L, 1, 0, 0);
							}, 0s });
					}
				}
				else
				{
					if (nearList.unsafe_erase(otherId))
					{
						sc_remove_obj remove;
						remove.id = otherId;
						Server::Get().GetClients()[Id_].DoSend(&remove);

						EventManager::Get().AddEvent(
							{ [id = GetId(), L = m->GetScripts()[eScriptType::UpdateAI], &mutex = m->ScriptLock[eScriptType::UpdateAI]] ()
							{
								lock_guard lck{ mutex };
								lua_getglobal(L, "EventPlayerExitSight");
								lua_pushnumber(L, id);
								lua_pcall(L, 1, 0, 0);
							}, 0s });
					}
				}
			}
		}

		{
			auto& obstacles = ns->GetObstacles();
			for (auto& o : obstacles)
			{
				auto otherPos = o->GetPos();
				auto otherId = o->GetId() + MAX_CHARACTER;
				if (IsInSight(otherPos))
				{
					// in the sight
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
					// out of the sight
					if (nearList.unsafe_erase(otherId))
					{
						sc_remove_obj remove;
						remove.id = otherId;
						Server::Get().GetClients()[Id_].DoSend(&remove);
					}
				}
			}
		}

		{
			shared_lock lck{ ns->ItemInstanceLock };
			auto& items = ns->GetItemInstances();
			for (auto& o : items)
			{
				auto itemPos = o.GetPos();
				auto itemType = o.GetOriginalItem().GetType();
				if (IsInSight(itemPos))
				{
					sc_set_iteminstance_position setIteminstance;
					setIteminstance.itemType = itemType;
					setIteminstance.pos = o.GetPos();
					Server::Get().GetClients()[Id_].DoSend(&setIteminstance);
				}
			}
		}
	}

	{
		unique_lock lck{ ViewLock };
		ViewList_ = nearList;
	}
}

void Player::ActivateSkill(eSkill skill)
{
	switch (skill)
	{
	case eSkill::accquireItem:
	{
		auto pos = GetPos();
		auto sector = GetSectorIdx();

		for (auto& itemInst : World::Get().GetSector(sector).GetItemInstances())
		{
			if (pos == itemInst.GetPos())
			{
				auto& itemBase = itemInst.GetOriginalItem();
				auto itemNum = itemBase.GetNum();
				auto itemType = itemBase.GetType();

				{
					Inventory_.CreateItem(itemBase);

					sc_sum_item sumItem;
					sumItem.changed = itemNum;
					sumItem.type = itemType;
					Server::Get().GetClients()[Id_].DoSend(&sumItem);
				}

				auto ns = World::Get().GetNearSectors4(pos, sector);
				for (auto s : ns)
				{
					shared_lock lck{ s->PlayerLock };
					for (auto& p : s->GetPlayers())
					{
						sc_remove_iteminstance removeIteminstance;
						removeIteminstance.itemType = itemType;
						removeIteminstance.pos = pos;
						Server::Get().GetClients()[p->GetId()].DoSend(&removeIteminstance);
					}
				}

				{
					unique_lock lck{ World::Get().GetSector(sector).ItemInstanceLock };
					auto& instances = World::Get().GetSector(sector).GetItemInstances();
					if (auto it = instances.find(itemInst); it != instances.end())
						instances.unsafe_erase(it);
				}

				break;
			}
		}
	}
	CASE eSkill::attack :
	{
		Attack();
	}
	CASE eSkill::heal :
	{

	}
	CASE eSkill::haste :
	{

	}
	CASE eSkill::set_teleport :
	{

	}
	CASE eSkill::teleport :
	{

	}
	break; default: break;
	}
}

void Player::Attack()
{
	auto pos = GetPos();
	auto ns = World::Get().GetNearSectors4(pos, GetSectorIdx());

	unordered_set<ID> attackableIdSet;

	for (auto s : ns)
	{
		for (auto& m : s->GetMonsters())
		{
			if (!m->IsAlive()) continue;
			auto diff = length(glm::vec2{ m->GetPos() - pos });
			if (diff <= 1) attackableIdSet.insert(m->GetId());
		}
	}

	vector<ID> attackableId{ attackableIdSet.begin(),attackableIdSet.end() };

	// item 무기 공격력 0~10 => DefaultAttack + rand()%(0~10)..
	if (Character::Attack(attackableId, AttackPoint()))
	{
		sc_use_skill use_skill;
		use_skill.id = GetId();
		use_skill.skill = eSkill::attack;

		for (auto s : ns)
		{
			for (auto& p : s->GetPlayers())
			{
				if (!p->GetEnable()) continue;
				Server::Get().GetClients()[p->GetId()].DoSend(&use_skill);
			}
		}
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
	auto requireExp = RequireExp(Level_);
	Exp_ += amount;
	if (requireExp <= Exp_)
	{
		//LevelUp
		Level_++; Exp_ = 0;
		HpIncrease(Id_, 9999);
		sc_set_level set_level;
		set_level.id = Id_;
		set_level.level = Level_;
		Server::Get().GetClients()[Id_].DoSend(&set_level);
	}

	sc_set_exp set_exp;
	set_exp.id = Id_;
	set_exp.exp = Exp_;
	Server::Get().GetClients()[Id_].DoSend(&set_exp);
}

void Player::HpDecrease(ID agent, int _dmg)
{
	auto dmg = max(0, _dmg - ArmorPoint_);

	if (Hp_ == MaxHp())
		EventManager::Get().AddEvent({ [this]() { this->HpRegen(); } ,5s });

	Hp_ -= dmg;
	Hp_ = max(Hp_.load(), 0);

	{
		auto ns = World::Get().GetNearSectors4(GetPos(), GetSectorIdx());
		unordered_set<Player*> players;
		for (auto s : ns)
		{
			for (auto p : s->GetPlayers())
			{
				players.insert(p);
			}
		}

		sc_set_hp pck;
		pck.hp = Hp_;
		pck.id = GetId();

		for (auto p : players)
			if (p->IsInSight(GetPos()))
				Server::Get().GetClients()[p->GetId()].DoSend(&pck);
	}

	if (Hp_ <= 0)
		Regen();
}

void Player::HpIncrease(ID agent, int amount)
{
	Hp_ += amount;
	Hp_ = min(Hp_.load(), MaxHp());

	{
		auto ns = World::Get().GetNearSectors4(GetPos(), GetSectorIdx());
		unordered_set<Player*> players;
		for (auto s : ns)
		{
			for (auto p : s->GetPlayers())
			{
				players.insert(p);
			}
		}

		sc_set_hp pck;
		pck.hp = Hp_;
		pck.id = GetId();

		for (auto p : players)
			if (p->IsInSight(GetPos()))
				Server::Get().GetClients()[p->GetId()].DoSend(&pck);
	}
}

int Player::UseItem(eItemType itemType, int num)
{
	auto used = Character::UseItem(itemType, num);
	sc_sum_item sumItem;
	sumItem.type = itemType;
	sumItem.changed = -used;
	Server::Get().GetClients()[Id_].DoSend(&sumItem);
	return used;
}
