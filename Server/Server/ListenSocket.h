#pragma once

#include "ExpOverlapped.h"

class ListenSocket
{
	SINGLE_TON(ListenSocket) = default;
	~ListenSocket();
private:
	SOCKET ListenSocket_{};
	SOCKET NewfaceSocket_{};
	ExpOverlapped AcceptOver_{ COMP_OP::OP_ACCEPT };
	array<char, 64> AcceptBuf_{};
public:
	void Init(HANDLE iocp);
	void DoAccept();
};

