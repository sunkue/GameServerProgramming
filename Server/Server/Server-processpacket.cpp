#include "stdafx.h"
#include "Server.h"
#include "GameLogic.h"
#include "GameBoard.h"

/////////////////////////////////////////////////////////////////////////////////////////

void Server::process_packet(ID id, const void* const packet)
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
	switch (packet_type)
	{
		CASE PACKET_TYPE::CS_HI :
		{
			{
				sc_hi hi;
				hi.n = GameBoard::get().get_n();
				hi.id = NetID(id);
				SND2ME(&hi);
			}

			for (int i = 0; i < clients.size(); i++)
			{
				if (clients[i].IsFree())
					continue;
				auto pos = GameLogic::get().get_position(i);
				sc_set_position other_pos;
				other_pos.id = NetID(i); /////
				other_pos.pos = decltype(other_pos.pos)::encode(pos);
				SND2ME(&other_pos);
			}

			{
				sc_set_position set_pos;
				set_pos.id = NetID(id); /////
				auto pos = GameLogic::get().get_position(id);
				set_pos.pos = decltype(set_pos.pos)::encode(pos);
				SND2EVERY(&set_pos);
			}

			{
				sc_ready ready;
				SND2ME(&ready);
			}

		}
		CASE PACKET_TYPE::CS_INPUT :
		{
			auto pck = reinterpret_cast<const cs_input*>(packet);
			if (GameLogic::get().move(id, pck->input))
			{
				sc_set_position set_pos;
				auto pos = GameLogic::get().get_position(id);
				set_pos.id = NetID(id); /////
				set_pos.pos = decltype(set_pos.pos)::encode(pos);
				SND2EVERY(&set_pos);
			}
		}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
	cerr << "[PACKET::" << +packet_type._to_string() << "]" << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////
