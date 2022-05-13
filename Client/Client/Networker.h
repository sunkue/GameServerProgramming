#pragma once

class Networker
{
	SINGLE_TON(Networker);
public:
	void Start();
	void DoSend(const void* const packet);
	GET(Ready);
private:
	void DoRecv();
	void ProcessPacket(const void* const packet);
	static void CALLBACK CbRecv(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag);
private:
	SOCKET Socket_;
	bool Ready_{ false };
	packet_size_t PrerecvSize_{};
	array<std::byte, MAX_BUFFER_SIZE> RecvBuf_{};
	WSABUF MyBuf_{};
	WSAOVERLAPPED RecvOver_{};
};

