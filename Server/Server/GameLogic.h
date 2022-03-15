#pragma once

#include "Player.h"

class GameLogic
{
	SINGLE_TON(GameLogic) = default;
public:

	bool move(NetID id, move_oper oper);
	glm::vec2 get_position(NetID id){return players.at(id).get_pos(); }
private:
	array<Player, MAX_PLAYER> players{};
};

