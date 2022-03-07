#pragma once

enum class OBJ_TYPE : int
{
	Wlook = 0,
	Wknight = 1,
	Wbishop = 2,
	Wking = 3,
	Wqueen = 4,
	Wpawn = 5,

	Blook = 15,
	Bknight = 16,
	Bbishop = 17,
	Bking = 18,
	Bqueen = 19,
	Bpawn = 10,
};

class Obj
{
public:
	Obj(OBJ_TYPE type, glm::vec2 pos = glm::vec2(0)) :type_{ type }, pos_{ pos } {};

public:
	bool move(glm::vec2 diff) noexcept;

public:
	GET(pos);
	GET(type);

private:
	glm::vec2 pos_;
	OBJ_TYPE type_;
};

#include "KeyboardEvent.h"

class Player : public Obj
{
public:
	Player(glm::vec2 pos = glm::vec2(0)) :Obj{ OBJ_TYPE::Wknight,pos } {};

	bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key)
	{
		bool pressed = (key.action != GLFW_RELEASE);
		if (pressed)
		{
			switch (key.key)
			{
			case GLFW_KEY_UP: case GLFW_KEY_W:
			{
				move({ 0,-1 });
			}
			CASE GLFW_KEY_DOWN : case GLFW_KEY_S:
			{
				move({ 0, 1 });
			}
			CASE GLFW_KEY_LEFT : case GLFW_KEY_A:
			{
				move({ -1, 0 });
			}
			CASE GLFW_KEY_RIGHT : case GLFW_KEY_D:
			{
				move({ 1, 0 });
			}
			break; default: return false;
			}
		}

		return true;
	}
};