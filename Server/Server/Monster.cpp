#include "stdafx.h"
#include "Monster.h"
#include "World.h"
#include "TimerEvent.h"
#include "Character.h"
#include "Server.h"

#define lua_toint(L, i ) static_cast<int>(lua_tonumber(L, i))

int API_SendMessage(lua_State* L)
{
	int clinetId = lua_toint(L, -4);
	int speakerId = lua_toint(L, -3);
	const char* mess = lua_tostring(L, -2);
	int delaySec = lua_toint(L, -1);
	lua_pop(L, 5);

	EventManager::Get().AddEvent({[clinetId, speakerId, mess]()
	{
			sc_chat chat;
			chat.id = speakerId;
			chat.time = system_clock::now();
			strcpy_s(chat.chat, mess);
			chat.size -= sizeof(chat.chat) - strlen(chat.chat);
			// assert(strlen(chat.chat) < MAX_CHAT_SIZE);
			Server::Get().GetClients()[clinetId].DoSend(&chat);
	}, seconds{ delaySec } });
	return 0;
}

int API_MoveRandomly(lua_State* L)
{
	int actorId = lua_toint(L, -2);
	int count = lua_toint(L, -1);
	lua_pop(L, 3);

	while (0 < count--)
	{
		EventManager::Get().AddEvent({[&manager = CharacterManager::Get(), actorId] ()
		{
			manager.Move(actorId, static_cast<eMoveOper>(rand() % 4));
		}, seconds{ count } });
	}
	return 0;
}

int API_GetPos(lua_State* L)
{
	int objectId = lua_toint(L, -1);
	lua_pop(L, 2);

	auto pos = CharacterManager::Get().GetPosition(objectId);

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

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

	auto ret = DynamicObj::Move(diff);

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
	if (!DynamicObj::Enable()) return false;
		EventManager::Get().AddEvent({ [this]() { Update(); }, 1s });
	return true;
}

bool Monster::Disable()
{
	if (!DynamicObj::Disable()) return false;


	return true;
}

