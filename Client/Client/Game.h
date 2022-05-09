#pragma once

#include "Obj.h"
#include "Player.h"

class Game
{
	SINGLE_TON(Game) { players_.reserve(MAX_PLAYER); }
public:
	void update();
	void init();
	GET(id); SET(id);
	GET_REF_UNSAFE(players);
	Player& GetPlayer() { return players_[id_]; }
private:
	ID id_{};
	unordered_map<ID, Player> players_;
};
