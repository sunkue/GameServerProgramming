#include "stdafx.h"
#include "Server.h"

struct RaiiThread
{
	template<class ...Args>
	RaiiThread(Args&& ... args) noexcept : _th{ std::move(args...) } {};
	RaiiThread(RaiiThread&&) noexcept = default;
	~RaiiThread() { if (_th.joinable()) { _th.join(); } };
	thread _th;
};

int main()
{
	vector<RaiiThread> workers; workers.reserve(thread::hardware_concurrency());
	for (int i = 0; i < workers.capacity(); i++)
	{
		workers.emplace_back([&]() { Server::get().ProcessQueuedCompleteOperationLoopEx(); });
	}
	Server::get().StartAccept();
}