#pragma comment (lib,"../../Share/Dependencies/LUA/lua54.lib")

extern "C" {
#include "../../Share/Dependencies/LUA/include/lauxlib.h"
#include "../../Share/Dependencies/LUA/include/lua.h"
#include "../../Share/Dependencies/LUA/include/lua.hpp"
#include "../../Share/Dependencies/LUA/include/luaconf.h"
#include "../../Share/Dependencies/LUA/include/lualib.h"
}

#include <iostream>

using namespace std;


int addnum_c(lua_State* L)
{
	int a = (int)lua_tonumber(L, -2);
	int b = (int)lua_tonumber(L, -1);
	lua_pop(L, 3);
	int result = a + b;
	lua_pushnumber(L, result);
	return 1;
}

int API_SendMessage(lua_State* L)
{
	cout << "@@" << endl;
	int clinetId = lua_tonumber(L, -3);
	int npcId = lua_tonumber(L, -2);
	const char* mess = lua_tostring(L, -1);
	lua_pop(L, 4);

	cout << "PAPI_SendMessage" << endl;
	cout << clinetId << endl;
	cout << npcId << endl;
	cout << mess << endl;
	// Server::Get().GetClients()[clinetId].DoSend();
	return 0;
}

int API_GetPos(lua_State* L)
{
	cout << "!!" << endl;
	int objectId = lua_tonumber(L, -1);
	lua_pop(L, 2);
	lua_pushnumber(L, 11);
	lua_pushnumber(L, 22);
	cout << "POS OID" << objectId << endl;
	return 2;
}

int F(lua_State* L)
{
	int ID = lua_tonumber(L, -1);
	cout << "TESTERER " << ID << endl;
	return 0;
}

int main()
{
	lua_State* AiScript_ = luaL_newstate();
	luaL_openlibs(AiScript_);
	luaL_dofile(AiScript_, "ex1.lua");
//	luaL_loadfile(AiScript_, "ex1.lua");
//	lua_pcall(AiScript_, 0, 0, 0);

	lua_register(AiScript_, "F", F);
	lua_register(AiScript_, "API_Chat", API_SendMessage);
	lua_register(AiScript_, "API_GetPos", API_GetPos);

	lua_getglobal(AiScript_, "SetObjectId");
	lua_pushnumber(AiScript_, 111);
	lua_pcall(AiScript_, 1, 0, 0);

	lua_getglobal(AiScript_, "EventPlayerMove");
	lua_pushnumber(AiScript_, 33);
	lua_pcall(AiScript_, 1, 0, 0);


	lua_getglobal(AiScript_, "F");
	lua_getglobal(AiScript_, "myId");
	lua_pcall(AiScript_, 1, 0, 0);
}


