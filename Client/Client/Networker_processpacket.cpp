#include "stdafx.h"
#include "Networker.h"
#include "Game.h"
#include "System.h"

void Networker::process_packet(const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	auto& game = Game::get();
	//	cerr << "[PACKET::" << +packet_type._to_string() << "]" << endl;
	switch (packet_type)
	{
	case PACKET_TYPE::SC_HI:
	{
		auto pck = reinterpret_cast<const sc_hi*>(packet);
		game.set_id(pck->id);
		game.get_players()[game.get_id()];
		game.init();
	}
	CASE PACKET_TYPE::SC_READY :
	{
		ready_ = true;
	}
	CASE PACKET_TYPE::SC_SET_POSITION :
	{
		auto pck = reinterpret_cast<const sc_set_position*>(packet);
		auto pos = pck->pos;
		auto id = pck->id;
		game.get_players()[id].set_pos(pos);
	}
	CASE PACKET_TYPE::SC_REMOVE_OBJ :
	{
		auto pck = reinterpret_cast<const sc_remove_obj*>(packet);
		game.get_players().erase(pck->id);
	}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
}
