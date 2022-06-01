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
	//	=> send event DB
	{
		QueryRequest q;
		q.Query = L"EXEC SelectCharacterDataGreaterLevel -1"sv;
		q.Targets = new vector<any>(3);
		(*q.Targets)[0] = make_any<SQLINTEGER*>(new SQLINTEGER);
		(*q.Targets)[1] = make_any<SQLWCHAR*>(new SQLWCHAR[50]);
		(*q.Targets)[2] = make_any<SQLINTEGER*>(new SQLINTEGER);
		q.Func = [](const vector<any>& t)
		{
			wcout << "[ ] " << *any_cast<SQLINTEGER*>(t[0]) << " :: "
				<< any_cast<SQLWCHAR*>(t[1]) << " :: "
				<< *any_cast<SQLINTEGER*>(t[2]) << endl;
		};
		DataBase::Get().AddQueryRequest(move(q));
	}

	vector<RaiiThread> workers; workers.reserve(thread::hardware_concurrency());

	workers.emplace_back([&]() { DataBase::Get().ProcessQueryQueueLoop(); });
	cout << "DB query ready" << endl;
	return 0;


	for (int i = 0; i < workers.capacity() - 3; i++)
	{
#ifdef GQCPEX
		workers.emplace_back([&]() { Server::Get().ProcessQueuedCompleteOperationLoopEx(); });
#else
		workers.emplace_back([&]() { Server::Get().ProcessQueuedCompleteOperationLoop(); });
#endif // GQCPEX
	}
	CharacterManager::Get();
	workers.emplace_back([&]() { EventManager::Get().ProcessEventQueueLoop(); });
	cout << "event manager ready" << endl;

	Server::Get().StartAccept();
	cout << "ready 2 accept" << endl;
	StartCommandLoop();
}