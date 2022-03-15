#pragma once
class Networker
{
	SINGLE_TON(Networker)
	{
		WSADATA WSAData;
		int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
		SocketUtil::CheckError(ret);
		socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		SOCKADDR_IN server_addr; ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		string sever_ip; cout << "서버 IP를 입력하세요 :: "; cin >> sever_ip;
		inet_pton(AF_INET, sever_ip.c_str(), &server_addr.sin_addr);
		ret = WSAConnect(socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), nullptr, nullptr, nullptr, nullptr);
		SocketUtil::CheckError(ret);
	}

public:
	void start_recv();
	void do_recv();
	void do_send(const void* const packet);
	void stop() { stop_work = true; }
private:
	void process_packet(const void* const packet);

private:
	SOCKET socket;
	bool stop_work = false;
	array<char, MAX_BUFFER_SIZE> recv_buf{};
};

