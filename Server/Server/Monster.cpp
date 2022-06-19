#include "stdafx.h"
#include "Monster.h"
#include "World.h"
#include "TimerEvent.h"
#include "Character.h"
#include "Server.h"

Monster::Monster(ID id) : Character{ id }
{
	AggressionType_ = eMonsterAggressionType(id % 2); // Peace, Agro
	MovementType_ = eMonsterMovementType(2 <= (id % 4)); // Fixed, Roaming
	CompileScript();
}

Monster::~Monster()
{

}

void Monster::CompileScript()
{
	lock_guard lck{ ScriptLock[eScriptType::UpdateAI] };
	lua_State* aiScript;
	aiScript = luaL_newstate();
	luaL_openlibs(aiScript);
	{
		string str; str.reserve(2500);
		ifstream basicGlobalDeclaration{ "LuaScript/Monster/monsterBasicDeclaration.lua" , ios::binary };
		ifstream movementRomaing{ "LuaScript/Monster/monsterMovementRoaming.lua" , ios::binary };
		ifstream movementFixed{ "LuaScript/Monster/monsterMovementFixed.lua" , ios::binary };
		ifstream aggressionAgro{ "LuaScript/Monster/monsterAggressionAgro.lua" , ios::binary };
		ifstream aggressionPeace{ "LuaScript/Monster/monsterAggressionPeace.lua" , ios::binary };

		str += string{ (std::istreambuf_iterator<char>(basicGlobalDeclaration)), std::istreambuf_iterator<char>() };

		switch (MovementType_)
		{
		case eMonsterMovementType::Fixed:
		{
			str += string{ (std::istreambuf_iterator<char>(movementFixed)), std::istreambuf_iterator<char>() };
		}
		CASE eMonsterMovementType::Roaming :
		{
			str += string{ (std::istreambuf_iterator<char>(movementRomaing)), std::istreambuf_iterator<char>() };
		}break; default:
		{
			cerr << "MovementType_ ERR" << endl;
		}
		break;
		}

		switch (AggressionType_)
		{
		case eMonsterAggressionType::Peace:
		{
			str += string{ (std::istreambuf_iterator<char>(aggressionPeace)), std::istreambuf_iterator<char>() };
		}
		CASE eMonsterAggressionType::Agro :
		{
			str += string{ (std::istreambuf_iterator<char>(aggressionAgro)), std::istreambuf_iterator<char>() };
		}break; default:
		{
			cerr << "AggressionType_ ERR" << endl;
		}
		break;
		}

		luaL_loadstring(aiScript, str.c_str());
		lua_pcall(aiScript, 0, 0, 0);
	}
	{
		lua_getglobal(aiScript, "SetObjectId");
		lua_pushnumber(aiScript, Id_);
		lua_pcall(aiScript, 1, 0, 0);
		lua_pop(aiScript, 2);
	}
	{
		lua_register(aiScript, "API_Chat", API_SendMessage);
		lua_register(aiScript, "API_GetPos", API_GetPos);
		lua_register(aiScript, "API_MoveRandomly", API_MoveRandomly);
		lua_register(aiScript, "API_Move", API_Move);
		lua_register(aiScript, "API_NavigateAstar", API_NavigateAstar);
		lua_register(aiScript, "API_Attack", API_Attack);
		lua_register(aiScript, "API_DEBUG", API_DebugMessage);
	}
	Scripts_[eScriptType::UpdateAI] = aiScript;
}



void Monster::HpDecrease(ID agent, int amount)
{
	Hp_ -= amount;
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
		Die(agent);
	else
	{
		EventManager::Get().AddEvent(
			{ [agent = agent, L = GetScripts()[eScriptType::UpdateAI], &mutex = ScriptLock[eScriptType::UpdateAI]] ()
			{
				lock_guard lck{ mutex };
				lua_getglobal(L, "EventBeAttacked");
				lua_pushnumber(L, agent);
				lua_pcall(L, 1, 0, 0);
			}, 0s });
	}
}

