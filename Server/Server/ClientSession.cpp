#include "stdafx.h"
#include "ClientSession.h"
#include "Server.h"

EXPOVERLAPPED::EXPOVERLAPPED(COMP_OP op, const void* const packet)
	: op{ op }
{
	wsabuf.buf = buf.data();
	wsabuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	memcpy(wsabuf.buf, packet, wsabuf.len);
}

EXPOVERLAPPED::EXPOVERLAPPED(COMP_OP op)
	: op{ op }
{
	wsabuf.buf = buf.data();
	wsabuf.len = static_cast<ULONG>(MAX_BUFFER_SIZE);
}

///////////////////////////////////////////////////////////

ClientSession::~ClientSession()
{
	sc_remove_obj remove;
	remove.id = id;
	for (auto& c : Server::get().get_clients())
	{
		c.second.do_send(&remove);
	}

	cerr << "diconnect::" << (int)id << "::" << endl;
	int res = ::closesocket(socket);
	SocketUtil::CheckError(res);
};

void ClientSession::do_recv()
{
	ZeroMemory(&recv_over, sizeof(WSAOVERLAPPED));
	recv_over.wsabuf.buf = recv_over.buf.data() + prerecv_size;
	recv_over.wsabuf.len = sizeof(recv_over.buf) - prerecv_size;
	DWORD recv_flag = 0;
	int res = ::WSARecv(socket, &recv_over.wsabuf, 1, 0, &recv_flag, &recv_over.over, nullptr);
	SocketUtil::CheckError(res);
}

void ClientSession::do_send(const void* const packet)
{
	EXPOVERLAPPED* over = new EXPOVERLAPPED{ COMP_OP::OP_SEND, packet };
	int ret = ::WSASend(socket, &over->wsabuf, 1, nullptr, 0, &over->over, nullptr);
	SocketUtil::CheckError(ret);
}


void ClientSession::do_disconnect()
{
	auto exover = new EXPOVERLAPPED{ COMP_OP::OP_DISCONNECT };
	PostQueuedCompletionStatus(Server::get().get_iocp(), 0, id, &exover->over);
}