#pragma once
#include "ClientSession.h"
#include "GameLogic.h"

class Server
{
	SINGLE_TON(Server)
	{
		WSADATA WSAData;
		int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
		SocketUtil::CheckError(ret);
		socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
		ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		ret = ::bind(socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
		SocketUtil::CheckError(ret);

		listen(socket, SOMAXCONN);
	}

	~Server() { WSACleanup(); };

public:
	void accept()
	{
		INT addr_size = sizeof(server_addr);
		SOCKET new_socket = WSAAccept(socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
		SocketUtil::CheckError(new_socket);

		clients.at(0) = ClientSession{ new_socket, 0 };
	}

	void do_recv() // loop
	{
		auto& client = clients.at(0);

		for (;;)
		{
			client.do_recv();
		}
	}

	void process_packet(NetID id, const void* const packet);

private:
	SOCKET socket;
	SOCKADDR_IN server_addr;
private:
	GameLogic& game = GameLogic::get();
	std::array<ClientSession, MAX_PLAYER> clients;
};
