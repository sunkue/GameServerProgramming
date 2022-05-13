#include "stdafx.h"
#include "ListenSocket.h"
#include "Server.h"


void ListenSocket::Init(HANDLE iocp)
{
	ListenSocket_ = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr; ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = ::htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	auto res = ::bind(ListenSocket_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	SocketUtil::CheckError(res, "bind");
	res = ::listen(ListenSocket_, SOMAXCONN);
	SocketUtil::CheckError(res, "listen");
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(ListenSocket_), iocp, 0, 0);
};

ListenSocket::~ListenSocket()
{
	auto res = ::closesocket(ListenSocket_);
	SocketUtil::CheckError(res, "listen closesocket");
}

void ListenSocket::DoAccept()
{
	NewfaceSocket_ = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&AcceptOver_.Over, sizeof(WSAOVERLAPPED));
	constexpr auto len = sizeof(SOCKADDR_IN) + 16;
	*reinterpret_cast<SOCKET*>(AcceptOver_.Buf.data()) = NewfaceSocket_;
	DWORD received{};
	auto res = ::AcceptEx(ListenSocket_, NewfaceSocket_, AcceptBuf_.data(), 0, len, len, &received, &AcceptOver_.Over);
	SocketUtil::CheckErrorEx(res, "listen AcceptEx");
}
