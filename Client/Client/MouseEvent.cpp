#include "stdafx.h"
#include "MouseEvent.h"

void MouseEventManager::Scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ScrollEvents_.emplace(xoffset, yoffset, clk::now());
}

void MouseEventManager::MouseButton(GLFWwindow* window, int key, int action, int modifiers)
{
	ButtonEvents_.emplace(key, action, modifiers, clk::now());
}

void MouseEventManager::CursorPosition(GLFWwindow* window, double xpos, double ypos)
{
	PosEvents_.emplace(xpos, ypos, clk::now());
}

void MouseEventManager::BindOverrideScrollFunc(OverrideScrollFunc func)
{
	OverrideScrollFunc_ = func;
}

void MouseEventManager::BindOverrideButtonFunc(OverrideButtonFunc func)
{
	OverrideButtonFunc_ = func;
}

void MouseEventManager::BindOverridePosFunc(OverridePosFunc func)
{
	OverridePosFunc_ = func;
}

void MouseEventManager::BindDefaultScrollFunc(DefaultScrollFunc func)
{
	DefaultScrollFunc_ = func;
}

void MouseEventManager::BindDefaultButtonFunc(DefaultButtonFunc func)
{
	DefaultButtonFunc_ = func;
}

void MouseEventManager::BindDefaultPosFunc(DefaultPosFunc func)
{
	DefaultPosFunc_ = func;
}

void MouseEventManager::ProcessInput()
{
	while (!ScrollEvents_.empty())
	{
		ScrollEvent& event = ScrollEvents_.front(); ScrollEvents_.pop();

		if (OverrideScrollFunc_(event))
		{
			continue;
		}

		DefaultScrollFunc_(event);
	}

	while (!ButtonEvents_.empty())
	{
		ButtonEvent& event = ButtonEvents_.front(); ButtonEvents_.pop();

		switch (event.button)
		{
		case GLFW_MOUSE_BUTTON_LEFT: { LeftClick_ = event.action; } break;
		case GLFW_MOUSE_BUTTON_RIGHT: { RightClick_ = event.action; } break;
		case GLFW_MOUSE_BUTTON_MIDDLE: { WheelClick_ = event.action; } break;
		}

		if (OverrideButtonFunc_(event))
		{
			continue;
		}

		DefaultButtonFunc_(event);
	}

	while (!PosEvents_.empty())
	{
		PosEvent& event = PosEvents_.front(); PosEvents_.pop();

		if (OverridePosFunc_(event))
		{
			PrevX_ = event.xPos;
			PrevY_ = event.yPos;
			continue;
		}
		DefaultPosFunc_(event);

		PrevX_ = event.xPos;
		PrevY_ = event.yPos;
	}
}
