#pragma once

struct Event
{
	Event(function<void()> f, milliseconds ms)
		: EventFunc{ f }, ActionTime{ clk::now() + ms }{}
	function<void()> EventFunc;
	clk::time_point ActionTime;
};

constexpr bool operator<(const Event& l, const Event& r) { return l.ActionTime < r.ActionTime; }


class TimerEvent
{
	SINGLE_TON(TimerEvent) = default;
public:
	void AddEvent(Event e) { EventQueue_.push(e); }
	void ProcessEventQueueLoop();
private:
	priority_queue<Event> EventQueue_;
};

