#include "stdafx.h"
#include "Networker.h"
#include "System.h"

Networker::Networker()
{
	WSADATA WSAData;
	int res = WSAStartup(MAKEWORD(2, 2), &WSAData);
	SocketUtil::CheckError(res, "WSAStartup");
	Socket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr; ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
RETRY:
	string sever_ip; cout << "서버 IP를 입력하세요 :: "; cin >> sever_ip;
	inet_pton(AF_INET, sever_ip.c_str(), &server_addr.sin_addr);
	res = WSAConnect(Socket_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), nullptr, nullptr, nullptr, nullptr);
	system("cls");
	SocketUtil::CheckError(res, "WSAConnect");
	if (SocketUtil::SOCKET_SUCCESS != res)
	{
		cout << "[ERR] FAIL TO CONNECT SERVER. try again " << endl;
		goto RETRY;
	}
}

void Networker::DoRecv()
{
	MyBuf_.buf = reinterpret_cast<CHAR*>(RecvBuf_.data() + PrerecvSize_);
	MyBuf_.len = static_cast<ULONG>(RecvBuf_.size());
	ZeroMemory(&RecvOver_, sizeof(WSAOVERLAPPED));
	DWORD recv_flag = 0;
	int ret = WSARecv(Socket_, &MyBuf_, 1, nullptr, &recv_flag, &RecvOver_, Networker::CbRecv);
	SocketUtil::CheckError(ret, "recv");
}

void Networker::DoSend(const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	// cerr << "Send [PACKET::" << +packet_type._to_string() << "]" << endl;
	static WSABUF mybuf{}; mybuf.buf = reinterpret_cast<char*>(const_cast<void*>(packet));
	mybuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	DWORD send_bytes;
	int ret = WSASend(Socket_, &mybuf, 1, &send_bytes, 0, 0, 0);
	SocketUtil::CheckError(ret, "send");
}

void Networker::CbRecv(DWORD error, DWORD transfered, LPWSAOVERLAPPED over, DWORD flag)
{
	if (0 == transfered)
	{
		cout << " Server Disconnected" << endl;
		glfwSetWindowShouldClose(System::Get().Window, true);
		return;
	}

	auto& nw = Networker::Get();

	auto pck_start = nw.RecvBuf_.data();
	auto remain_bytes = transfered + nw.PrerecvSize_;

	for (auto need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
		need_bytes <= remain_bytes && 0 != remain_bytes;)
	{
		nw.ProcessPacket(pck_start);

		pck_start += need_bytes;
		remain_bytes -= need_bytes;
		need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
	}

	nw.PrerecvSize_ = static_cast<packet_size_t>(remain_bytes);
	memmove(nw.RecvBuf_.data(), pck_start, remain_bytes);

	nw.DoRecv();
}

void Networker::LoginSignupOnConsole()
{
RETRY:
	{
		string str; str.resize(MAX_LOGIN_ID_BUFFER_SIZE);
		cout << " LOGIN or SIGNUP? (L/S)" << endl; cin >> str;
		if (str.find_first_of('L') < str.find_first_of('S'))
		{
			system("cls");
			cout << "=========== LOGIN ===========" << endl;
			cs_login login;
			cout << "[ ID ]  :: "; str.clear(); cin >> str;
			strcpy_s(login.login_id, str.c_str());
			cout << "[P / W] :: "; str.clear(); cin >> str;
			strcpy_s(login.login_password, str.c_str());

			cout << login.login_id << " " << login.login_password << endl;
			DoSend(&login);
			return;
		}
		else if (str.find_first_of('L') == str.find_first_of('S'))
		{
			system("cls");
			goto RETRY;
		}
		else
		{
			system("cls");
			cout << "=========== SIGNUP ===========" << endl;
			cs_signup signup;
			cout << "[ ID ]  :: "; str.clear(); cin >> str;
			strcpy_s(signup.signup_id, str.c_str());
			cout << "[P / W] :: "; str.clear(); cin >> str;
			strcpy_s(signup.signup_password, str.c_str());
			cout << signup.signup_id << " " << signup.signup_password << endl;
			cout << "Is it alright? (Y/N)" << endl; str.clear(); cin >> str;
			if (str.find_first_of('N') != string::npos) {
				system("cls");
				cout << "(CANCLE)" << endl;
				goto RETRY;
			}
			DoSend(&signup);
			return;
		}
	}
}

void Networker::Start()
{
	DoRecv();
	LoginSignupOnConsole();
}

