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
	void OnRecvComplete(ID Id_, DWORD transfered);
	void OnSendComplete(ID Id_, ExpOverlapped* exover);
	void OnAcceptComplete(ExpOverlapped* exover);
	void OnDisconnectComplete(ID Id_, ExpOverlapped* exover);
public:
	void ProcessPacket(ID Id_, const void* const packet);
private:
	ID GetFreeId();
public:
	GET(Iocp);
	GET_REF_UNSAFE(Clients);
private:
	HANDLE Iocp_;
	array<ClientSession, MAX_PLAYER> Clients_;
};



