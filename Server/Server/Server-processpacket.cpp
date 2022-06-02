#include "stdafx.h"
#include "Server.h"
#include "World.h"
#include "Character.h"
#include "DataBase.h"

/////////////////////////////////////////////////////////////////////////////////////////

void Server::ProcessPacket(ID Id_, const void* const packet)
{
	auto SND2ME = [this, Id_](const void* const packet)
	{
		Clients_[Id_].DoSend(packet);
	};

	auto SND2OTHERS = [this, Id_](const void* const packet)
	{
		for (int i = 0; i < Clients_.size(); i++)
		{
			if (i != Id_) { Clients_[i].DoSend(packet); }
		}
	};

	auto SND2EVERY = [this](const void* const packet)
	{
		for (auto& c : Clients_)
		{
			c.DoSend(packet);
		}
	};

	auto SND2OEVERY_NEARSECTOR4 = [this](ID Id, const void* const packet)
	{
		auto& player = CharacterManager::Get().GetCharacters()[Id];
		auto Ns = World::Get().GetNearSectors4(player->GetPos(), player->GetSectorIdx());
		unordered_set<NetID> Ids;
		for (auto& ns : Ns)
		{
			for (auto& p : ns->GetPlayers())
			{
				Ids.insert(p->GetId());
			}
		}

		for (auto id : Ids)
			Clients_[id].DoSend(packet);
	};

	auto SND2OTHERS_NEARSECTOR4 = [this](ID Id, const void* const packet)
	{
		auto& player = CharacterManager::Get().GetCharacters()[Id];
		auto Ns = World::Get().GetNearSectors4(player->GetPos(), player->GetSectorIdx());
		unordered_set<NetID> Ids;
		for (auto& ns : Ns)
		{
			for (auto& p : ns->GetPlayers())
			{
				Ids.insert(p->GetId());
			}
		}

		for (auto id : Ids)
		{
			if (id == Id) continue;
			Clients_[id].DoSend(packet);
		}
	};

	auto SND2OEVERY_NEARSECTOR9 = [this](ID Id, const void* const packet)
	{
		auto& player = CharacterManager::Get().GetCharacters()[Id];
		auto Ns = World::Get().GetNearSectors9(player->GetPos(), player->GetSectorIdx());
		unordered_set<ID> Ids;
		for (auto& ns : Ns)
		{
			for (auto& p : ns->GetPlayers())
			{
				Ids.insert(p->GetId());
			}
		}

		for (auto id : Ids)
			Clients_[id].DoSend(packet);
	};

	auto SND2OTHERS_NEARSECTOR9 = [this](ID Id, const void* const packet)
	{
		auto& player = CharacterManager::Get().GetCharacters()[Id];
		auto Ns = World::Get().GetNearSectors9(player->GetPos(), player->GetSectorIdx());
		unordered_set<ID> Ids;
		for (auto& ns : Ns)
		{
			for (auto& p : ns->GetPlayers())
			{
				Ids.insert(p->GetId());
			}
		}

		for (auto id : Ids)
		{
			if (id == Id) continue;
			Clients_[id].DoSend(packet);
		}
	};

	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;

	// cerr << "[PACKET::" << +packet_type._to_string() << "]" << endl;

	switch (packet_type)
	{
	case PACKET_TYPE::Cs_login:
	{
		auto pck = reinterpret_cast<const cs_login*>(packet);

		QueryRequest q;
		q.Query = L"EXEC UserLogin "s;
		q.Query += wstring{ &pck->login_id[0], &pck->login_id[strnlen_s(pck->login_id, MAX_LOGIN_ID_SIZE)] };
		q.Query += L", "s;
		q.Query += wstring{ &pck->login_password[0], &pck->login_password[strnlen_s(pck->login_password, MAX_LOGIN_PASSWORD_SIZE)] };
		q.Targets = make_shared<vector<any>>(); q.Targets->reserve(2);
		q.Targets->emplace_back(make_any<SQLWCHAR>());
		q.Targets->emplace_back(make_any<SQLINTEGER>());
		q.Func = [this, Id_](const vector<any>& t)
		{
			auto res = any_cast<SQLWCHAR>(t[0]);
			auto dbId = any_cast<SQLINTEGER>(t[1]);
			switch (res)
			{
			case L'Y':
			{
				{
					// 로그인 성공
					sc_login_result result;
					result.id = Id_;
					Clients_[Id_].DoSend(&result);
				}

				CharacterManager::Get().InitFromDataBase(Id_, dbId);
			}
			CASE L'W' :
			{
				// 비밀번호 틀림
				sc_login_result result;
				result.id = -2;
				Clients_[Id_].DoSend(&result);
			}
			CASE L'N' :
			{
				// 없는 ID
				sc_login_result result;
				result.id = -1;
				Clients_[Id_].DoSend(&result);
			}
			}
		};
		DataBase::Get().AddQueryRequest(q);
	}
	CASE PACKET_TYPE::Cs_signup :
	{
		auto pck = reinterpret_cast<const cs_signup*>(packet);
		QueryRequest q;
		q.Query = L"EXEC UserSignup "s;
		q.Query += wstring{ pck->signup_id, &pck->signup_id[strnlen_s(pck->signup_id, MAX_LOGIN_ID_SIZE)] };
		q.Query += L", "s + wstring{ pck->signup_password, &pck->signup_password[strnlen_s(pck->signup_password, MAX_LOGIN_PASSWORD_SIZE)] };
		q.Targets = make_shared<vector<any>>();
		q.Targets->emplace_back(make_any<SQLWCHAR>()); // 'S' 'E'
		q.Func = [this, Id_](const vector<any>& t)
		{
			auto result = any_cast<SQLWCHAR>(t[0]);
			switch (result)
			{
			case L'S':
			{
				// success
				sc_signup_result signup_result;
				signup_result.result = 'S';
				Clients_[Id_].DoSend(&signup_result);
			}
			CASE L'E':
			{
				// exist
				sc_signup_result signup_result;
				signup_result.result = 'E';
				Clients_[Id_].DoSend(&signup_result);
			}
			break; default: break;
			}
		};
		DataBase::Get().AddQueryRequest(q);
	}
	CASE PACKET_TYPE::Cs_input :
	{
		auto pck = reinterpret_cast<const cs_input*>(packet);
		CharacterManager::Get().Move(Id_, pck->input);
	}
	CASE PACKET_TYPE::Cs_request_name :
	{
		auto pck = reinterpret_cast<const cs_request_name*>(packet);
		auto player = reinterpret_cast<Player*>(CharacterManager::Get().GetCharacters()[pck->id].get());
		auto name = player->GetName();
		sc_set_name set_name;
		set_name.id = pck->id;
		size_t temp; wcstombs_s(&temp, set_name.name, name.c_str(), name.size());
		set_name.size -= static_cast<decltype(set_name.size)>(MAX_CHARACTER_NAME_SIZE - strlen(set_name.name));
		SND2ME(&set_name);
	}
	CASE PACKET_TYPE::Cs_input_timestamp :
	{
		auto pck = reinterpret_cast<const cs_input_timestamp*>(packet);
		if (CharacterManager::Get().Move(Id_, pck->input))
		{
			sc_set_position_timestamp set_pos;
			auto pos = CharacterManager::Get().GetPosition(Id_);
			set_pos.id = NetID(Id_);
			set_pos.pos = pos;
			set_pos.timestamp = pck->timestamp;
			SND2ME(&set_pos);
		}
	}
	CASE PACKET_TYPE::Cs_chat :
	{
		auto pck = reinterpret_cast<const cs_chat*>(packet);

		sc_chat chat;
		chat.id = Id_;
		chat.time = system_clock::now();
		strcpy_s(chat.chat, MAX_CHAT_SIZE, pck->chat);
		chat.size -= static_cast<decltype(chat.size)>(MAX_CHAT_SIZE - strlen(chat.chat));
		SND2OEVERY_NEARSECTOR4(Id_, &chat);
	}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
