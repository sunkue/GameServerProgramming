#pragma once

#include"ExpOverlapped.h"


class ClientSession
{
	friend class Server;
public:
	ClientSession(SOCKET s) :socket{ s } { };
	ClientSession() = default;
	~ClientSession();

public:
	void do_recv();
	void do_send(const void* const packet);
	void do_disconnect();
private:
	union {
		SOCKET socket;
		ID id;
	};
	packet_size_t prerecv_size{};
	EXPOVERLAPPED recv_over{ COMP_OP::OP_RECV };
};


