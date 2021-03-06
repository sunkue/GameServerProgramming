#pragma comment (lib,"../../Share/Dependencies/LUA/lua54.lib")

extern "C" {
#include "../../Share/Dependencies/LUA/include/lauxlib.h"
#include "../../Share/Dependencies/LUA/include/lua.h"
#include "../../Share/Dependencies/LUA/include/lua.hpp"
#include "../../Share/Dependencies/LUA/include/luaconf.h"
#include "../../Share/Dependencies/LUA/include/lualib.h"
}

#include <iostream>
#include <fstream>
#include <string>
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
//	luaL_loadfile(AiScript_, "ex1.lua");

	string str; str.reserve(10000);
	ifstream basicGlobalDeclaration{ "Lua/monsterBasicDeclaration.lua" , ios::binary };
//	ifstream speech{ "Lua/monsterSpeech0.lua" , ios::binary };
	ifstream movement{ "Lua/monsterMovementRoaming.lua" , ios::binary };
	ifstream aggression{ "Lua/monsterAggressionPeace.lua" , ios::binary };
	str += string{ (std::istreambuf_iterator<char>(basicGlobalDeclaration)), std::istreambuf_iterator<char>() };
	//str += string{ (std::istreambuf_iterator<char>(speech)), std::istreambuf_iterator<char>() };
	str += string{ (std::istreambuf_iterator<char>(movement)), std::istreambuf_iterator<char>() };
	str += string{ (std::istreambuf_iterator<char>(aggression)), std::istreambuf_iterator<char>() };
	luaL_loadstring(AiScript_, str.c_str());
	lua_pcall(AiScript_, 0, 0, 0);


}


