#include "stdafx.h"
#include "ClientSession.h"
#include "Server.h"
#include "Player.h"
#include "Character.h"


///////////////////////////////////////////////////////////

ClientSession::~ClientSession()
{

};

void ClientSession::Init(SOCKET s)
{
	Socket_ = s;
}

void ClientSession::DoRecv()
{
	if (IsBad())
		return;
	ZeroMemory(&RecvOver_.Over, sizeof(WSAOVERLAPPED));
#ifdef RINGBUFFER
	RecvOver_.Wsabuf.buf = reinterpret_cast<CHAR*>(RecvOver_.Buf.End());
	RecvOver_.Wsabuf.len = static_cast<ULONG>(RecvOver_.Buf.BytesToRecv());
#else
	recv_over.wsabuf.buf = reinterpret_cast<CHAR*>(recv_over.buf.data()) + prerecv_size;
	recv_over.wsabuf.len = sizeof(recv_over.buf) - prerecv_size;
#endif // RINGBUFFER
	DWORD recv_flag = 0;
	int res = ::WSARecv(Socket_, &RecvOver_.Wsabuf, 1, 0, &recv_flag, &RecvOver_.Over, nullptr);
	//SocketUtil::CheckError(res, "do_recv");
}

void ClientSession::DoSend(const void* const packet)
{
	if (IsBad())
		return;
	ExpOverlapped* Over = new ExpOverlapped{ COMP_OP::OP_SEND, packet };
	int res = ::WSASend(Socket_, &Over->Wsabuf, 1, nullptr, 0, &Over->Over, nullptr);
	if (0 != res)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			delete	Over;
		}
	}
//	SocketUtil::CheckError(res, ("do_send"s + to_string(this->id)).c_str());
}


void ClientSession::DoDisconnect()
{
	if (IsBad())
		return;
	State_ = SESSION_STATE::ON_DISCONNECT;
	CharacterManager::Get().Disable(Id_);

	auto exover = new ExpOverlapped{ COMP_OP::OP_DISCONNECT };
	PostQueuedCompletionStatus(Server::Get().GetIocp(), 0, Id_, &exover->Over);
}