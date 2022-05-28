#include "stdafx.h"
#include "Script.h"
#include "Server.h"
#include "Character.h"
#include "World.h"
#include "TimerEvent.h"


int API_SendMessage(lua_State* L)
{
	int clinetId = lua_toint(L, -4);
	int speakerId = lua_toint(L, -3);
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

int API_GetPos(lua_State* L)
{
	int objectId = lua_toint(L, -1);
	lua_pop(L, 2);

	auto pos = CharacterManager::Get().GetPosition(objectId);

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}