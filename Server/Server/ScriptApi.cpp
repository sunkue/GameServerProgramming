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

int API_ReturnToStartPoint(lua_State* L)
{
	ID agentId = lua_toint(L, -1);
	lua_pop(L, 2);
	bool finish{ true };
	// return to StartPoint_.
	lua_pushboolean(L, finish);
	return 1;
}

int API_NavigateAstar(lua_State* L)
{
	Position from = { lua_toint(L, -4), lua_toint(L, -3) };
	Position to = { lua_toint(L, -2), lua_toint(L, -1) };
	lua_pop(L, 5);
	Position navigatePos{ from };
	lua_pushnumber(L, navigatePos.x);
	lua_pushnumber(L, navigatePos.y);
	return 2;
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

