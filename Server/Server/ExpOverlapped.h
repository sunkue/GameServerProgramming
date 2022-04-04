#pragma once

enum class COMP_OP : int8
{
	OP_NONE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND,
	OP_DISCONNECT
};

struct EXPOVERLAPPED
{
	EXPOVERLAPPED(COMP_OP op, const void* const packet);
	explicit EXPOVERLAPPED(COMP_OP op);
	WSAOVERLAPPED over{};
	WSABUF wsabuf{};
	array<char, MAX_BUFFER_SIZE> buf{};
	COMP_OP op;
};