#include "stdafx.h"
#include "Server.h"
#include "ClientSession.h"
#include "GameLogic.h"
#include "GameBoard.h"

/////////////////////////////////////////////////////////////////////////////////////////

void Server::process_packet(SOCKET id, const void* const packet)
{
	auto SND2ME = [this, id](const void* const packet)
	{
		clients[id].do_send(packet);
	};

	auto SND2OTHERS = [this, id](const void* const packet)
	{
		for (auto& c : clients)
		{
			if (c.second.get_id() != id) { c.second.do_send(packet); }
		}
	};

	auto SND2EVERY = [this](const void* const packet)
	{
		for (auto& c : clients)
		{
			c.second.do_send(packet);
		}
	};

	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	switch (packet_type)
	{
		CASE PACKET_TYPE::CS_HI :
		{
			{
				sc_hi hi;
				hi.n = GameBoard::get().get_n();
				hi.id = NetID(id);
				SND2ME(&hi);
			}

			for (auto& c : clients)
			{
				cout << clients.size() << endl;
				auto other_id = c.first;
				auto pos = game.get_position(other_id);
				sc_set_position other_pos;
				other_pos.id = NetID(other_id); /////
				other_pos.pos = decltype(other_pos.pos)::encode(pos);
				SND2ME(&other_pos);
			}

			{
				sc_set_position set_pos;
				set_pos.id = NetID(id); /////
				auto pos = game.get_position(id);
				set_pos.pos = decltype(set_pos.pos)::encode(pos);
				SND2EVERY(&set_pos);
			}

			{
				sc_ready ready;
				SND2ME(&ready);
				cout << "ready" << endl;
			}

		}
		CASE PACKET_TYPE::CS_INPUT :
		{
			auto pck = reinterpret_cast<const cs_input*>(packet);
			if (game.move(id, pck->input))
			{
				sc_set_position set_pos;
				auto pos = game.get_position(id);
				set_pos.id = NetID(id); /////
				set_pos.pos = decltype(set_pos.pos)::encode(pos);
				SND2EVERY(&set_pos);
			}
		}
	break; default: break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

Server::Server()
{
	WSADATA WSAData; int ret = WSAStartup(MAKEWORD(2, 2), &WSAData); SocketUtil::CheckError(ret);
	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = ::bind(socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	SocketUtil::CheckError(ret);

	ret = listen(socket, SOMAXCONN);
	SocketUtil::CheckError(ret);
}

Server::~Server()
{
	WSACleanup();
}

void Server::loop_accept()
{
	INT addr_size = sizeof(server_addr);

	for (SOCKET new_socket;;)
	{
		new_socket = WSAAccept(socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
		SocketUtil::CheckError(new_socket);
		char tcp_opt = 1;
		int res = ::setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_opt, sizeof(tcp_opt));
		SocketUtil::CheckError(res);

		clients.try_emplace(new_socket, new_socket);
		clients[new_socket].do_recv();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void Server::cb_recv(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag)
{
	auto socket = reinterpret_cast<SOCKET>(over->hEvent);
	auto& clients = Server::get().clients;
	auto& client = clients[socket];
	if (0 == transfered)
	{
		cout << " Client Disconnected" << endl;
		clients.erase(client.socket);
		return;
	}

	Server::get().process_packet(socket, client.recv_buf.data());

	client.do_recv();
}

void Server::cb_send(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag)
{
	auto& clients = Server::get().clients;
	auto& client = clients[reinterpret_cast<SOCKET>(over->hEvent)];
	auto exover = reinterpret_cast<EXPOVERLAPPED*>(over);
	auto pck = reinterpret_cast<const packet_base<void>*>(&exover->send_buf);
	delete exover;
}
