#pragma once



class Party
{
	friend class PartyManager;
public:
	Party() { for (auto& pc : PartyCrews_) pc = -1; }
	GET_REF(PartyCrews);
protected:
	array<atomic<ID>, MAX_PARTY_CREW> PartyCrews_;
	atomic_bool Empty{ true };
private:
};

class PartyManager
{
	SINGLE_TON(PartyManager) = default;
public:
	ID CreateParty(ID characterId);
	const Party* GetParty(ID partyId);
	bool JoinParty(ID partyId, ID characterId);
	bool ExitParty(ID partyId, ID characterId);
private:
	array<Party, MAX_PARTY> Partys_;
};