#pragma once

#include"ExpOverlapped.h"
#include "RingBuffer.hpp"

enum class SESSION_STATE : int8
{
	FREE,
	ACCEPTED,
	__BAD__,
	__GOOD__,
	INGAME,
};

class ClientSession
{
	friend class Server;
public:
	ClientSession() = default;
	~ClientSession();

public:
	void init(SOCKET s);

public:
	void do_recv();
	void do_send(const void* const packet);
	void do_disconnect();

public:
	bool IsGood() { return SESSION_STATE::__GOOD__ <= state; }
	bool IsBad() { return !IsGood(); }
	bool IsFree() { return SESSION_STATE::FREE == state; }
	bool IsUnFree() { return !IsFree(); }

private:
	SOCKET socket{};
	ID id{ -1 };
	packet_size_t prerecv_size{};
	ExpOverlapped recv_over{ COMP_OP::OP_RECV };
	atomic<SESSION_STATE> state{ SESSION_STATE::FREE };
};


