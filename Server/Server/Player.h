#pragma once

#include "Obj.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////

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
	GET(id);
protected:
	shared_mutex viewLock;
	ID id_{ -1 };
private:
	concurrent_unordered_set<ID> viewList_;
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
		for (ID id = 0; id < players_.size(); id++)
		{
			players_[id].id_ = id;
		}
	}
public:
	bool Move(ID id, move_oper oper);
	bool Move(ID id, Position to);
	Position GetPosition(ID id) { return players_[id].get_pos(); }
	void SetPosition(ID id, Position pos) { players_[id].set_pos(pos); }
public:
	void Enable(ID id) { players_[id].Enable(); }
	void Disable(ID id) { players_[id].Disable(); }
private:
	array<Player, MAX_PLAYER> players_;
};