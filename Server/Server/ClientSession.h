#pragma once

struct EXPOVERLAPPED
{
	EXPOVERLAPPED(const void* const packet);
	WSAOVERLAPPED send_over{};
	WSABUF send_wsabuf{};
	array<char, MAX_BUFFER_SIZE> send_buf{};
};

class ClientSession
{
	friend class Server;
public:
	ClientSession(SOCKET s) :socket{ s } { recv_wsabuf.buf = recv_buf.data(); recv_wsabuf.len = static_cast<ULONG>(recv_buf.size()); };
	ClientSession() = default;

public:
	void do_recv();
	void do_send(const void* const packet);

public:
	GET(id);
private:
	union {
		SOCKET socket;
		ID id_;
	};
	WSABUF recv_wsabuf{};
	WSAOVERLAPPED recv_over{};
	array<char, MAX_BUFFER_SIZE> recv_buf{};
};


