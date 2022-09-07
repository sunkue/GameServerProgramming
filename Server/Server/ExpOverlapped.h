#pragma once

#include "RingBuffer.hpp"

enum class COMP_OP : int8
{
	OP_NONE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND,
	OP_DISCONNECT,
	OP_EVENT,
	OP_DB_EVENT
};

struct ExpOverlappedBase
{
	ExpOverlappedBase(COMP_OP Op) :Op{ Op } { ZeroMemory(&Over, sizeof(Over)); };
	WSAOVERLAPPED Over{};
	COMP_OP Op;
};

struct ExpOverlapped : ExpOverlappedBase
{
	ExpOverlapped(COMP_OP Op, const void* const packet);
	explicit ExpOverlapped(COMP_OP Op);
	WSABUF Wsabuf{};
	array<std::byte, MAX_PACKET_SIZE> Buf{};
};

struct RecvExpOverlapped : ExpOverlappedBase
{
	RecvExpOverlapped();
	WSABUF Wsabuf{};
#ifdef RINGBUFFER
	RecvRingBuffer<MAX_BUFFER_SIZE> Buf{};
#else
	array<std::byte, MAX_BUFFER_SIZE> buf{};
#endif // RINGBUFFER
};

struct EventExpOverlapped : ExpOverlappedBase
{
	EventExpOverlapped(function<void()> e) : ExpOverlappedBase{ COMP_OP::OP_EVENT }, EventFunc{ e }{}
	function<void()> EventFunc;
};

struct DataBaseExpOverlapped : ExpOverlappedBase
{
	DataBaseExpOverlapped(function<void(vector<any>)> f, vector<any> R)
		: ExpOverlappedBase{ COMP_OP::OP_DB_EVENT }, Func{ f } { Results = R; }
	vector<any> Results;
	function<void(vector<any>)> Func;
};


