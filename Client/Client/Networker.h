#pragma once

class Networker
{
	SINGLE_TON(Networker);
public:
	void start();
	void do_send(const void* const packet);
	GET(ready);
private:
	void do_recv();
	void process_packet(const void* const packet);
	static void CALLBACK cb_recv(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag);
private:
	SOCKET socket;
	bool ready_{ false };
	packet_size_t prerecv_size{};
	array<std::byte, MAX_BUFFER_SIZE> recv_buf{};
	WSABUF mybuf{};
	WSAOVERLAPPED recv_over{};
};

