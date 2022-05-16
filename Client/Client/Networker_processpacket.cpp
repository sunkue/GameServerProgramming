#include "stdafx.h"
#include "Networker.h"
#include "Game.h"
#include "System.h"

void Networker::ProcessPacket(const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	auto& game = Game::Get();
	//	cerr << "[PACKET::" << +packet_type._to_string() << "]" << endl;
	switch (packet_type)
	{
	case PACKET_TYPE::Sc_hi:
	{
		auto pck = reinterpret_cast<const sc_hi*>(packet);
		game.SetId(pck->id);
		game.GetPlayers()[game.GetId()];
		game.init();
	}
	CASE PACKET_TYPE::Sc_ready :
	{
		Ready_ = true;
	}
	CASE PACKET_TYPE::Sc_set_position :
	{
		auto pck = reinterpret_cast<const sc_set_position*>(packet);
		auto pos = pck->pos;
		auto id = pck->id;
		game.GetPlayers()[id].SetPos(pos);
	}
	CASE PACKET_TYPE::Sc_set_hp :
	{
		auto pck = reinterpret_cast<const sc_set_hp*>(packet);
		auto hp = pck->hp;
		auto id = pck->id;
		game.GetPlayers()[id].SetHp(hp);
	}
	CASE PACKET_TYPE::Sc_remove_obj :
	{
		auto pck = reinterpret_cast<const sc_remove_obj*>(packet);
		game.GetPlayers().erase(pck->id);
	}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
}
