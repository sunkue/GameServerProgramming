#pragma once

enum class COMP_OP : int8
{
	OP_NONE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND,
	OP_DISCONNECT,
	OP_EVENT
};

struct ExpOverlappedBasic
{
	ExpOverlappedBasic(COMP_OP Op) :Op{ Op } { ZeroMemory(&Over, sizeof(Over)); };
	WSAOVERLAPPED Over{};
	COMP_OP Op;
};

struct ExpOverlapped : ExpOverlappedBasic
{
	ExpOverlapped(COMP_OP Op, const void* const packet);
	explicit ExpOverlapped(COMP_OP Op);
	WSABUF Wsabuf{};
	array<std::byte, MAX_PACKET_SIZE> Buf{};
};

#include "RingBuffer.hpp"

struct RecvExpOverlapped : ExpOverlappedBasic
{
	RecvExpOverlapped();
	WSABUF Wsabuf{};
#ifdef RINGBUFFER
	RecvRingBuffer<MAX_BUFFER_SIZE> Buf{};
#else
	array<std::byte, MAX_BUFFER_SIZE> buf{};
#endif // RINGBUFFER
};

struct EventExpOverlapped : ExpOverlappedBasic
{
	EventExpOverlapped(function<void()> e) : ExpOverlappedBasic{ COMP_OP::OP_EVENT }, EventFunc{ e }{}
	function<void()> EventFunc;
};