struct lua_State;

int API_SendMessage(lua_State* L);
int API_MoveRandomly(lua_State* L);
int API_GetPos(lua_State* L);
int API_Move(lua_State* L);
int API_ReturnToStartPoint(lua_State* L);
int API_NavigateAstar(lua_State* L);
int API_Attack(lua_State* L);
int API_DebugMessage(lua_State* L);
