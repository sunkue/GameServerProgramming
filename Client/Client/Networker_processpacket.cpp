#include "stdafx.h"
#include "Networker.h"
#include "Game.h"
#include "System.h"
#include "Chat.h"

void Networker::ProcessPacket(const void* const packet)
{
	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	auto& game = Game::Get();
	// cerr << "Recv [PACKET::" << +packet_type._to_string() << "]" << endl;
	switch (packet_type)
	{
	case PACKET_TYPE::Sc_login_result:
	{
		auto pck = reinterpret_cast<const sc_login_result*>(packet);
		switch (pck->id)
		{
		case -1: // NO ID
		{
			cout << "======[!!] ID don't exist. try agin. =======" << endl;
			LoginSignupOnConsole();
		}
		CASE - 2 : // WRONG PASSWORD
		{
			cout << "======[!!] Wrong Password. try agin. =======" << endl;
			LoginSignupOnConsole();
		}
		break; default:
		{
			cout << "======[!!] LOGIN COMPLETE. Plz Enjoy Game  =======" << endl;
			game.SetId(pck->id);
			game.GetCharacters()[game.GetId()];
		} break;
		}
	}
	CASE PACKET_TYPE::Sc_signup_result :
	{
		auto pck = reinterpret_cast<const sc_signup_result*>(packet);
		switch (pck->result)
		{
		case 'S':
		{
			cout << "======[*] SignUp SUCCESS!!! Login now :) =======" << endl;
			LoginSignupOnConsole();
		}
		CASE 'E' :
		{
			cout << "======[!!] SignUp Fail. Id Already Exist.. :( =======" << endl;
			LoginSignupOnConsole();
		}
		break; default: break;
		}
	}
	CASE PACKET_TYPE::Sc_ready :
	{
		auto pck = reinterpret_cast<const sc_ready*>(packet);
		auto& player = game.GetCharacters()[game.GetId()];
		player.SetLevel(pck->level);
		player.SetHp(pck->hp);
		player.SetMoney(pck->money);
		player.SetExp(pck->exp);
		player.SetName(string{ pck->name, &pck->name[strnlen_s(pck->name, MAX_CHARACTER_NAME_SIZE)] });

		Ready_ = true;
		{
			cs_input input;
			input.input = eMoveOper::up;
			Networker::Get().DoSend(&input);
			input.input = eMoveOper::down;
			Networker::Get().DoSend(&input);
			input.input = eMoveOper::right;
			Networker::Get().DoSend(&input);
			input.input = eMoveOper::left;
			Networker::Get().DoSend(&input);
		}
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
	CASE PACKET_TYPE::Sc_set_name :
	{
		auto pck = reinterpret_cast<const sc_set_name*>(packet);
		auto id = pck->id;
		game.GetCharacters()[id].SetName(string{ pck->name, &pck->name[strnlen_s(pck->name, MAX_CHARACTER_NAME_SIZE)] });
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
