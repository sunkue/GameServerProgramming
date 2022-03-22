#pragma once

class Networker
{
	SINGLE_TON(Networker);

public:
	void start();
	void do_send(const void* const packet);

public:
	GET(ready);

private:
	void do_recv();

private:
	void process_packet(const void* const packet);

private:
	static void CALLBACK cb_recv(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag);

private:
	SOCKET socket;
	bool ready_{ false };
	packet_size_t prerecv_size{};
	array<char, MAX_BUFFER_SIZE> recv_buf{};
};

