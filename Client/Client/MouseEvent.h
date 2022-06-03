#pragma once


class MouseEventManager
{
	SINGLE_TON(MouseEventManager) = default;

public:

	struct ScrollEvent
	{
		double xOffset, yOffset;
		clk::time_point eventTime;
	};

	struct ButtonEvent
	{
		int button, action, modifiers;
		clk::time_point eventTime;
	};

	struct PosEvent
	{
		double xPos, yPos;
		clk::time_point eventTime;
	};

	using OverrideScrollFunc = std::function<bool(const ScrollEvent&)>;
	using DefaultScrollFunc = std::function<void(const ScrollEvent&)>;

	using OverrideButtonFunc = std::function<bool(const ButtonEvent&)>;
	using DefaultButtonFunc = std::function<void(const ButtonEvent&)>;

	using OverridePosFunc = std::function<bool(const PosEvent&)>;
	using DefaultPosFunc = std::function<void(const PosEvent&)>;

private:
	std::queue<ScrollEvent> ScrollEvents_;
	std::queue<ButtonEvent> ButtonEvents_;
	std::queue<PosEvent> PosEvents_;

private:
	DefaultScrollFunc DefaultScrollFunc_ = [](const auto&) { return; };
	DefaultButtonFunc DefaultButtonFunc_ = [](const auto&) { return; };
	DefaultPosFunc DefaultPosFunc_ = [](const auto&) { return; };

	OverrideScrollFunc  OverrideScrollFunc_ = [](const auto&) { return false; };
	OverrideButtonFunc  OverrideButtonFunc_ = [](const auto&) { return false; };
	OverridePosFunc OverridePosFunc_ = [](const auto&) { return false; };

public:
	GET(LeftClick);
	GET(RightClick);
	GET(WheelClick);
	GET(PrevX);
	GET(PrevY);

private:
	bool LeftClick_ = false;
	bool RightClick_ = false;
	bool WheelClick_ = false;

	double PrevX_{};
	double PrevY_{};

public:
	void Scroll(GLFWwindow* window, double xoffset, double yoffset);
	void MouseButton(GLFWwindow* window, int key, int action, int modifiers);
	void CursorPosition(GLFWwindow* window, double xpos, double ypos);

	void BindOverrideScrollFunc(OverrideScrollFunc func);
	void BindOverrideButtonFunc(OverrideButtonFunc func);
	void BindOverridePosFunc(OverridePosFunc func);

	void BindDefaultScrollFunc(DefaultScrollFunc func);
	void BindDefaultButtonFunc(DefaultButtonFunc func);
	void BindDefaultPosFunc(DefaultPosFunc func);

	void ProcessInput();
};

