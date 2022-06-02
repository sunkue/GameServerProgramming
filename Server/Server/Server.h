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
	void OnRecvComplete(NetID Id_, DWORD transfered);
	void OnSendComplete(NetID Id_, ExpOverlapped* exover);
	void OnAcceptComplete(ExpOverlapped* exover);
	void OnDisconnectComplete(NetID Id_, ExpOverlapped* exover);
	void OnEventTimerComplete(ExpOverlapped* exover);
	void OnDataBaseQueryComplete(ExpOverlapped* exover);
public:
	void ProcessPacket(NetID Id_, const void* const packet);
private:
	NetID GetFreeId();
public:
	GET(Iocp);
	GET_REF_UNSAFE(Clients);
private:
	HANDLE Iocp_;
	array<ClientSession, MAX_PLAYER> Clients_;
};



