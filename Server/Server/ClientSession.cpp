#include "stdafx.h"
#include "ClientSession.h"
#include "Server.h"

void ClientSession::do_recv()
{
	WSABUF mybuf; mybuf.buf = recv_buf.data(); mybuf.len = static_cast<ULONG>(recv_buf.size());
	DWORD recv_byte, recv_flag = 0;
	cout << "RC" << endl;
	int ret = WSARecv(socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
	cout << "[ " << (int)id << " ] Sent [" << recv_byte << " bytes]" << endl;
	SocketUtil::CheckError(ret);
	Server::get().process_packet(id, recv_buf.data());
	// recv_buf.fill(0);
}

void ClientSession::do_send(const void* const packet)
{
	WSABUF mybuf; mybuf.buf = reinterpret_cast<char*>(const_cast<void*>(packet));
	mybuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	DWORD sent_byte;
	int ret = WSASend(socket, &mybuf, 1, &sent_byte, 0, 0, 0);
	SocketUtil::CheckError(ret);
}
