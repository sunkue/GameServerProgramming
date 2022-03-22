#include "stdafx.h"
#include "ClientSession.h"
#include "Server.h"

EXPOVERLAPPED::EXPOVERLAPPED(const void* const packet)
{
	send_wsabuf.buf = send_buf.data();
	send_wsabuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	memcpy(send_wsabuf.buf, packet, send_wsabuf.len);
}

///////////////////////////////////////////////////////////

ClientSession::~ClientSession()
{
	// 필요없을지도
	int res = ::closesocket(socket);
	SocketUtil::CheckError(res);

	sc_remove_obj remove;
	remove.id = socket;
	for (auto& c : Server::get().clients)
	{
		c.second.do_send(&remove);
	}
};

void ClientSession::do_recv()
{
	recv_buf.fill(NULL);
	ZeroMemory(&recv_over, sizeof(WSAOVERLAPPED));
	recv_over.hEvent = reinterpret_cast<HANDLE>(id_);
	DWORD recv_flag = 0;
	int ret = WSARecv(socket, &recv_wsabuf, 1, nullptr, &recv_flag, &recv_over, Server::cb_recv);
	SocketUtil::CheckError(ret);
}

void ClientSession::do_send(const void* const packet)
{
	EXPOVERLAPPED* over = new EXPOVERLAPPED(packet);

	over->send_over.hEvent = reinterpret_cast<HANDLE>(id_);

	int ret = WSASend(socket, &over->send_wsabuf, 1, nullptr, 0, &over->send_over, Server::cb_send);
	SocketUtil::CheckError(ret);
}


