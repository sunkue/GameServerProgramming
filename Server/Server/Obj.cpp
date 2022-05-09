#include "stdafx.h"
#include "Obj.h"
#include "World.h"

bool DynamicObj::Move(const Position diff)
{
	unique_lock lck(positionLock);

	pos_ += diff;

	if (pos_.x < 0 || MAP_SIZE <= pos_.x || pos_.y < 0 || MAP_SIZE <= pos_.y)
	{
		pos_ -= diff;
		return false;
	}

	return true;
}
