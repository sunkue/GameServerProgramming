#pragma once

class Timer
{
	friend class System;

	SINGLE_TON(Timer) = default;
public:
	milliseconds tick_time()
	{
		return elapsed_time_;
	}

	float tick_timef()
	{
		return elapsed_time_.count() / 1000.f;
	}

	milliseconds game_time()
	{
		return game_time_;
	}

private:
	void tick()
	{
		auto now = clk::now();
		elapsed_time_ = duration_cast<milliseconds>(now - prev_tp_);
		game_time_ += elapsed_time_; // should be overflow
		prev_tp_ = now;
	};

private:
	milliseconds elapsed_time_{};
	milliseconds game_time_{};
	clk::time_point prev_tp_{ clk::now() };
};


class System
{
	SINGLE_TON(System) = default;

public:
	void update()
	{
		timer.tick();
	}

	struct Screen
	{
		const int width{ 520 };
		const int height{ 520 };
	};
public:
	Timer& timer = Timer::get();
	GLFWwindow* window = nullptr;
	Screen screen;
};