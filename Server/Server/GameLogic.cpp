#include "stdafx.h"
#include "GameLogic.h"
#include "GameBoard.h"
#include "Server.h"

bool GameLogic::move(NetID id, move_oper oper)
{
	glm::vec2 diff{};

	switch (oper)
	{
	case move_oper::up :
	{
		diff = { 0,-1 };
	}
	CASE move_oper::down :
	{
		diff = { 0, 1 };
	}
	CASE move_oper::right :
	{
		diff = { 1, 0 };
	}
	CASE move_oper::left :
	{
		diff = {-1, 0 };
	}
	}
	
	return players.at(id).move(diff);
}
