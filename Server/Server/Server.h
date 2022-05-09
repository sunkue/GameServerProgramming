#pragma once
#include "ClientSession.h"
#include "ListenSocket.h"

class Server
{
	SINGLE_TON(Server);
	~Server();
public:
	void ProcessQueuedCompleteOperationLoop();
	void ProcessQueuedCompleteOperationLoopEx();
	void StartAccept();
	void RepeatSendLoop(milliseconds repeat_time = 50ms);
private:
	void OnRecvComplete(ID id, DWORD transfered);
	void OnSendComplete(ID id, ExpOverlapped* exover);
	void OnAcceptComplete(ExpOverlapped* exover);
	void OnDisconnectComplete(ID id, ExpOverlapped* exover);
public:
	void ProcessPacket(ID id, const void* const packet);
private:
	ID get_free_id();
public:
	auto get_iocp() { return iocp; };
	auto& get_clients() { return clients; }
private:
	HANDLE iocp;
	array<ClientSession, MAX_PLAYER> clients;
//	unordered_map<ID, class ClientSession> clients;
};



