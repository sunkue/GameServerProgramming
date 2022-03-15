#include "stdafx.h"
#include "Obj.h"
#include "GameBoard.h"

bool Obj::move(glm::vec2 diff)
{
	pos_ += diff;

	auto n = GameBoard::get().get_n();
	if (n <= pos_.x || pos_.x < 0 || n <= pos_.y || pos_.y < 0)
	{
		pos_ -= diff;
		return false;
	}

	return true;;
}
