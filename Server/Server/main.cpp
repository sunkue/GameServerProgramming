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
	SQLWCHAR* userName = new SQLWCHAR[50];
	SQLINTEGER* userId = new SQLINTEGER;
	SQLINTEGER* userLevel = new SQLINTEGER;
//	=> send event DB
	QueryRequest q;
	q.Query = L"EXEC SelectCharacterDataGreaterLevel -1"sv;
	q.Targets = new vector<any>(3);
	q.Targets[0] = any{ new SQLINTEGER };
	q.Targets[1] = make_any<SQLWCHAR>();
	q.Targets[2] = make_any<SQLINTEGER>();
	q.Func = [t = q.Targets]()
	{
		wcout << "[ ] " << *any_cast<SQLINTEGER*>(t[0]) << " :: "
			<< *any_cast<SQLWCHAR*>(t[1]) << " :: "
			<< *any_cast<SQLINTEGER*>(t[2]) << endl;
	};

	//=> Do Query and func
	DataBase::Get().ExecuteQuery(L"EXEC SelectCharacterDataGreaterLevel -1"sv, [&]()
		{
			wcout << "[ ] " << *userId << " :: " << userName << " :: " << *userLevel << endl;
		}, userId, userName, userLevel);
//	=> GQCP
	delete[] userName;
	delete userId;
	delete userLevel;

	vector<RaiiThread> workers; workers.reserve(thread::hardware_concurrency());
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
	workers.emplace_back([&]() { DataBase::Get().ProcessQueryQueueLoop(); });
	cout << "DB query ready" << endl;
	Server::Get().StartAccept();
	cout << "ready 2 accept" << endl;
	StartCommandLoop();
}