#include "stdafx.h"
#include "Player.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////






/////////////////////////////////
// 
//			PlayerManager
// 
/////////////////////////////////

bool PlayerManager::Move(ID id, move_oper oper)
{
	static constexpr Position MOVE_OPER_TABLE[4]
	{ { 0, -1 }, { 0, 1 } ,{ 1, 0 } ,{ -1, 0 } };
	return players_[id].Move(MOVE_OPER_TABLE[static_cast<int>(oper)]);
}
