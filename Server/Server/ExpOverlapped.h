#pragma once

enum class COMP_OP : int8
{
	OP_NONE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND,
	OP_DISCONNECT
};

struct ExpOverlappedBasic
{
	ExpOverlappedBasic(COMP_OP op) :op{ op } { ZeroMemory(&over, sizeof(over)); };
	WSAOVERLAPPED over{};
	WSABUF wsabuf{};
	COMP_OP op;
};

struct ExpOverlapped : ExpOverlappedBasic
{
	ExpOverlapped(COMP_OP op, const void* const packet);
	explicit ExpOverlapped(COMP_OP op);
	array<std::byte, MAX_PACKET_SIZE> buf{};
};

#include "RingBuffer.hpp"

struct RecvExpOverlapped : ExpOverlappedBasic
{
	RecvExpOverlapped();
	RecvRingBuffer<MAX_BUFFER_SIZE> ring_buf{};
};