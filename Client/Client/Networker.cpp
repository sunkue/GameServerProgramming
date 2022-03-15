#include "stdafx.h"
#include "Networker.h"
#include "Game.h"



void Networker::do_recv()
{
	WSABUF mybuf; mybuf.buf = recv_buf.data(); mybuf.len = static_cast<ULONG>(recv_buf.size());
	DWORD recv_byte, recv_flag = 0;
	int ret = WSARecv(socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
	cout << "[ Server ] Sent [" << recv_byte << " bytes]" << endl;
	SocketUtil::CheckError(ret);
	process_packet(recv_buf.data());
	// recv_buf.fill(0);
}

void Networker::do_send(const void* const packet)
{
	WSABUF mybuf; mybuf.buf = reinterpret_cast<char*>(const_cast<void*>(packet));
	mybuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	DWORD sent_byte;
	cout << "send" ;
	int ret = WSASend(socket, &mybuf, 1, &sent_byte, 0, 0, 0);
	cout << "ed" ;
	cout << " [" << sent_byte << " bytes]" << endl;
	SocketUtil::CheckError(ret);
}

void Networker::start_recv()
{
	cs_hi hi; 
	do_send(&hi);

	while (false == stop_work)
	{
		do_recv();
	}
}

void Networker::process_packet(const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	switch (packet_type)
	{
	case PACKET_TYPE::NONE:
	{}
	CASE PACKET_TYPE::SC_HI :
	{
		auto pck = reinterpret_cast<const sc_hi*>(packet);
		Game::get().set_n(pck->n);
	}
	CASE PACKET_TYPE::SC_SET_POSITION :
	{
		auto pck = reinterpret_cast<const sc_set_position*>(packet);
		auto pos = decltype(sc_set_position::pos)::decode<glm::vec2>(pck->pos);
		Game::get().get_player()->set_pos(pos);
	}
	break; default: break;
	}
}
