#include "stdafx.h"
#include "Server.h"
#include "World.h"
#include "Character.h"

/////////////////////////////////////////////////////////////////////////////////////////

void Server::ProcessPacket(ID Id_, const void* const packet)
{
	// conditional funtion => 함수객체 전달해서 처리.
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

	auto SND2OTHERS_NEARSECTOR4 = [this](ID Id, const void* const packet)
	{
		auto& player = CharacterManager::Get().GetCharacters()[Id];
		auto Ns = World::Get().GetNearSectors4(player->GetPos(), player->GetSectorIdx());
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
		CASE PACKET_TYPE::Cs_hi :
		{
			{
				sc_hi hi;
				hi.id = NetID(Id_);
				SND2ME(&hi);
			}

			//if (PlayerManager::get().Move(id, Position{}))
			if (CharacterManager::Get().InitialMove(Id_, Position{ rand() % MAP_SIZE, rand() % MAP_SIZE }))
			{
				sc_set_position set_pos;
				auto pos = CharacterManager::Get().GetPosition(Id_);
				set_pos.id = NetID(Id_);
				set_pos.pos = pos;
				cout << pos.x << " " << pos.y << endl;
				SND2ME(&set_pos);
			}
			else
			{
				cerr << "[!!!]OverflowedInitPosition" << CharacterManager::Get().GetPosition(Id_).x << " " << CharacterManager::Get().GetPosition(Id_).y << endl;
			}

			{
				sc_ready ready;
				SND2ME(&ready);
			}

			CharacterManager::Get().Enable(Id_);
		}
		CASE PACKET_TYPE::Cs_input :
		{
			auto pck = reinterpret_cast<const cs_input*>(packet);
			CharacterManager::Get().Move(Id_, pck->input);
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
