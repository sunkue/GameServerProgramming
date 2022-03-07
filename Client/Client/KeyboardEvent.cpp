#include "stdafx.h"
#include "KeyboardEvent.h"

void KEY_BOARD_EVENT_MANAGER::KeyBoard(GLFWwindow* window, int key, int code, int action, int modifiers)
{
	key_events_.emplace(key, code, action, modifiers, clk::now());
}

void KEY_BOARD_EVENT_MANAGER::BindKeyFunc(key key, key_func func)
{
	key_functions_[key] = func;
}

void KEY_BOARD_EVENT_MANAGER::BindMainKeyFunc(main_key_func func)
{
	main_func_ = func;
}

void KEY_BOARD_EVENT_MANAGER::ProcessInput()
{
	while (!key_events_.empty())
	{
		key_event& event = key_events_.front(); key_events_.pop();

		keys_[event.key] = event.action;

		if (main_func_(event))
		{
			continue;
		}
		
		if (key_functions_.count(event.key))
		{
			key_functions_[event.key](event);
		}
	}
}
