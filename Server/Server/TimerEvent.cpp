#include "stdafx.h"
#include "TimerEvent.h"
#include "Server.h"

void EventManager::ProcessEventQueueLoop()
{
	HANDLE iocp = Server::Get().GetIocp();
	while (true)
	{
		this_thread::sleep_for(100ms);
		Event e{ []() {},{} };
		auto now = clk::now();
		while (EventQueue_.try_pop(e))
		{
			 // e = EventQueue_.top();
			if (now <= e.ActionTime)
			{
				EventQueue_.push(e);
				break;
			}

			// EventQueue_.pop();
			auto exover = new EventExpOverlapped{ e.EventFunc };
			PostQueuedCompletionStatus(iocp, 0, 0, &exover->Over);
		}
	}
};
