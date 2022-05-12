#include "stdafx.h"
#include "Server.h"
#include "World.h"

/////////////////////////////////////////////////////////////////////////////////////////

void Server::ProcessPacket(ID id, const void* const packet)
{
	// conditional funtion => 함수객체 전달해서 처리.
	auto SND2ME = [this, id](const void* const packet)
	{
		clients[id].do_send(packet);
	};

	auto SND2OTHERS = [this, id](const void* const packet)
	{
		for (int i = 0; i < clients.size(); i++)
		{
			if (i != id) { clients[i].do_send(packet); }
		}
	};

	auto SND2EVERY = [this](const void* const packet)
	{
		for (auto& c : clients)
		{
			c.do_send(packet);
		}
	};

	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	// cerr << "[PACKET::" << +packet_type._to_string() << "]" << endl;
	switch (packet_type)
	{
		CASE PACKET_TYPE::CS_HI :
		{
			{
				sc_hi hi;
				hi.id = NetID(id);
				SND2ME(&hi);
			}

			//if (PlayerManager::get().Move(id, Position{}))
			if (PlayerManager::get().Move(id, Position{ rand() % MAP_SIZE, rand() % MAP_SIZE }))
			{
				sc_set_position set_pos;
				auto pos = PlayerManager::get().GetPosition(id);
				set_pos.id = NetID(id);
				set_pos.pos = pos;
				SND2ME(&set_pos);
			}
			else
			{
				cerr << "[!!!]OverflowedInitPosition" << PlayerManager::get().GetPosition(id).x << " " << PlayerManager::get().GetPosition(id).y << endl;
			}
			

			/*
			for (int i = 0; i < clients.size(); i++)
			{
				if (clients[i].IsBad())
					continue;
				auto pos = PlayerManager::get().GetPosition(i);
				sc_set_position other_pos;
				other_pos.id = NetID(i);
				other_pos.pos = pos;
				SND2ME(&other_pos);
			}
			*/
			
			/*
			{
				sc_set_position set_pos;
				set_pos.id = NetID(id); /////
				auto pos = PlayerManager::get().GetPosition(id);
				set_pos.pos = pos;
				SND2EVERY(&set_pos);
			}
			*/

			{
				sc_ready ready;
				SND2ME(&ready);
			}

			PlayerManager::get().Enable(id);
		}
		CASE PACKET_TYPE::CS_INPUT :
		{
			auto pck = reinterpret_cast<const cs_input*>(packet);
			if (PlayerManager::get().Move(id, pck->input))
			{
				sc_set_position set_pos;
				auto pos = PlayerManager::get().GetPosition(id);
				set_pos.id = NetID(id);
				set_pos.pos = pos;
				set_pos.timestamp = pck->timestamp;
				SND2ME(&set_pos);
			}
		}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
