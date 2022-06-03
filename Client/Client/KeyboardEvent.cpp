#include "stdafx.h"
#include "KeyboardEvent.h"

void KeyboardEventManager::Keyboard(GLFWwindow* window, int key, int code, int action, int modifiers)
{
	KeyEvents_.emplace(key, code, action, modifiers, clk::now());
}

void KeyboardEventManager::BindKeyFunc(Key key, KeyFunc func)
{
	KeyFunctions_[key] = func;
}

void KeyboardEventManager::BindMainKeyFunc(OverrideKeyFunc func)
{
	OverrideKeyFunc_ = func;
}

void KeyboardEventManager::ProcessInput()
{
	while (!KeyEvents_.empty())
	{
		KeyEvent& event = KeyEvents_.front(); KeyEvents_.pop();

		KeyActions_[event.key] = event.action;

		if (OverrideKeyFunc_(event))
		{
			continue;
		}
		
		if (KeyFunctions_.count(event.key))
		{
			KeyFunctions_[event.key](event);
		}
	}
}
