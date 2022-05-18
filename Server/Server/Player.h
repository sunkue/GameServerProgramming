#pragma once

#include "Obj.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////

class Player : public DynamicObj
{
public:
	virtual ~Player() = default;
	virtual bool Move(Position diff) override;
	virtual bool Enable() override;
	virtual bool Disable() override;
	virtual void Update() override;
	void UpdateViewList();
public:
	bool EraseFromViewList(ID);
	bool InsertToViewList(ID);
public:
	GET_REF(Exp);
	void AddExp(int add)
	{
		// 이 함수를 아토믹하게 쓸 수 있을까?
		auto requireExp = RequireExp(Hp_);
		Exp_ += add;
		if (requireExp <= Exp_) { Level_++; Exp_ -= requireExp; }
	};
protected:
	shared_mutex ViewLock;
	atomic_int Exp_{};
private:
	void HpRegen();
private:
	concurrent_unordered_set<ID> ViewList_;

	// 자주 변화되지 않으므로 atomic 혹은 lock써도 됨.
	array<ID, MAX_PARTY - 1> PartyCrews_;
};

