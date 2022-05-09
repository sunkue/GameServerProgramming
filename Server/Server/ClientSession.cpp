#include "stdafx.h"
#include "ClientSession.h"
#include "Server.h"



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
	if (IsBad())
		return;
	ZeroMemory(&recv_over.over, sizeof(WSAOVERLAPPED));
	recv_over.wsabuf.buf = reinterpret_cast<CHAR*>(recv_over.ring_buf.end());
	recv_over.wsabuf.len = recv_over.ring_buf.bytes_to_recv();
	DWORD recv_flag = 0;
	int res = ::WSARecv(socket, &recv_over.wsabuf, 1, 0, &recv_flag, &recv_over.over, nullptr);
	//SocketUtil::CheckError(res, "do_recv");
}

void ClientSession::do_send(const void* const packet)
{
	if (IsBad())
		return;
	ExpOverlapped* over = new ExpOverlapped{ COMP_OP::OP_SEND, packet };
	int res = ::WSASend(socket, &over->wsabuf, 1, nullptr, 0, &over->over, nullptr);
	if (0 != res)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			delete	over;
		}
	}
//	SocketUtil::CheckError(res, ("do_send"s + to_string(this->id)).c_str());
}


void ClientSession::do_disconnect()
{
	if (IsBad())
		return;
	state = SESSION_STATE::ON_DISCONNECT;
	sc_remove_obj remove;
	remove.id = id;
	for (auto& c : Server::get().get_clients())
	{
		c.do_send(&remove);
	}
	auto exover = new ExpOverlapped{ COMP_OP::OP_DISCONNECT };
	PostQueuedCompletionStatus(Server::get().get_iocp(), 0, id, &exover->over);
}