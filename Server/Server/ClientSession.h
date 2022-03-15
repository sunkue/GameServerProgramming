#pragma once

class ClientSession
{
public:
	ClientSession(SOCKET s, NetID id) :socket{ s }, id{ id } {};
	ClientSession(){};

	void do_recv();
	void do_send(const void* const packet);

private:
	NetID id;
	SOCKET socket;
	array<char, MAX_BUFFER_SIZE> recv_buf{};
};

