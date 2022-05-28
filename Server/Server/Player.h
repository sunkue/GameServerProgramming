#pragma once

#include "Character.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////

class Player : public Character
{
public:
	Player(ID id) : Character{ id } {};
	virtual ~Player() = default;
	virtual bool Move(Position diff) override;
	virtual bool Enable() override;
	virtual bool Disable() override;
	virtual void Update() override;
	virtual void Regen() override;
	void UpdateViewList();
	bool EraseFromViewList(ID);
	bool InsertToViewList(ID);
	GET_REF(Exp);
	// amount could be minus. ex) die-regen
	void ExpSum(ID agent, int amount);
	virtual void HpDecrease(ID agent, int amount) override;
	virtual void HpIncrease(ID agent, int amount) override;
protected:
private:
	void HpRegen();
public:
protected:
	shared_mutex ViewLock;
private:
	atomic_int Exp_{};
	concurrent_unordered_set<ID> ViewList_;
	// 자주 변화되지 않으므로 atomic 혹은 lock써도 됨.
	array<ID, MAX_PARTY - 1> PartyCrews_{ -1 };
};

