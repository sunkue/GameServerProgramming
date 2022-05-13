#pragma once

#include "Obj.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////

inline int RequireExp(int HP) { return 50 * pow(2, HP); }


class Player : public DynamicObj
{
	friend class PlayerManager;
public:
	void Enable();
	void Disable();
	void UpdateViewList();
	virtual ~Player() = default;
private:
	bool EraseFromViewList(ID);
	bool InsertToViewList(ID);
public:
	GET(Id);
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
	ID Id_{ -1 };
	atomic_int Exp_{};
private:
	concurrent_unordered_set<ID> ViewList_;

	// 자주 변화되지 않으므로 atomic 혹은 lock써도 됨.
	array<ID, MAX_PARTY - 1> PartyCrews_; 
};

/////////////////////////////////
// 
//			PlayerManager
// 
/////////////////////////////////

class PlayerManager
{
	SINGLE_TON(PlayerManager)
	{
		for (ID Id_ = 0; Id_ < players_.size(); Id_++)
		{
			players_[Id_].Id_ = Id_;
		}
	}
public:
	bool Move(ID Id_, move_oper oper);
	bool Move(ID Id_, Position to);
	Position GetPosition(ID Id_) { return players_[Id_].GetPos(); }
	void SetPosition(ID Id_, Position pos) { players_[Id_].SetPos(pos); }
public:
	void Enable(ID Id_) { players_[Id_].Enable(); }
	void Disable(ID Id_) { players_[Id_].Disable(); }
private:
	array<Player, MAX_PLAYER> players_;
};