#include "stdafx.h"
#include "Networker.h"
#include "Game.h"
#include "System.h"
#include "Chat.h"

void Networker::ProcessPacket(const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	auto& game = Game::Get();
	// cerr << "[PACKET::" << +packet_type._to_string() << "]" << endl;
	switch (packet_type)
	{
	case PACKET_TYPE::Sc_hi:
	{
		auto pck = reinterpret_cast<const sc_hi*>(packet);
		game.SetId(pck->id);
		game.GetCharacters()[game.GetId()];
		game.init();
	}
	CASE PACKET_TYPE::Sc_ready :
	{
		Ready_ = true;
		cs_input pck;
		pck.input = eMoveOper::up;
		Networker::Get().DoSend(&pck);
		pck.input = eMoveOper::down;
		Networker::Get().DoSend(&pck);
		pck.input = eMoveOper::right;
		Networker::Get().DoSend(&pck);
		pck.input = eMoveOper::left;
		Networker::Get().DoSend(&pck);
	}
	CASE PACKET_TYPE::Sc_set_position :
	{
		auto pck = reinterpret_cast<const sc_set_position*>(packet);
		auto pos = pck->pos;
		auto id = pck->id;
		game.GetCharacters()[id].SetPos(pos);
	}
	CASE PACKET_TYPE::Sc_set_hp :
	{
		auto pck = reinterpret_cast<const sc_set_hp*>(packet);
		auto hp = pck->hp;
		auto id = pck->id;
		game.GetCharacters()[id].SetHp(hp);
	}
	CASE PACKET_TYPE::Sc_set_exp :
	{
		auto pck = reinterpret_cast<const sc_set_exp*>(packet);
		auto exp = pck->exp;
		auto id = pck->id;
		game.GetCharacters()[id].SetExp(exp);
	}
	CASE PACKET_TYPE::Sc_set_level :
	{
		auto pck = reinterpret_cast<const sc_set_level*>(packet);
		auto level = pck->level;
		auto id = pck->id;
		game.GetCharacters()[id].SetLevel(level);
	}
	CASE PACKET_TYPE::Sc_remove_obj :
	{
		auto pck = reinterpret_cast<const sc_remove_obj*>(packet);
		game.GetCharacters().erase(pck->id);
	}
	CASE PACKET_TYPE::Sc_chat :
	{
		auto pck = reinterpret_cast<const sc_chat*>(packet);
		auto chatSize = sizeof(sc_chat::chat) - (sizeof(sc_chat) - pck->size);
		Chat chat;
		chat.speaker = pck->id;
		chat.timestamp = pck->time;
		memcpy(chat.mess, pck->chat, chatSize);
		ChatManager::Get().AddChat(chat);
		if (chatSize < 10)
			Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair(chat.mess, pck->time + 1500ms));
		else
			Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair("...", pck->time + 1500ms));
	}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
}
