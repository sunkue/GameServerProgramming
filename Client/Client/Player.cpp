#include "stdafx.h"
#include "Player.h"
#include "NetWorker.h"

bool Player::ProcessInput(const KEY_BOARD_EVENT_MANAGER::key_event& key)
{
	bool pressed = (key.action != GLFW_RELEASE);

	if (pressed)
	{
		switch (key.key)
		{
		case GLFW_KEY_UP: case GLFW_KEY_W:
		{
			cs_input pck;
			pck.input = eMoveOper::up;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_DOWN : case GLFW_KEY_S:
		{
			cs_input pck;
			pck.input = eMoveOper::down;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_RIGHT : case GLFW_KEY_D:
		{
			cs_input pck;
			pck.input = eMoveOper::right;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_LEFT : case GLFW_KEY_A:
		{
			cs_input pck;
			pck.input = eMoveOper::left;
			Networker::Get().DoSend(&pck);
		}
		break; default: return false;
		}
	}

	return true;
}