void Monster::HpIncrease(ID agent, int amount)
{
	Hp_ += amount;
	Hp_ = min(Hp_.load(), MaxHp(Level_));

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

void Monster::Die(ID agent)
{
	Disable();
	Died_ = true;
	Hp_ = 0;

	{
		auto pos = GetPos();
		auto secterIdx = GetSectorIdx();

		{
			auto& s = World::Get().GetSector(secterIdx);
			shared_lock lck{ Inventory_.ItemLock };
			if (!Inventory_.GetItems().empty())
			{
				auto base = Inventory_.GetItems().begin()->second.get();
				shared_lock lck{ s.ItemInstanceLock };
				auto newItemInst = ItemInstance{ *base, pos };
				auto& instances = s.GetItemInstances();
				if (auto it = instances.find(newItemInst); it != instances.end())
					it->GetOriginalItem().SumNum(base->GetNum());
				else
					instances.insert(newItemInst);

				auto ns = World::Get().GetNearSectors4(pos, secterIdx);
				for (auto s : ns)
				{
					shared_lock lck{ s->PlayerLock };
					for (auto& p : s->GetPlayers())
					{
						sc_set_iteminstance_position setIteminstance;
						setIteminstance.itemType = base->GetType();
						setIteminstance.pos = pos;
						Server::Get().GetClients()[p->GetId()].DoSend(&setIteminstance);
					}
				}
			}
		}
	}


	EventManager::Get().AddEvent({ [this]()
		{ Regen(); }, 30s });
	if (auto killerAsPlayer = dynamic_cast<Player*>(CharacterManager::Get().GetCharacters()[agent].get()))
	{
		auto lvl = Level_.load();
		int exp = lvl * lvl * 2;
		int money = lvl * 10;
		exp += exp * (eMonsterAggressionType::Agro == AggressionType_);
		exp += exp * (eMonsterMovementType::Roaming == MovementType_);
		killerAsPlayer->ExpSum(Id_, exp, true);
		killerAsPlayer->MoneySum(Id_, money);
	}
}

void Monster::Regen()
{
	HpIncrease(Id_, MaxHp(Level_));
	Move(StartPosition_ - GetPos());
	Died_ = false;
	Enable();
}

bool Monster::Move(Position diff)
{
	constexpr size_t NEARLIST_RESERVE_HINT = 2000;
	unordered_set<Player*> oldNearList; oldNearList.reserve(NEARLIST_RESERVE_HINT);
	unordered_set<Player*> newNearList; newNearList.reserve(NEARLIST_RESERVE_HINT);

	auto oldPos = GetPos();

	if (!IsInBoundary(oldPos + diff, StartPosition_, MOVEMENT_BOUNDRAY))
		return false;

	{
		auto oldSector = GetSectorIdx();
		auto nearSector = World::Get().GetNearSectors4(oldPos, oldSector);
		for (auto& ns : nearSector)
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto p : players)
			{
				if (p->IsInSightAndEnable(oldPos))
				{
					oldNearList.insert(p);
				}
			}
		}
	}

	auto ret = Character::Move(diff);

	Position newPos;
	{
		newPos = GetPos();
		auto newSector = GetSectorIdx();
		auto nearSector = World::Get().GetNearSectors4(newPos, newSector);
		for (auto& ns : nearSector)
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto p : players)
			{
				if (p->IsInSightAndEnable(newPos))
				{
					newNearList.insert(p);
				}
			}
		}
	}

	if (newNearList.empty())
	{
		Disable();
	}

	for (auto& p : newNearList)
	{
		if (p->InsertToViewList(Id_))
		{
			EventManager::Get().AddEvent(
				{ [id = p->GetId(), L = Scripts_[eScriptType::UpdateAI], &mutex = ScriptLock[eScriptType::UpdateAI]] ()
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
				{ [id = p->GetId(), L = Scripts_[eScriptType::UpdateAI], &mutex = ScriptLock[eScriptType::UpdateAI]] ()
				{
					lock_guard lck{ mutex };
					lua_getglobal(L, "EventPlayerStaySight");
					lua_pushnumber(L, id);
					lua_pcall(L, 1, 0, 0);
				}, 0s });
		}
	}

	for (auto& p : oldNearList)
	{
		if (!newNearList.contains(p))
		{
			p->EraseFromViewList(Id_);

			EventManager::Get().AddEvent(
				{ [id = p->GetId(), L = Scripts_[eScriptType::UpdateAI], &mutex = ScriptLock[eScriptType::UpdateAI]] ()
				{
					lock_guard lck{ mutex };
					lua_getglobal(L, "EventPlayerExitSight");
					lua_pushnumber(L, id);
					lua_pcall(L, 1, 0, 0);
				}, 0s });
		}
	}

	return ret;
}

