#pragma once

#include "Obj.h"
#include "Player.h"

class Game
{
	SINGLE_TON(Game) = default;

public:
	void update();
	void init();
public:
	GET(n);
	SET(n);
	GET(id);
	SET(id);
	GET_REF_UNSAFE(players);

private:
	int n_{};
private:
	ID id_{};
	unordered_map<ID, Player> players_;
};
