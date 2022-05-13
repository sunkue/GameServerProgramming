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
	ExpOverlappedBasic(COMP_OP Op) :Op{ Op } { ZeroMemory(&Over, sizeof(Over)); };
	WSAOVERLAPPED Over{};
	WSABUF Wsabuf{};
	COMP_OP Op;
};

struct ExpOverlapped : ExpOverlappedBasic
{
	ExpOverlapped(COMP_OP Op, const void* const packet);
	explicit ExpOverlapped(COMP_OP Op);
	array<std::byte, MAX_PACKET_SIZE> Buf{};
};

#include "RingBuffer.hpp"

struct RecvExpOverlapped : ExpOverlappedBasic
{
	RecvExpOverlapped();
#ifdef RINGBUFFER
	RecvRingBuffer<MAX_BUFFER_SIZE> Buf{};
#else
	array<std::byte, MAX_BUFFER_SIZE> buf{};
#endif // RINGBUFFER
};