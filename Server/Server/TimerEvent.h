#pragma once

struct Event
{
	Event(function<void()> f, milliseconds ms)
		: EventFunc{ f }, ActionTime{ clk::now() + ms }{}
	function<void()> EventFunc;
	clk::time_point ActionTime;
};

constexpr bool operator<(const Event& l, const Event& r) { return l.ActionTime < r.ActionTime; }
constexpr bool operator>(const Event& l, const Event& r) { return l.ActionTime > r.ActionTime; }


class EventManager
{
	SINGLE_TON(EventManager) = default;
public:
	void AddEvent(const Event& e) { EventQueue_.push(e); }
	void ProcessEventQueueLoop();
private:
	concurrent_priority_queue<Event, greater<Event>> EventQueue_{ 5000000 };
};

