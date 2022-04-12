#include "stdafx.h"
#include "ClientSession.h"
#include "Server.h"

ExpOverlapped::ExpOverlapped(COMP_OP op, const void* const packet)
	: op{ op }
{
	wsabuf.buf = buf.data();
	wsabuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	memcpy(wsabuf.buf, packet, wsabuf.len);
}

ExpOverlapped::ExpOverlapped(COMP_OP op)
	: op{ op }
{
	wsabuf.buf = buf.data();
	wsabuf.len = static_cast<ULONG>(MAX_BUFFER_SIZE);
}

///////////////////////////////////////////////////////////

ClientSession::~ClientSession()
{

};

void ClientSession::init(SOCKET s)
{
	socket = s;
}

void ClientSession::do_recv()
{
	if (IsFree())
		return;
	ZeroMemory(&recv_over.over, sizeof(WSAOVERLAPPED));
	recv_over.wsabuf.buf = recv_over.buf.data() + prerecv_size;
	recv_over.wsabuf.len = sizeof(recv_over.buf) - prerecv_size;
	DWORD recv_flag = 0;
	int res = ::WSARecv(socket, &recv_over.wsabuf, 1, 0, &recv_flag, &recv_over.over, nullptr);
	SocketUtil::CheckError(res);
}

void ClientSession::do_send(const void* const packet)
{
	if (IsFree())
		return;
	ExpOverlapped* over = new ExpOverlapped{ COMP_OP::OP_SEND, packet };
	int ret = ::WSASend(socket, &over->wsabuf, 1, nullptr, 0, &over->over, nullptr);
	SocketUtil::CheckError(ret);
}


void ClientSession::do_disconnect()
{
	if (IsFree())
		return;
	state = SESSION_STATE::FREE;
	auto exover = new ExpOverlapped{ COMP_OP::OP_DISCONNECT };
	PostQueuedCompletionStatus(Server::get().get_iocp(), 0, id, &exover->over);
}