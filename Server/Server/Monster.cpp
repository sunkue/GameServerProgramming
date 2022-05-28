#include "stdafx.h"
#include "Monster.h"
#include "World.h"
#include "TimerEvent.h"
#include "Character.h"
#include "Server.h"

Monster::Monster(ID id) : Character{ id }
{
	CompileScript();
}

Monster::~Monster()
{

}

void Monster::CompileScript()
{
	lock_guard lck{ ScriptLock[eScriptType::AI] };
	lua_State* aiScript;
	aiScript = luaL_newstate();
	luaL_openlibs(aiScript);
	luaL_dofile(aiScript, "LuaScript/Monster/monsterAi.lua");
	lua_getglobal(aiScript, "SetObjectId");
	lua_pushnumber(aiScript, Id_);
	lua_pcall(aiScript, 1, 0, 0);
	lua_pop(aiScript, 2);
	lua_register(aiScript, "API_Chat", API_SendMessage);
	lua_register(aiScript, "API_GetPos", API_GetPos);
	lua_register(aiScript, "API_MoveRandomly", API_MoveRandomly);
	Scripts_[eScriptType::AI] = aiScript;
}



void Monster::HpDecrease(ID agent, int amount)
{
	Hp_ -= amount;
	if (Hp_ <= 0)
		Die(agent);
}

void Monster::HpIncrease(ID agent, int amount)
{
	Hp_ = max(Hp_ + amount, MaxHp(Level_));
}

void Monster::Die(ID agent)
{
	Died_ = true;
	Disable();
	EventManager::Get().AddEvent({ [this]()
		{ Regen(); }, 30s });
	if (auto killerAsPlayer = dynamic_cast<Player*>(CharacterManager::Get().GetCharacters()[agent].get()))
	{
		int exp = Level_ * Level_ * 2;
		exp += exp * (eMonsterAggressionType::Agro == AggressionType_);
		exp += exp * (eMonsterMovementType::Roaming == MovementType_);
		killerAsPlayer->ExpSum(Id_, exp);
	}
}

void Monster::Regen()
{
	HpIncrease(Id_, MaxHp(Level_));
	Move(StartPosition_ - GetPos());
	Died_ = false;
}

bool Monster::Move(Position diff)
{
	constexpr size_t NearListReserveHint = 20;
	unordered_set<Player*> oldNearList; oldNearList.reserve(NearListReserveHint);
	unordered_set<Player*> newNearList; newNearList.reserve(NearListReserveHint);

	{
		auto oldPos = GetPos();
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
		p->InsertToViewList(Id_);

	}

	for (auto& p : oldNearList)
	{
		if (!newNearList.contains(p))
		{
			p->EraseFromViewList(Id_);
		}
	}

	return ret;
}

void Monster::Update()
{
	CharacterManager::Get().Move(Id_, static_cast<eMoveOper>(rand() % 4));

	if (GetEnable())
		EventManager::Get().AddEvent({ [this]() { Update(); }, 1s });
}

bool Monster::Enable()
{
	if (Died_) return false;
	if (!DynamicObj::Enable()) return false;
	//EventManager::Get().AddEvent({ [this]() { Update(); }, 1s });
	return true;
}

bool Monster::Disable()
{
	if (!DynamicObj::Disable()) return false;

	return true;
}

