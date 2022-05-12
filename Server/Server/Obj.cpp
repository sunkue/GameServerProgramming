#include "stdafx.h"
#include "Obj.h"
#include "World.h"


bool DynamicObj::Move(const Position diff)
{
	Position pos;
	Position sector;

	{
		shared_lock lck(positionLock);
		pos = get_pos();
		sector = get_sector();
	}

	pos += diff;

	if (IsOverFlowedPosition(pos))
	{
		pos -= diff;
		return false;
	}


	auto newSector = GetSectorByPosition(pos);

	{
		unique_lock lck(positionLock);
		set_pos(pos);
	}

	if (sector != newSector)
	{
		World::get().ChangeSector(this, newSector);
	}

	return true;
}

bool StaticObj::IsInSight(Position target)
{
	auto diff = target - pos_;
	auto maxCompDiff = glm::compMax(glm::abs(diff));
	return maxCompDiff <= sight_ / 2;
}