bool Monster::MoveForce(Position diff)
{
	constexpr size_t NEARLIST_RESERVE_HINT = 2000;
	unordered_set<Player*> oldNearList; oldNearList.reserve(NEARLIST_RESERVE_HINT);
	unordered_set<Player*> newNearList; newNearList.reserve(NEARLIST_RESERVE_HINT);

	auto oldPos = GetPos();

	{
		auto oldSector = GetSectorIdx();
		auto nearSector = World::Get().GetNearSectors4(oldPos, oldSector);
		for (auto& ns : nearSector)
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto p : players)
			{
				if (p->IsInSightAndEnable(oldPos))
				{
					oldNearList.insert(p);
				}
			}
		}
	}

	auto ret = Character::MoveForce(diff);

	Position newPos;
	{
		newPos = GetPos();
		auto newSector = GetSectorIdx();
		auto nearSector = World::Get().GetNearSectors4(newPos, newSector);
		for (auto& ns : nearSector)
		{
			shared_lock lck{ ns->PlayerLock };
			auto& players = ns->GetPlayers();
			for (auto p : players)
			{
				if (p->IsInSightAndEnable(newPos))
				{
					newNearList.insert(p);
				}
			}
		}
	}

	if (newNearList.empty())
	{
		Disable();
	}

	for (auto& p : newNearList)
	{
		if (p->InsertToViewList(Id_))
		{
			EventManager::Get().AddEvent(
				{ [id = p->GetId(), L = Scripts_[eScriptType::UpdateAI], &mutex = ScriptLock[eScriptType::UpdateAI]] ()
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
				{ [id = p->GetId(), L = Scripts_[eScriptType::UpdateAI], &mutex = ScriptLock[eScriptType::UpdateAI]] ()
				{
					lock_guard lck{ mutex };
					lua_getglobal(L, "EventPlayerStaySight");
					lua_pushnumber(L, id);
					lua_pcall(L, 1, 0, 0);
				}, 0s });
		}
	}

	for (auto& p : oldNearList)
	{
		if (!newNearList.contains(p))
		{
			p->EraseFromViewList(Id_);

			EventManager::Get().AddEvent(
				{ [id = p->GetId(), L = Scripts_[eScriptType::UpdateAI], &mutex = ScriptLock[eScriptType::UpdateAI]] ()
				{
					lock_guard lck{ mutex };
					lua_getglobal(L, "EventPlayerExitSight");
					lua_pushnumber(L, id);
					lua_pcall(L, 1, 0, 0);
				}, 0s });
		}
	}

	return ret;
}

void Monster::Update()
{
	if (!GetEnable()) return;
	{
		lock_guard lck{ ScriptLock[eScriptType::UpdateAI] };
		lua_getglobal(Scripts_[eScriptType::UpdateAI], "Update");
		lua_pcall(Scripts_[eScriptType::UpdateAI], 0, 0, 0);
	}
	EventManager::Get().AddEvent({ [this]() {Update(); }, 500ms });
}

bool Monster::Enable()
{
	if (Died_) return false;
	if (!DynamicObj::Enable()) return false;
	EventManager::Get().AddEvent({ [this]() { Update(); }, 100ms });
	return true;
}

bool Monster::Disable()
{
	if (!DynamicObj::Disable()) return false;

	return true;
}

