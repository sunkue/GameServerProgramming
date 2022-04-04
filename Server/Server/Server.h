#pragma once
#include "ClientSession.h"
#include "GameLogic.h"
#include "ListenSocket.h"

class Server
{
	SINGLE_TON(Server);
	~Server();
public:
	void ProcessQueuedCompleteOperationLoop();
	//	void RepeatSendLoop(milliseconds repeat_time = 50ms);
private:
	void OnRecvComplete(ID id, DWORD transfered);
	void OnSendComplete(ID id, EXPOVERLAPPED* exover);
	void OnAcceptComplete(EXPOVERLAPPED* exover);
	void OnDisconnectComplete(ID id, EXPOVERLAPPED* exover);
public:
	void process_packet(ID id, const void* const packet);
public:
	auto get_iocp() { return iocp; };
	auto& get_clients() { return clients; }
private:
	HANDLE iocp;
	unordered_map<ID, class ClientSession> clients;
};



