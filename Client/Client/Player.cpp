#include "stdafx.h"
#include "Player.h"
#include "NetWorker.h"

bool Player::process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key)
{
	bool pressed = (key.action != GLFW_RELEASE);
	if (pressed)
	{
		switch (key.key)
		{
		case GLFW_KEY_UP: case GLFW_KEY_W:
		{
			cs_input pck;
			pck.input = move_oper::up;
			Networker::get().do_send(&pck);
		}
		CASE GLFW_KEY_DOWN : case GLFW_KEY_S:
		{
			cs_input pck;
			pck.input = move_oper::down;
			Networker::get().do_send(&pck);
		}
		CASE GLFW_KEY_RIGHT : case GLFW_KEY_D:
		{
			cs_input pck;
			pck.input = move_oper::right;
			Networker::get().do_send(&pck);
		}
		CASE GLFW_KEY_LEFT : case GLFW_KEY_A:
		{
			cs_input pck;
			pck.input = move_oper::left;
			Networker::get().do_send(&pck);
		}
		break; default: return false;
		}
	}

	return true;
}
