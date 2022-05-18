#pragma once

#include "Obj.h"

/////////////////////////////////
// 
//			CharacterManager
// 
/////////////////////////////////

class CharacterManager
{
	SINGLE_TON(CharacterManager);
public:
	bool Move(ID Id_, move_oper oper);
	bool Move(ID Id_, Position to);
	Position GetPosition(ID Id_) { return Characters_[Id_]->GetPos(); }
	void SetPosition(ID Id_, Position pos) { Characters_[Id_]->SetPos(pos); }
public:
	void Enable(ID Id_) { Characters_[Id_]->Enable(); }
	void Disable(ID Id_) { Characters_[Id_]->Disable(); }
private:
	// PLAYER		[ 0, MAX_PLAYER )
	// MONSTER	[ MAX_PLAYER, MAX_PLAYER + MAX_MONSTER )
	// NPC		[ MAX_PLAYER + MAX_MONSTER, MAX_CHARACTER )
	array<unique_ptr<DynamicObj>, MAX_CHARACTER> Characters_;
	// 포인터로 만들자.
};