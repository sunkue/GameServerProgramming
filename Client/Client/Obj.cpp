#include "stdafx.h"
#include "Obj.h"
#include "Game.h"

bool Obj::move(glm::vec2 diff) noexcept
{
	pos_ += diff;
	
	auto n = Game::get().get_n();
	if (n <= pos_.x || pos_.x < 0 || n <= pos_.y || pos_.y < 0)
	{
		pos_ -= diff;
		return false;
	}

	return true;;
}
