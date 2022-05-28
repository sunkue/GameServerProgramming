#pragma once

enum class eScriptType
{
	AI,
};

class Scriptable
{
public:
	virtual ~Scriptable();
	virtual void CompileScript() {};
	GET_REF_UNSAFE(Scripts);
public:
	map<eScriptType, mutex> ScriptLock;
protected:
	map<eScriptType, lua_State*> Scripts_;
};

///////////////////////////////////////////

#define lua_toint(L, i ) static_cast<int>(lua_tonumber(L, i))

#include "ScriptApi.h"