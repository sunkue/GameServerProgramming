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
	void Init(SOCKET s);

public:
	void DoRecv();
	void DoSend(const void* const packet);
	void DoDisconnect();

public:
	bool IsGood() { return SESSION_STATE::__GOOD__ <= State_; }
	bool IsBad() { return !IsGood(); }
	bool IsFree() { return SESSION_STATE::FREE == State_; }
	bool IsUnFree() { return !IsFree(); }

private:
	SOCKET Socket_{};
	ID Id_{ -1 };
	RecvExpOverlapped RecvOver_{};
	atomic<SESSION_STATE> State_{ SESSION_STATE::FREE };
#ifndef RINGBUFFER
	packet_size_t prerecv_size{};
#endif // RINGBUFFER
};


