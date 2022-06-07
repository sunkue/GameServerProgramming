#include "stdafx.h"
#include "Server.h"
#include "TimerEvent.h"
#include "Character.h"
#include "DataBase.h"

struct RaiiThread
{
	template<class ...Args>
	RaiiThread(Args&& ... args) noexcept : _th{ std::move(args...) } {};
	RaiiThread(RaiiThread&&) noexcept = default;
	~RaiiThread() { if (_th.joinable()) { _th.join(); } };
	thread _th;
};

void StartCommandLoop()
{
	cout << "Command Loop Start" << endl;
	while (true)
	{
		cout << "Command [CompileScript / 0 / 0]" << endl;
		string str; cin >> str;
		if (str == "CompileScript")
		{
			cout << "Sectect CompileScript of [Player / Monster / Npc]" << endl;
			cin >> str;
			if (str == "Monster")
			{
				cout << "CompileScript of Monster Start.." << endl;
				auto& characters = CharacterManager::Get().GetCharacters();
				for (int i_ = 0; i_ < MAX_MONSTER; i_++)
				{
					int id = i_ + MAX_PLAYER;
					characters[id]->CompileScript();
				}
				cout << "CompileScript of Monster Done." << endl;
			}
		}
		cout << "Unknown Command." << endl;
	}
}

int main()
{
	vector<RaiiThread> workers; workers.reserve(thread::hardware_concurrency());

	Server::Get();
	DataBase::Get();
	CharacterManager::Get();

	auto WORKER_THREAD_NUM = workers.capacity() - 3;
	for (int i = 0; i < WORKER_THREAD_NUM; i++)
	{
#ifdef GQCPEX
		workers.emplace_back([&]() { Server::Get().ProcessQueuedCompleteOperationLoopEx(); });
#else
		workers.emplace_back([&]() { Server::Get().ProcessQueuedCompleteOperationLoop(); });
#endif // GQCPEX
}
	cout << "worker thread ready (" << WORKER_THREAD_NUM << ")" << endl;

	workers.emplace_back([&]() { DataBase::Get().ProcessQueryQueueLoop(); });
	cout << "DB query thread ready(1)" << endl;

	workers.emplace_back([&]() { EventManager::Get().ProcessEventQueueLoop(); });
	cout << "event manager thread ready (1)" << endl;

	Server::Get().StartAccept();
	cout << "ready 2 accept" << endl;

	StartCommandLoop();
}