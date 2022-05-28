#include "stdafx.h"
#include "Script.h"

Scriptable::~Scriptable()
{
	for (auto& s : Scripts_)
	{
		lock_guard lck{ ScriptLock[s.first] };
		lua_close(s.second);
	}
};
