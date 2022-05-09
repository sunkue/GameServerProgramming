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

private:

public:

private:
};

/////////////////////////////////
// 
//			PlayerManager
// 
/////////////////////////////////

class PlayerManager
{
	SINGLE_TON(PlayerManager) = default;
public:
	bool Move(ID id, move_oper oper);
	Position GetPosition(ID id) { return players_[id].get_pos(); }
	void SetPosition(ID id, Position pos) { players_[id].set_pos(pos); }
public:
	void Disable(ID id) { players_[id].set_enable(false); }
private:
	array<Player, MAX_PLAYER> players_;
};