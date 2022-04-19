#include "stdafx.h"
#include "Server.h"

Server::Server()
{
	for (int i = 0; i < clients.size(); i++)
	{
		clients[i].id = i;
	}

	WSADATA WSAData; int ret = ::WSAStartup(MAKEWORD(2, 2), &WSAData); SocketUtil::CheckError(ret);
	iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
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
		DWORD returned_bytes{}; ID id{}; ExpOverlapped* exover{};
		//	cerr << "GQCSTART::";
		auto res = GetQueuedCompletionStatus(iocp, &returned_bytes, reinterpret_cast<PULONG_PTR>(&id), reinterpret_cast<WSAOVERLAPPED**>(&exover), INFINITE);
		//	cout << this_thread::get_id() << endl;
	//	cerr << "GQCS::" << (SOCKET)id << "::" << returned_bytes << "::" << endl;

		if (FALSE == res) [[unlikely]]
		{
			cerr << "GQCS::ERR::" << WSAGetLastError() << "::" << endl;
			if (0 == returned_bytes)
			{
				clients[id].do_disconnect();
			}
			continue;
		};

		switch (exover->op)
		{
		case COMP_OP::OP_RECV: OnRecvComplete(id, returned_bytes); break;
		case COMP_OP::OP_SEND: OnSendComplete(id, exover); break;
		case COMP_OP::OP_ACCEPT: OnAcceptComplete(exover); break;
		case COMP_OP::OP_DISCONNECT: OnDisconnectComplete(id, exover); break;
		default: cerr << "[[[??]]]" << endl; break;
		}
	}
}

void Server::StartAccept()
{
	ListenSocket::get().init(iocp);
	ListenSocket::get().do_accept();
}

/////////////////////////////////////////////////////////////////////////////////////////

void Server::OnRecvComplete(ID id, DWORD transfered)
{
	auto& client = clients[id];
	/*
	if (0 == transfered)
	{
		cout << " Client Disconnected" << endl;
		clients.erase(client.socket);
		return;
	}
	*/
	cerr << "[recv]" << endl;
	auto& recvbuf = client.recv_over.ring_buf;
	recvbuf.move_rear(transfered);

	auto pck_start = recvbuf.begin();
	auto remain_bytes = recvbuf.size();

	for (auto need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
		need_bytes <= remain_bytes && 0 < remain_bytes;)
	{
		// 링버퍼경계에 걸친 패킷
		if (recvbuf.check_overflow_when_read(need_bytes))
		{
			cerr << "[RingBuffer]::CollideOnEdge" << endl;
			// 미완성 패킷임
			if (recvbuf.size() < need_bytes)
			{
				continue;
			}

			std::byte temp_packet[MAX_PACKET_SIZE];
			auto packetsize1 = recvbuf.filled_edgespace();
			memcpy(temp_packet, pck_start, packetsize1);
			auto packetsize2 = need_bytes - packetsize1;
			memcpy(temp_packet + packetsize1, &recvbuf, packetsize2);
			pck_start = temp_packet;
		}

		process_packet(id, pck_start);
		recvbuf.move_front(need_bytes);

		pck_start = recvbuf.begin();
		remain_bytes = recvbuf.size();
		need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
	}

	/*
		일반배열	{recv-> memmove}
		링버퍼	{recv-> move_rear, bytes_to_recv}
				{processpacket -> move_front}
				{move_front collide edge ? -> memcpy * 2}

		성능비교를 해봐야 뭐가 더 좋을지 확신할 수 있겠음,,
		패킷 대비 버퍼가 클 수록 링버퍼가 유리.
	*/

	/*
	auto pck_start = client.recv_over.buf.data();
	auto remain_bytes = transfered + client.prerecv_size;

	for (auto need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
		need_bytes <= remain_bytes && 0 < remain_bytes;)
	{
		process_packet(id, pck_start);

		pck_start += need_bytes;
		remain_bytes -= need_bytes;
		need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
		// 허위 정보가 들어갈 수 있으나, 그땐 remain_bytes 가 0 이기에 괜찮다.
	}

	client.prerecv_size = static_cast<packet_size_t>(remain_bytes);

	if (0 != remain_bytes)
	{
		memmove(client.recv_over.buf.data(), pck_start, remain_bytes);
	}
	*/

	client.do_recv();
}

void Server::OnSendComplete(ID id, ExpOverlapped* exover)
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

ID Server::get_free_id()
{
	for (ID i = 0; i < clients.size(); i++)
	{
		auto expect = SESSION_STATE::FREE;
		if (clients[i].state.compare_exchange_strong(expect, SESSION_STATE::ACCEPTED))
		{
			return i;
		}
	}

	cerr << "NO FREE ID" << endl;
	return -1;
}

void Server::OnAcceptComplete(ExpOverlapped* exover)
{
	SOCKET new_socket = *reinterpret_cast<SOCKET*>(exover->buf.data());
	ID id = get_free_id();
	__analysis_assume(id == 0);

	if (-1 == id) [[unlikely]]
	{
		cerr << "Clients Full [" << clients.size() << "]" << endl;
		int res = ::closesocket(new_socket);
		SocketUtil::CheckError(res);
	}

	cerr << "accept ::" << id << endl;

	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_socket), iocp, id, 0);
	clients[id].init(new_socket);
	//	cerr << "[ " << clients.size() << " ] on_line" << endl;
	clients[id].do_recv();

	ListenSocket::get().do_accept();
}

void Server::OnDisconnectComplete(ID id, ExpOverlapped* exover)
{
	sc_remove_obj remove;
	remove.id = id;
	for (auto& c : clients)
	{
		c.do_send(&remove);
	}

	cerr << "diconnect::" << id << "::" << endl;
	int res = ::closesocket(clients[id].socket);
	SocketUtil::CheckError(res);
}



