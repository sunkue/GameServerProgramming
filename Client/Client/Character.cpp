#include "stdafx.h"
#include "Character.h"
#include "NetWorker.h"
#include "GameGui.h"
#include "Game.h"
#include "Chat.h"

bool Character::ProcessInput(const KeyboardEventManager::KeyEvent& key)
{
	bool pressed = (key.action != GLFW_RELEASE);
	bool press = (key.action == GLFW_PRESS);
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
		break; default: break;
		}
	}


	if (press)
	{
		if (key.key == GLFW_KEY_ENTER) ChatManager::Get().ToggleChatFocus();
		if (ChatManager::Get().GetIsChatFocused()) return true;
		
		switch (key.key)
		{
		CASE GLFW_KEY_A :
		{
			cs_use_skill pck;
			pck.skill = eSkill::attack;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_Z :
		{
			cs_use_skill pck;
			pck.skill = eSkill::accquireItem;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_X :
		{
			cs_use_skill pck;
			pck.skill = eSkill::explosion;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_C :
		{
			cs_use_skill pck;
			pck.skill = eSkill::windBooster;
			Networker::Get().DoSend(&pck);
		}
		CASE GLFW_KEY_I :
		{
			GameGuiManager::Get().ToggleShowMyInventory();
		}
		CASE GLFW_KEY_E :
		{
			GameGuiManager::Get().ToggleShowMyEquipment();
		}
		CASE GLFW_KEY_S :
		{
			GameGuiManager::Get().ToggleShowMyStatus();
		}
		CASE GLFW_KEY_K :
		{
			// GameGuiManager::Get().ToggleShowMySkill();
		}
		CASE GLFW_KEY_P :
		{
			GameGuiManager::Get().ToggleShowMyParty();
		}
		break; default: break;
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

