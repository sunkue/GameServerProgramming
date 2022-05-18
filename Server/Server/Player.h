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
		// �� �Լ��� ������ϰ� �� �� ������?
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

	// ���� ��ȭ���� �����Ƿ� atomic Ȥ�� lock�ᵵ ��.
	array<ID, MAX_PARTY - 1> PartyCrews_;
};

