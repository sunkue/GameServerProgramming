#pragma once

#include "ExpOverlapped.h"
#include "RingBuffer.hpp"

enum class SESSION_STATE : int8
{
	FREE,
	ON_DISCONNECT,
	__BAD__,
	__GOOD__,
	ACCEPTED,
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
	RecvExpOverlapped recv_over{};
	atomic<SESSION_STATE> state{ SESSION_STATE::FREE };
};


