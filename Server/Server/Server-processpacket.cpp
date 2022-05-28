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
			if (CharacterManager::Get().Move(Id_, Position{ rand() % MAP_SIZE, rand() % MAP_SIZE }))
			{
				sc_set_position set_pos;
				auto pos = CharacterManager::Get().GetPosition(Id_);
				set_pos.id = NetID(Id_);
				set_pos.pos = pos;
				SND2ME(&set_pos);
			}
			else
			{
				cerr << "[!!!]OverflowedInitPosition" << CharacterManager::Get().GetPosition(Id_).x << " " << CharacterManager::Get().GetPosition(Id_).y << endl;
			}


			/*
			for (int i = 0; i < clients.size(); i++)
			{
				if (clients[i].IsBad())
					continue;
				auto pos = PlayerManager::get().GetPosition(i);
				sc_set_position other_pos;
				other_pos.id = NetID(i);
				other_pos.pos = pos;
				SND2ME(&other_pos);
			}
			*/

			/*
			{
				sc_set_position set_pos;
				set_pos.id = NetID(id); /////
				auto pos = PlayerManager::get().GetPosition(id);
				set_pos.pos = pos;
				SND2EVERY(&set_pos);
			}
			*/

			{
				sc_ready ready;
				SND2ME(&ready);
			}

			CharacterManager::Get().Enable(Id_);
		}
		CASE PACKET_TYPE::Cs_input :
		{
			auto pck = reinterpret_cast<const cs_input*>(packet);
			if (CharacterManager::Get().Move(Id_, pck->input))
			{
				sc_set_position set_pos;
				auto pos = CharacterManager::Get().GetPosition(Id_);
				set_pos.id = NetID(Id_);
				set_pos.pos = pos;
				set_pos.timestamp = pck->timestamp;
				SND2ME(&set_pos);
			}
		}
	break; default: cerr << "[[[!!]]]" << endl; break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
