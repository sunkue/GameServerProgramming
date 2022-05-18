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
	lua_State* L = luaL_newstate();	//  루아 오픈
	luaL_openlibs(L);				//  루아 라이브러리 오픈

	luaL_dofile(L, "ex1.lua");		//  스크립트파일 실행
//	luaL_loadfile(L, "ex1.lua");	//  스크립트파일 로드
//	lua_pcall(L, 0, 0, 0);			//  프로그램 실행
	lua_getglobal(L, "pos_x");		//  스택에 ""값 쌓기
	lua_getglobal(L, "pos_y");		//  스택에 ""값 쌓기
	x = (int)lua_tonumber(L, -2);	//  스택N번째에서 peek
	y = (int)lua_tonumber(L, -1);	//  스택N번째에서 peek
	printf("X %d, Y %d\n", x, y);
	lua_pop(L, 2);					//  스택에서 n개 제거
	lua_close(L);					//  루아 닫기
}


