#include "stdafx.h"
#include "Networker.h"
#include "Game.h"
#include "System.h"
#include "Chat.h"
#include "Renderer.h"

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
		auto name = string{ pck->name, &pck->name[strnlen_s(pck->name, MAX_CHARACTER_NAME_SIZE)] }; trim(name);
		player.SetName(name);
		Ready_ = true;
	}
	CASE PACKET_TYPE::Sc_set_position :
	{
		auto pck = reinterpret_cast<const sc_set_position*>(packet);
		auto pos = pck->pos;
		auto id = pck->id;
		game.GetCharacters()[id].SetPos(pos);
	}
	CASE PACKET_TYPE::Sc_set_money :
	{
		auto pck = reinterpret_cast<const sc_set_money*>(packet);
		auto money = pck->money;
		auto id = pck->id;
		game.GetCharacters()[id].SetMoney(money);
	}
	CASE PACKET_TYPE::Sc_set_hp :
	{
		auto pck = reinterpret_cast<const sc_set_hp*>(packet);
		auto hp = pck->hp;
		auto id = pck->id;
		auto prevHp = game.GetCharacters()[id].GetHp();
		game.GetCharacters()[id].SetHp(hp);

		if (prevHp != WaitForRequestAnswer<decltype(prevHp)>() && prevHp != NeedRequest<decltype(prevHp)>())
		{
			if (0 < hp - prevHp)
				Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair("Hp +" + to_string(hp - prevHp), system_clock::now() + 800ms));
			else if (hp - prevHp < 0)
				Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair("Hp " + to_string(hp - prevHp), system_clock::now() + 800ms));
		}

		if (hp == 0)
		{
			if (id == Game::Get().GetId())
			{
				Chat chat;
				chat.speaker = SYSTEM_ID;
				chat.timestamp = system_clock::now();
				strcpy_s(chat.mess, "You died..");
				ChatManager::Get().AddChat(chat);
				strcpy_s(chat.mess, "..resurrected!!");
				ChatManager::Get().AddChat(chat);
				Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair(" #(X _ X)# ", system_clock::now() + 1200ms));
			}
			else Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair(" (X _ X) ", system_clock::now() + 800ms));
		}
	}
	CASE PACKET_TYPE::Sc_set_exp :
	{
		auto pck = reinterpret_cast<const sc_set_exp*>(packet);
		auto exp = pck->exp;
		auto id = pck->id;
		auto prevExp = game.GetCharacters()[id].GetExp();
		game.GetCharacters()[id].SetExp(exp);

		if (prevExp != WaitForRequestAnswer<decltype(prevExp)>() && prevExp != NeedRequest<decltype(prevExp)>())
		{
			if (0 < exp - prevExp)
				Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair("Exp +" + to_string(exp - prevExp), system_clock::now() + 1000ms));
		}
	}
	CASE PACKET_TYPE::Sc_set_level :
	{
		auto pck = reinterpret_cast<const sc_set_level*>(packet);
		auto level = pck->level;
		auto id = pck->id;
		auto prevLevel = game.GetCharacters()[id].GetLevel();
		game.GetCharacters()[id].SetLevel(level);

		if (prevLevel != WaitForRequestAnswer<decltype(prevLevel)>() && prevLevel != NeedRequest<decltype(prevLevel)>())
		{
			Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair("Level Up! (" + to_string(level) + ")", system_clock::now() + 1500ms));
		}
	}
	CASE PACKET_TYPE::Sc_set_name :
	{
		auto pck = reinterpret_cast<const sc_set_name*>(packet);
		auto id = pck->id;
		auto name = string{ pck->name, &pck->name[strnlen_s(pck->name, MAX_CHARACTER_NAME_SIZE)] }; trim(name);
		game.GetCharacters()[id].SetName(name);
	}
	CASE PACKET_TYPE::Sc_set_iteminstance_position :
	{
		auto pck = reinterpret_cast<const sc_set_iteminstance_position*>(packet);
		ItemInstance item;
		item.Type = pck->itemType;
		item.Pos = pck->pos;
		game.GetItemInstances().emplace(item);
	}
	CASE PACKET_TYPE::Sc_remove_iteminstance :
	{
		auto pck = reinterpret_cast<const sc_remove_iteminstance*>(packet);
		ItemInstance item;
		item.Type = pck->itemType;
		item.Pos = pck->pos;
		auto& itemInstances = game.GetItemInstances();
		game.GetItemInstances().erase(item);
	}
	CASE PACKET_TYPE::Sc_remove_obj :
	{
		auto pck = reinterpret_cast<const sc_remove_obj*>(packet);
		game.GetCharacters()[pck->id].SetState(eCharacterState::disable);
	}
	CASE PACKET_TYPE::Sc_use_skill :
	{
		auto pck = reinterpret_cast<const sc_use_skill*>(packet);
		const Position NEAR_POS_TABLE[4]
		{ { 0, -1 }, { 0, 1 } ,{ 1, 0 } ,{ -1, 0 } };
		switch (pck->skill)
		{
		case eSkill::attack:
		{
			auto now = clk::now();
			constexpr auto delay = 250ms;
			for (int i = 0; i < 4; i++)
			{
				auto pos = Game::Get().GetCharacters()[pck->id].GetPos();
				Renderer::Get().GetEffects().emplace_front(make_tuple(now, delay, eEffectType::attack0, pos + NEAR_POS_TABLE[i]));
				Renderer::Get().GetEffects().emplace_front(make_tuple(now + delay, delay, eEffectType::attack1, pos + NEAR_POS_TABLE[i]));
				Renderer::Get().GetEffects().emplace_front(make_tuple(now + delay + delay, delay, eEffectType::attack2, pos + NEAR_POS_TABLE[i]));
			}
		}
		CASE eSkill::haste :
		{

		}
		CASE eSkill::heal :
		{

		}
		CASE eSkill::set_teleport :
		{

		}
		CASE eSkill::teleport :
		{

		}break; default: cerr << "[[[eSkill]]]" << endl; break;
		}
	}
	CASE PACKET_TYPE::Sc_chat :
	{
		auto pck = reinterpret_cast<const sc_chat*>(packet);
		auto chatSize = sizeof(sc_chat::chat) - (sizeof(sc_chat) - pck->size);
		Chat chat;
		chat.speaker = pck->id;
		chat.timestamp = pck->time;
		memcpy(chat.mess, pck->chat, chatSize);
		if (strncmp(chat.mess, "(@)", 3))
			ChatManager::Get().AddChat(chat);
		else
		{
			memmove_s(chat.mess, chatSize + 1, chat.mess + 3, chatSize - 2);
			chatSize -= 3;
		}

		if (chatSize < 10)
			Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair(chat.mess, pck->time + 1500ms));
		else
			Game::Get().GetCharacters()[pck->id].SetSpeechBubble(make_pair("...", pck->time + 1500ms));
	}
	CASE PACKET_TYPE::Sc_sum_item :
	{
		auto pck = reinterpret_cast<const sc_sum_item*>(packet);
		game.GetPlayer().GetInventory().GetItems()[pck->type] += pck->changed;
	}
	CASE PACKET_TYPE::Sc_equip_item :
	{
		auto pck = reinterpret_cast<const sc_equip_item*>(packet);
		game.GetPlayer().Equipment_.Equip(pck->type);
	}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
}
