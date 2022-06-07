#include "stdafx.h"
#include "Character.h"
#include "NetWorker.h"
#include "GameGui.h"
#include "Game.h"

string VisualizationId(ID id)
{
	if (id < MAX_PLAYER) return Game::Get().GetCharacters()[id].GetName();
	else if (id < MAX_PLAYER + MAX_MONSTER) return "M_" + to_string(id - MAX_PLAYER);
	else if (id < MAX_PLAYER + MAX_MONSTER + MAX_NPC) return "NPC_" + to_string(id - MAX_PLAYER - MAX_MONSTER);
	else if (id < MAX_OBJECT) return "OBSTRACLE_" + to_string(id - MAX_PLAYER - MAX_MONSTER - MAX_NPC);
	else return "ERR_ID";
}

bool Character::ProcessInput(const KeyboardEventManager::KeyEvent& key)
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
		CASE GLFW_KEY_DOWN :
		{
			cs_input pck;
			pck.input = eMoveOper::down;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_RIGHT :
		{
			cs_input pck;
			pck.input = eMoveOper::right;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_LEFT :
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

bool Character::ProcessInput(const MouseEventManager::ScrollEvent& scroll)
{
	auto& mm = MouseEventManager::Get();
	return false;
}

bool Character::ProcessInput(const MouseEventManager::ButtonEvent& button)
{
	auto& mm = MouseEventManager::Get();

	if (GLFW_MOUSE_BUTTON_LEFT == button.button)
	{
		if (GLFW_PRESS == button.action)
		{
			GameGuiManager::Get().SetSelectedObjId(-1);
		}
	}

	if (GLFW_MOUSE_BUTTON_MIDDLE == button.button)
	{
		if (GLFW_PRESS == button.action)
		{
		}
	}

	if (GLFW_MOUSE_BUTTON_RIGHT == button.button)
	{
		if (GLFW_PRESS == button.action)
		{
			auto targetTileDiff = GetTileIdxFromWindowPos(mm.GetPrevX(), mm.GetPrevY()) - Position{ 10,10 };
			auto targetPos = targetTileDiff + GetPos();
			for (auto& obj : Game::Get().GetCharacters())
			{
				if (targetPos == obj.second.GetPos())
				{
					GameGuiManager::Get().SetSelectedObjId(obj.first);
					return false;
				}
			}
			GameGuiManager::Get().SetSelectedObjId(-1);
		}
	}
	return true;
}

bool Character::ProcessInput(const MouseEventManager::PosEvent& pos)
{
	auto& mm = MouseEventManager::Get();
	return true;
}

