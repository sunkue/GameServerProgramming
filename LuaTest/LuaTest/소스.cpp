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

int main()
{
	int x, y;
	lua_State* L = luaL_newstate();	//  ��� ����
	luaL_openlibs(L);				//  ��� ���̺귯�� ����

	luaL_dofile(L, "ex1.lua");		//  ��ũ��Ʈ���� ����
//	luaL_loadfile(L, "ex1.lua");	//  ��ũ��Ʈ���� �ε�
//	lua_pcall(L, 0, 0, 0);			//  ���α׷� ����
	lua_getglobal(L, "pos_x");		//  ���ÿ� ""�� �ױ�
	lua_getglobal(L, "pos_y");		//  ���ÿ� ""�� �ױ�
	x = (int)lua_tonumber(L, -2);	//  ����N��°���� peek
	y = (int)lua_tonumber(L, -1);	//  ����N��°���� peek
	printf("X %d, Y %d\n", x, y);
	lua_pop(L, 2);					//  ���ÿ��� n�� ����
	lua_close(L);					//  ��� �ݱ�
}


