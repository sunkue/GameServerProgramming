#pragma once

#include "Obj.h"
#include "Player.h"

class Game
{
	SINGLE_TON(Game) { Players_.reserve(MAX_PLAYER); }
public:
	void update();
	void init();
	GET(Id); SET(Id);
	GET_REF_UNSAFE(Players);
	Player& GetPlayer() { return Players_[Id_]; }
private:
	ID Id_{};
	unordered_map<ID, Player> Players_;
};
