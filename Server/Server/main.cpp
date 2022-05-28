#include "stdafx.h"
#include "Server.h"
#include "TimerEvent.h"
#include "Character.h"

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
	for (int i = 0; i < workers.capacity() - 2; i++)
	{
#ifdef GQCPEX
		workers.emplace_back([&]() { Server::Get().ProcessQueuedCompleteOperationLoopEx(); });
#else
		workers.emplace_back([&]() { Server::Get().ProcessQueuedCompleteOperationLoop(); });
#endif // GQCPEX
	}
	CharacterManager::Get();
	Server::Get().StartAccept();
	cout << "ready 2 accept" << endl;
	workers.emplace_back([&]() { EventManager::Get().ProcessEventQueueLoop(); });
	cout << "event manager ready" << endl;
	StartCommandLoop();
}