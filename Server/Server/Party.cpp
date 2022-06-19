#include "stdafx.h"
#include "Party.h"
#include "Character.h"
#include "Player.h"
#include "Server.h"

ID PartyManager::CreateParty(ID characterId)
{
	for (int i = 0; i < Partys_.size(); i++)
	{
		bool f = true;
		if (Partys_[i].Empty.compare_exchange_strong(f, false))
		{
			JoinParty(i, characterId);
			return i;
		}
	}

	return -1;
}

const Party* PartyManager::GetParty(ID partyId)
{
	if (partyId < 0 || Partys_.size() <= partyId) return nullptr;
	else return &Partys_[partyId];
}

bool PartyManager::JoinParty(ID partyId, ID characterId)
{
	auto player = reinterpret_cast<Player*>(CharacterManager::Get().GetCharacters()[characterId].get());
	if (auto prevPartyId = player->GetPartyId(); 0 <= prevPartyId)
		ExitParty(prevPartyId, characterId);

	for (auto& c : Partys_[partyId].PartyCrews_)
	{
		if (c < 0)
		{
		//	cout << "JOIN " << partyId << " " << characterId << endl;
			c = characterId;
			player->SetPartyId(partyId);

			sc_insert_partycrew insertCrew;
			for (auto& c : Partys_[partyId].PartyCrews_)
			{
				if (c < 0) continue;
				insertCrew.crewId = characterId;
				Server::Get().GetClients()[c].DoSend(&insertCrew);
				insertCrew.crewId = c;
				Server::Get().GetClients()[characterId].DoSend(&insertCrew);
			}

			return true;
		}
	}

	return false;
}

bool PartyManager::ExitParty(ID partyId, ID characterId)
{
	auto player = reinterpret_cast<Player*>(CharacterManager::Get().GetCharacters()[characterId].get());
	bool ret = false;
	int inChairCount = 0;

	if (partyId < 0)
	{
		player->SetPartyId(-1);
		return ret;
	}

	for (int i = 0; i < Partys_[partyId].PartyCrews_.size(); i++)
	{
		if (0 <= Partys_[partyId].PartyCrews_[i])
			inChairCount++;

		if (Partys_[partyId].PartyCrews_[i] == characterId)
		{
		//	cout << "EXIT " << partyId << " " << characterId << endl;
			sc_erase_partycrew eraseCrew;
			for (auto& c : Partys_[partyId].PartyCrews_)
			{
				if (c < 0) continue;
				eraseCrew.crewId = characterId;
				Server::Get().GetClients()[c].DoSend(&eraseCrew);
				eraseCrew.crewId = c;
				Server::Get().GetClients()[characterId].DoSend(&eraseCrew);
			}

			player->SetPartyId(-1);
			Partys_[partyId].PartyCrews_[i] = -1;

			ret = true;
		}
	}
	if (0 == inChairCount)
		Partys_[partyId].Empty = true;

	return ret;
}
