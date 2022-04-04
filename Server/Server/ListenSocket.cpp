#include "stdafx.h"
#include "ListenSocket.h"
#include "Server.h"


void ListenSocket::init(HANDLE iocp)
{
	listen_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr; ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = ::htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	auto res = ::bind(listen_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	SocketUtil::CheckError(res);
	res = ::listen(listen_socket, SOMAXCONN); SocketUtil::CheckError(res);
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listen_socket), iocp, 0, 0);
};

ListenSocket::~ListenSocket()
{
	auto res = ::closesocket(listen_socket);
	SocketUtil::CheckError(res);
}

void ListenSocket::do_accept()
{
	newface_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&accept_over.over, sizeof(WSAOVERLAPPED));
	constexpr auto len = sizeof(SOCKADDR_IN) + 16;
	*reinterpret_cast<SOCKET*>(accept_over.buf.data()) = newface_socket;
	DWORD received{};
	auto res = ::AcceptEx(listen_socket, newface_socket, accept_buf.data(), 0, len, len, &received, &accept_over.over);
	SocketUtil::CheckErrorEx(res);
	cerr << "accept started, waiting for... ::" << newface_socket << endl;
}
