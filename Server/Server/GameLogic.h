#pragma once

#include "Player.h"


class GameLogic
{
	SINGLE_TON(GameLogic) = default;
public:
	bool move(ID id, move_oper oper);
	glm::vec2 get_position(ID id) { return players[id].get_pos(); }
	glm::vec2 set_position(ID id, glm::vec2 pos) { players[id].set_pos(pos); }
public:
	void erase(ID id) { players.erase(id); }
private:
	unordered_map<ID, class Player> players;
};

