#pragma once

#include "Obj.h"
#include "Player.h"

class Game
{
	SINGLE_TON(Game) { Characters_.reserve(MAX_PLAYER); }
public:
	void update();
	void init();
	GET(Id); SET(Id);
	GET_REF_UNSAFE(Characters);
	Character& GetPlayer() { return Characters_[Id_]; }
private:
	ID Id_{};
	unordered_map<ID, Character> Characters_;
};
