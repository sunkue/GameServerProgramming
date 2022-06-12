#include "stdafx.h"
#include "Script.h"
#include "Server.h"
#include "Character.h"
#include "World.h"
#include "TimerEvent.h"


int API_SendMessage(lua_State* L)
{
	int speakerId = lua_toint(L, -4);
	int clinetId = lua_toint(L, -3);
	const char* mess = lua_tostring(L, -2);
	int delaySec = lua_toint(L, -1);
	lua_pop(L, 5);

	EventManager::Get().AddEvent({ [clinetId, speakerId, mess]()
	{
			sc_chat chat;
			chat.id = speakerId;
			chat.time = system_clock::now();
			strcpy_s(chat.chat, sizeof(chat.chat) - 1, mess);
			chat.size -= static_cast<decltype(chat.size)>(MAX_CHAT_SIZE - strlen(chat.chat));
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
		EventManager::Get().AddEvent({ [&manager = CharacterManager::Get(), actorId] ()
		{
			manager.Move(actorId, static_cast<eMoveOper>(rand() % 4));
		}, seconds{ count } });
	}
	return 0;
}

int API_MoveRandomlyInBoundary(lua_State* L)
{
	int actorId = lua_toint(L, -2);
	int count = lua_toint(L, -1);
	lua_pop(L, 3);

	CharacterManager::Get().GetCharacters()[actorId]->GetPos();

	while (0 < count--)
	{
		EventManager::Get().AddEvent({ [&manager = CharacterManager::Get(), actorId] ()
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

int API_Move(lua_State* L)
{
	ID agentId = lua_toint(L, -3);
	Position to = { lua_toint(L, -2), lua_toint(L, -1) };
	lua_pop(L, 4);
	auto ret = CharacterManager::Get().GetCharacters()[agentId]->Move(to);
	lua_pushnumber(L, ret);
	return 1;
}

int API_NavigateAstar(lua_State* L)
{
	auto& pathFinder = AstarPathFinder::GetThreadLocal();
	ID agentId = lua_toint(L, -4);
	Position from = CharacterManager::Get().GetCharacters()[agentId]->GetPos();
	Position to = { lua_toint(L, -3), lua_toint(L, -2) };
	int step = lua_toint(L, -1);
	lua_pop(L, 5);
	pathFinder.InitAstar(from, to);

	for (int i = 1; i <= step; i++)
	{
		Position nextStepPos = pathFinder.GetWorldPos(i);
		if (nextStepPos.x < 0)
		{
			EventManager::Get().AddEvent({ [agentId]()
					{
						auto& m = CharacterManager::Get().GetCharacters()[agentId];
						auto L = m->GetScripts()[eScriptType::AI];
						lock_guard lck{ m->ScriptLock[eScriptType::AI] };
						lua_getglobal(L, "EventTargetMissing");
						lua_pcall(L, 0, 0, 0);
					}, seconds{ i } });
			return 0;
		}

		EventManager::Get().AddEvent({ [&manager = CharacterManager::Get(), agentId, nextStepPos] ()
			{
				auto pos = manager.GetCharacters()[agentId]->GetPos();
				if (abs(pos.x - nextStepPos.x) <= 1 && abs(pos.y - nextStepPos.y) <= 1)
				{
					manager.MoveForce(agentId, nextStepPos - pos);
				}
			}, seconds{ i } });

		if (nextStepPos == to)
		{
			step = i;
			break;
		}
	}

	EventManager::Get().AddEvent({ [agentId]()
		{
			auto& m = CharacterManager::Get().GetCharacters()[agentId];
			auto L = m->GetScripts()[eScriptType::AI];
			lock_guard lck{ m->ScriptLock[eScriptType::AI] };
			lua_getglobal(L, "EventNaviagateDone");
			lua_pcall(L, 0, 0, 0);
		}, seconds{ step } });

	return 0;
}

int API_Attack(lua_State* L)
{
	ID agentId = lua_toint(L, -3);
	ID targetId = lua_toint(L, -2);
	int damage = lua_toint(L, -1);
	lua_pop(L, 4);
	vector<ID> targets{ targetId };
	CharacterManager::Get().GetCharacters()[agentId]->Attack(targets, damage + damage * CharacterManager::Get().GetCharacters()[agentId]->GetLevel());
	return 0;
}

int API_DebugMessage(lua_State* L)
{
	int speakerId = lua_toint(L, -2);
	const char* mess = lua_tostring(L, -1);
	lua_pop(L, 3);
	cerr << speakerId << " :: " << mess << endl;
	return 0;
}

