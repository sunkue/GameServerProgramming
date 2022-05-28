#include "stdafx.h"
#include "Character.h"
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
	cerr << "initiallize Players..";
	for (ID _id = 0; _id < MAX_PLAYER; _id++)
	{
		auto id = _id;
		Characters_[id] = make_unique<Player>(id);
	}
	cerr << "..done" << endl;

	// Monsters
	cerr << "initiallize Monsters..";
	for (ID _id = 0; _id < MAX_MONSTER; _id++)
	{
		auto id = _id + MAX_PLAYER;
		Characters_[id] = make_unique<Monster>(id);
		InitialMove(id, { rand() % MAP_SIZE, rand() % MAP_SIZE });
	}
	cerr << "..done" << endl;

	// NPCs
	cerr << "initiallize NPCs..";
	for (ID _id = 0; _id < MAX_NPC; _id++)
	{
		auto id = _id + MAX_PLAYER + MAX_MONSTER;
		Characters_[id] = make_unique<Character>(id);
		//		Characters_[id]->Enable();
	}
	cerr << "..done" << endl;
}

bool CharacterManager::Move(ID Id_, eMoveOper oper)
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

bool CharacterManager::InitialMove(ID Id_, Position to)
{
	Characters_[Id_]->StartPosition_ = to;
	return Move(Id_, to);
}

void Character::Regen()
{
	Hp_ = 1;
	Move(StartPosition_ - GetPos());
}

void Character::Attack()
{
	if (!Attackable_) return;
	Attackable_ = false;
	AttackImpl();
	EventManager::Get().AddEvent({ [&Attackable = Attackable_]()
		{ Attackable = true; }, AttackCooltime_ });
}

bool Character::Move(Position diff)
{
	if (!Moveable_) return false;
	auto ret = DynamicObj::Move(diff);
	if (ret)
	{
		Moveable_ = false;
		EventManager::Get().AddEvent({ [&Moveable = Moveable_]()
			{ Moveable = true; }, MovementCooltime_ });
	}
	return ret;
}
