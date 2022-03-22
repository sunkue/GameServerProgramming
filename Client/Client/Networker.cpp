#include "stdafx.h"
#include "Networker.h"
#include "Game.h"

Networker::Networker()
{
	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
	SocketUtil::CheckError(ret);
	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr; ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	string sever_ip; cout << "서버 IP를 입력하세요 :: "; cin >> sever_ip;
	inet_pton(AF_INET, sever_ip.c_str(), &server_addr.sin_addr);
	ret = WSAConnect(socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), nullptr, nullptr, nullptr, nullptr);
	SocketUtil::CheckError(ret);
}

void Networker::do_recv()
{
	static WSABUF mybuf{ static_cast<ULONG>(recv_buf.size()), recv_buf.data() };
	mybuf.buf = recv_buf.data();
	mybuf.len = static_cast<ULONG>(recv_buf.size());
	static WSAOVERLAPPED recv_over; ZeroMemory(&recv_over, sizeof(WSAOVERLAPPED));
	DWORD recv_flag = 0;
	int ret = WSARecv(socket, &mybuf, 1, nullptr, &recv_flag, &recv_over, Networker::cb_recv);
	SocketUtil::CheckError(ret);
}

void Networker::do_send(const void* const packet)
{
	static WSABUF mybuf; mybuf.buf = reinterpret_cast<char*>(const_cast<void*>(packet));
	mybuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	DWORD send_bytes;
	int ret = WSASend(socket, &mybuf, 1, &send_bytes, 0, 0, 0);
	SocketUtil::CheckError(ret);
}

void Networker::cb_recv(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag)
{
	auto& nw = Networker::get();
	nw.process_packet(nw.recv_buf.data());
	nw.do_recv();
}

void Networker::start()
{
	cs_hi hi; do_send(&hi);
	do_recv();
}

///////////////////////////////////////////////////////////////////////////////////////

void Networker::process_packet(const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	auto& game = Game::get();

	switch (packet_type)
	{
	case PACKET_TYPE::SC_HI:
	{
		auto pck = reinterpret_cast<const sc_hi*>(packet);
		game.set_n(pck->n);
		game.set_id(pck->id);
		game.get_players()[game.get_id()];
		game.init();
		cout << "hi" << endl;
	}
	CASE PACKET_TYPE::SC_READY :
	{
		ready_ = true;
		cout << "ready" << endl;
	}
	CASE PACKET_TYPE::SC_SET_POSITION :
	{
		auto pck = reinterpret_cast<const sc_set_position*>(packet);
		auto pos = decltype(pck->pos)::decode<glm::vec2>(pck->pos);
		auto id = pck->id;
		game.get_players()[id].set_pos(pos);
		cout << game.get_players().size() << endl;
	}
	break; default: break;
	}
}
