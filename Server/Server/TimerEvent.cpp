#include "stdafx.h"
#include "TimerEvent.h"
#include "Server.h"

void TimerEvent::ProcessEventQueueLoop()
{
	HANDLE iocp = Server::Get().GetIocp();
	while (true)
	{
		this_thread::sleep_for(1s);
		while (!EventQueue_.empty())
		{
			auto now = clk::now();
			Event e = EventQueue_.top();
			if (now < e.ActionTime)
				break;
			EventQueue_.pop();
			auto exover = new EventExpOverlapped{ e.EventFunc };
			PostQueuedCompletionStatus(iocp, 0, 0, &exover->Over);
		}
	}
};
