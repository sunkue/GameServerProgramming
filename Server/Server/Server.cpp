#include "stdafx.h"
#include "Server.h"
#include "GameLogic.h"
#include "GameBoard.h"

void Server::process_packet(NetID id, const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	switch (packet_type)
	{
	case PACKET_TYPE::NONE:
	{}
	CASE PACKET_TYPE::CS_HI :
	{
		sc_hi hi;
		hi.n = GameBoard::get().get_n();
		clients[id].do_send(&hi);
	}
	CASE PACKET_TYPE::CS_INPUT :
	{
		auto pck = reinterpret_cast<const cs_input*>(packet);
		if (game.move(id, pck->input))
		{
			sc_set_position set_pos;
			auto pos = game.get_position(id);
			set_pos.pos = decltype(sc_set_position::pos)::encode(pos);
			clients[id].do_send(&set_pos);
		}
	}
	break; default: break;
	}
}
