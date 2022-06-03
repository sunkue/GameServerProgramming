#pragma once


class KeyboardEventManager
{
	SINGLE_TON(KeyboardEventManager) = default;

public:

	struct KeyEvent
	{
		KeyEvent(int key, int code, int action, int modifiers, clk::time_point time_of_event)
			:key{ key }, code{ code }, action{ action }, modifiers{ modifiers }, eventTime{ time_of_event }{};
		int key, code, action, modifiers;
		clk::time_point eventTime;
	};

	using Key = int;
	using Action = int;
	// main_func return true if wanna don't call other keyfunctions with the key.
	using OverrideKeyFunc = std::function<bool(const KeyEvent&)>;
	using KeyFunc = std::function<void(const KeyEvent&)>;

private:
	std::map<Key, Action> KeyActions_;
	std::queue<KeyEvent> KeyEvents_;
	std::map<Key, KeyFunc> KeyFunctions_;

private:
	OverrideKeyFunc OverrideKeyFunc_ = [](auto) { return false; };

public:
	void Keyboard(GLFWwindow* window, int key, int code, int action, int modifiers);

	void BindKeyFunc(Key key, KeyFunc func);
	void BindMainKeyFunc(OverrideKeyFunc func);

	void ProcessInput();
};
