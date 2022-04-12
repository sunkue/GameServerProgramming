#pragma once

#include "ExpOverlapped.h"

class ListenSocket
{
	SINGLE_TON(ListenSocket) = default;
	~ListenSocket();
private:
	SOCKET listen_socket{};
	SOCKET newface_socket{};
	ExpOverlapped accept_over{ COMP_OP::OP_ACCEPT };
	array<char, 64> accept_buf{};
public:
	void init(HANDLE iocp);
	void do_accept();
};

