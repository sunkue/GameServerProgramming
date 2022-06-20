#include "stdafx.h"
#include "Server.h"
#include "TimerEvent.h"

Server::Server()
{
	for (int i = 0; i < Clients_.size(); i++)
	{
		Clients_[i].Id_ = i;
	}

	WSADATA WSAData; int res = ::WSAStartup(MAKEWORD(2, 2), &WSAData); SocketUtil::CheckError(res, "WSAStartup");
	Iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
}

Server::~Server()
{
	::WSACleanup();
}

/////////////////////////////////////////////////////////////

void Server::ProcessQueuedCompleteOperationLoop()
{
	while (true)
	{
		DWORD returned_bytes{}; ID Id_{}; ExpOverlapped* exover{};
		//	cerr << "GQCSTART::";
		auto res = GetQueuedCompletionStatus(Iocp_, &returned_bytes, reinterpret_cast<PULONG_PTR>(&Id_), reinterpret_cast<WSAOVERLAPPED**>(&exover), INFINITE);
		//	cout << this_thread::get_id() << endl;
		// cerr << "GQCS::" << (SOCKET)Id_ << "::" << returned_bytes << "::" << endl;

		if (FALSE == res) [[unlikely]]
		{
			// cerr << "GQCS::ERR::" << WSAGetLastError() << "::" << endl;
			Clients_[Id_].DoDisconnect();
			continue;
		};

		switch (exover->Op)
		{
		case COMP_OP::OP_RECV: OnRecvComplete(Id_, returned_bytes); break;
		case COMP_OP::OP_SEND: OnSendComplete(Id_, exover); break;
		case COMP_OP::OP_ACCEPT: OnAcceptComplete(exover); break;
		case COMP_OP::OP_DISCONNECT: OnDisconnectComplete(Id_, exover); break;
		case COMP_OP::OP_EVENT: OnEventTimerComplete(exover); break;
		case COMP_OP::OP_DB_EVENT: OnDataBaseQueryComplete(exover); break;
		default: cerr << "[[[??]]]" << endl; break;
		}
	}
}

void Server::ProcessQueuedCompleteOperationLoopEx()
{
	constexpr size_t MAX_GQCS_IO_ENTRY{ 10 };

	ULONG returned_ios{};
	array<OVERLAPPED_ENTRY, MAX_GQCS_IO_ENTRY> exovers{};
	
	while (true)
	{
		exovers.fill({});
		//	cerr << "GQCSTART::";
		auto res = GetQueuedCompletionStatusEx(Iocp_, exovers.data(), static_cast<ULONG>(exovers.size()), &returned_ios, INFINITE, FALSE);
		//	cout << this_thread::get_id() << endl;
//	cerr << "GQCS::" << (SOCKET)id << "::" << returned_bytes << "::" << endl;

		if (1 < returned_ios)
		{
			if (MAX_GQCS_IO_ENTRY == returned_ios)
			{
				// cerr << "[!!io num]" << endl;
			}
			// cerr << "[!!io num] [ " << returned_ios << " ]" << endl;
		}

		for (ULONG i = 0; i < returned_ios; i++)
		{
			DWORD returned_bytes = exovers[i].dwNumberOfBytesTransferred;
			ID Id_ = static_cast<ID>(exovers[i].lpCompletionKey);
			ExpOverlapped* exover = reinterpret_cast<ExpOverlapped*>(exovers[i].lpOverlapped);

			if (FALSE == res) [[unlikely]]
			{
				// cerr << "GQCS::ERR::" << WSAGetLastError() << "::" << endl;
				Clients_[Id_].DoDisconnect();
				continue;
			};

			switch (exover->Op)
			{
			case COMP_OP::OP_RECV: OnRecvComplete(Id_, returned_bytes); break;
			case COMP_OP::OP_SEND: OnSendComplete(Id_, exover); break;
			case COMP_OP::OP_ACCEPT: OnAcceptComplete(exover); break;
			case COMP_OP::OP_DISCONNECT: OnDisconnectComplete(Id_, exover); break;
			case COMP_OP::OP_EVENT: OnEventTimerComplete(exover); break;
			default: cerr << "[[[??]]]" << endl; break;
			}
		}
	}
}

void Server::StartAccept()
{
	ListenSocket::Get().Init(Iocp_);
	ListenSocket::Get().DoAccept();
}

