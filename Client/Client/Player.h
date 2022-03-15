#pragma once
#include "Obj.h"
#include "KeyboardEvent.h"

class Player : public Obj
{
public:
	Player(glm::vec2 pos = glm::vec2(0)) :Obj{ OBJ_TYPE::Wknight,pos } {};

	bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key);
};