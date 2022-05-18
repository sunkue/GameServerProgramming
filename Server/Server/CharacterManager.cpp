#include "stdafx.h"
#include "CharacterManager.h"
#include "TimerEvent.h"
#include "Monster.h"
#include "Player.h"

/////////////////////////////////
// 
//			CharacterManager
// 
/////////////////////////////////

CharacterManager::CharacterManager()
{
	// Players
	for (ID _id = 0; _id < MAX_PLAYER; _id++)
	{
		auto id = _id;
		Characters_[id] = make_unique<Player>();
		Characters_[id]->Id_ = id;
	}
	// Monsters
	for (ID _id = 0; _id < MAX_MONSTER; _id++)
	{
		auto id = _id + MAX_PLAYER;
		Characters_[id] = make_unique<Monster>();
		Characters_[id]->Id_ = id;
		Characters_[id]->Move({ rand() % MAP_SIZE, rand() % MAP_SIZE });
	}
	// NPCs
	for (ID _id = 0; _id < MAX_NPC; _id++)
	{
		auto id = _id + MAX_PLAYER + MAX_MONSTER;
		Characters_[id] = make_unique<DynamicObj>();
		Characters_[id]->Id_ = id;
		//		Characters_[id]->Enable();
	}
}

bool CharacterManager::Move(ID Id_, move_oper oper)
{
	const Position MOVE_OPER_TABLE[4]
	{ { 0, -1 }, { 0, 1 } ,{ 1, 0 } ,{ -1, 0 } };
	auto moved = Characters_[Id_]->Move(MOVE_OPER_TABLE[static_cast<int>(oper)]);
	return moved;
}

bool CharacterManager::Move(ID Id_, Position to)
{
	auto moved = Characters_[Id_]->Move(to);
	return moved;
}