void Server::RepeatSendLoop(milliseconds repeat_time)
{
	while (true)
	{
		auto curr = std::chrono::high_resolution_clock::now();
		static auto TimeAfterSend = 0ms;
		static auto past = curr;

		TimeAfterSend += duration_cast<milliseconds>(curr - past);

		if (TimeAfterSend < repeat_time)
		{
			continue;
		}

		for (auto& s : Clients_)
		{
			//sc_test_heart_bit x;
			//x.time_after_send = TimeAfterSend;
			//s.do_send(&x, sizeof(sc_test_heart_bit));
		}

		past = curr;
		TimeAfterSend = 0ms;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////

void Server::OnRecvComplete(ID Id_, DWORD transfered)
{
	// cerr << "[recv]" << endl;

	auto& client = Clients_[Id_];
	auto& recvbuf = client.RecvOver_.Buf;


#ifdef RINGBUFFER

	if (0 == transfered)
	{
		// cerr << " zero recv " << endl;
		if (recvbuf.BytesToRecv())
		{
			client.DoDisconnect();
		}
		else
		{
			// cerr << " recvbuf full " << endl;
		}
		return;
	}

	recvbuf.MoveRear(transfered);

	auto pck_start = recvbuf.Begin();
	auto remain_bytes = recvbuf.Size();

	for (auto need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
		need_bytes <= remain_bytes && 0 != remain_bytes;)
	{
		// 링버퍼경계에 걸친 패킷
		if (recvbuf.CheckOverflowOnRead(need_bytes))
		{
			// 미완성 패킷
			if (recvbuf.Size() < need_bytes)
				continue;

			thread_local static std::byte temp_packet[MAX_PACKET_SIZE];
			auto packetsize1 = recvbuf.FilledEdgespace();
			memcpy(temp_packet, pck_start, packetsize1);
			auto packetsize2 = need_bytes - packetsize1;
			memcpy(temp_packet + packetsize1, &recvbuf, packetsize2);
			pck_start = temp_packet;
		}

		ProcessPacket(Id_, pck_start);
		recvbuf.MoveFront(need_bytes);

		pck_start = recvbuf.Begin();
		remain_bytes = recvbuf.Size();
		need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
	}
#else
	if (0 == transfered)
	{
		client.do_disconnect();
		return;
	}

	auto pck_start = client.recv_over.buf.data();
	auto remain_bytes = transfered + client.prerecv_size;

	for (auto need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
		need_bytes <= remain_bytes && 0 != remain_bytes;)
	{
		ProcessPacket(id, pck_start);

		pck_start += need_bytes;
		remain_bytes -= need_bytes;
		need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
	}

	client.prerecv_size = static_cast<packet_size_t>(remain_bytes);
	memmove(client.recv_over.buf.data(), pck_start, remain_bytes);
#endif // RINGBUFFER


	client.DoRecv();
}

void Server::OnSendComplete(ID Id_, ExpOverlapped* exover)
{
	//cerr << "send" << endl;
	/*
	auto& client = clients[id];

	if (transfered != exover->wsabuf.len)
	{
		client.do_disconnect();
	}
	*/
	delete exover;
}

ID Server::GetFreeId()
{
	for (ID i = 0; i < Clients_.size(); i++)
	{
		auto expect = SESSION_STATE::FREE;
		if (Clients_[i].State_.compare_exchange_strong(expect, SESSION_STATE::ACCEPTED))
		{
			return i;
		}
	}

	cerr << "NO FREE ID" << endl;
	return -1;
}

void Server::OnAcceptComplete(ExpOverlapped* exover)
{
	SOCKET new_socket = *reinterpret_cast<SOCKET*>(exover->Buf.data());
	ID Id_ = GetFreeId();
	__analysis_assume(Id_ == 0);

	if (-1 == Id_) [[unlikely]]
	{
		cerr << "Clients Full [" << Clients_.size() << "]" << endl;
		int res = ::closesocket(new_socket);
		SocketUtil::CheckError(res, "full->close_socket");
	}

		// cerr << "accept ::" << id << endl;

	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_socket), Iocp_, Id_, 0);
	Clients_[Id_].Init(new_socket);
	//	cerr << "[ " << clients.size() << " ] on_line" << endl;
	Clients_[Id_].DoRecv();

	ListenSocket::Get().DoAccept();
}

void Server::OnDisconnectComplete(ID Id_, ExpOverlapped* exover)
{
	// cerr << "diconnect::" << id << "::" << endl;
	int res = ::closesocket(Clients_[Id_].Socket_);
	SocketUtil::CheckError(res, "close socket");
	Clients_[Id_].State_ = SESSION_STATE::FREE;
	delete exover;
}


void Server::OnEventTimerComplete(ExpOverlapped* exover)
{
	auto e = reinterpret_cast<EventExpOverlapped*>(exover);
	e->EventFunc();
	delete exover;
}

void Server::OnDataBaseQueryComplete(ExpOverlapped* exover)
{
	auto e = reinterpret_cast<DataBaseExpOverlapped*>(exover);
	e->Func(e->Results);
	delete exover;
}



