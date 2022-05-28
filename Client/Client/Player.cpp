#include "stdafx.h"
#include "Player.h"
#include "NetWorker.h"

bool Character::ProcessInput(const KEY_BOARD_EVENT_MANAGER::key_event& key)
{
	bool pressed = (key.action != GLFW_RELEASE);

	if (pressed)
	{
		switch (key.key)
		{
		case GLFW_KEY_UP:
		{
			cs_input pck;
			pck.input = eMoveOper::up;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_DOWN:
		{
			cs_input pck;
			pck.input = eMoveOper::down;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_RIGHT:
		{
			cs_input pck;
			pck.input = eMoveOper::right;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_LEFT:
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


