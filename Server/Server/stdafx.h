#pragma once

using namespace std;

#include "../../Share/Share.h"

#pragma comment(lib, "MSWSock.lib")
#include <MSWSock.h>

#pragma comment (lib,"../../Share/Dependencies/LUA/lua54.lib")

extern "C" {
#include "../../Share/Dependencies/LUA/include/lauxlib.h"
#include "../../Share/Dependencies/LUA/include/lua.h"
#include "../../Share/Dependencies/LUA/include/lua.hpp"
#include "../../Share/Dependencies/LUA/include/luaconf.h"
#include "../../Share/Dependencies/LUA/include/lualib.h"
}