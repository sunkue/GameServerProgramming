#pragma once
#include "ClientSession.h"
#include "GameLogic.h"

class Server
{
	SINGLE_TON(Server);
	~Server();

private:
	SOCKET socket;
	SOCKADDR_IN server_addr;
public:
	void loop_accept();

public:
	static void CALLBACK cb_recv(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag);
	static void CALLBACK cb_send(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag);

public:
	void process_packet(SOCKET id, const void* const packet);

public:
	unordered_map<SOCKET, class ClientSession> clients;
	GameLogic& game = GameLogic::get();

public:
	// 네트워크 최적화 용
	//array<SOCKET, MAX_PLAYER> idtosocket;
};



