#pragma once
#include "Obj.h"
#include "KeyboardEvent.h"

class Player : public DynamicObj
{
public:
	Player(Position pos = {}) :DynamicObj{ OBJ_TYPE::Wknight, pos } {};
	bool ProcessInput(const KEY_BOARD_EVENT_MANAGER::key_event& key);
};