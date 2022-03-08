#pragma once


class KEY_BOARD_EVENT_MANAGER
{
	SINGLE_TON(KEY_BOARD_EVENT_MANAGER) = default;

public:

	struct key_event
	{
		key_event(int key, int code, int action, int modifiers, clk::time_point time_of_event)
			:key{ key }, code{ code }, action{ action }, modifiers{ modifiers }, time_of_event{ time_of_event }{};
		int key, code, action, modifiers;
		clk::time_point time_of_event;
	};

	using key = int;
	using action = int;
	// main_func return true if wanna don't call other keyfunctions with the key.
	using main_key_func = std::function<bool(const key_event&)>;
	using key_func = std::function<void(const key_event&)>;

private:
	std::map<key, action> keys_;
	std::queue<key_event> key_events_;
	std::map<key, key_func> key_functions_;

private:
	main_key_func main_func_ = [](auto) { return false; };

public:
	void KeyBoard(GLFWwindow* window, int key, int code, int action, int modifiers);

	void BindKeyFunc(key key, key_func func);
	void BindMainKeyFunc(main_key_func func);

	void ProcessInput();
};
