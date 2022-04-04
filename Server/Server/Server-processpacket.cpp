#include "stdafx.h"
#include "Server.h"
#include "GameLogic.h"
#include "GameBoard.h"

/////////////////////////////////////////////////////////////////////////////////////////

void Server::process_packet(ID id, const void* const packet)
{
	auto SND2ME = [this, id](const void* const packet)
	{
		clients[id].do_send(packet);
	};

	auto SND2OTHERS = [this, id](const void* const packet)
	{
		for (auto& c : clients)
		{
			if (c.second.id != id) { c.second.do_send(packet); }
		}
	};

	auto SND2EVERY = [this](const void* const packet)
	{
		for (auto& c : clients)
		{
			c.second.do_send(packet);
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

			for (auto& c : clients)
			{
				auto other_id = c.first;
				auto pos = GameLogic::get().get_position(other_id);
				sc_set_position other_pos;
				other_pos.id = NetID(other_id); /////
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
}

/////////////////////////////////////////////////////////////////////////////////////////
