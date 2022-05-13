#include "stdafx.h"
#include "Obj.h"
#include "World.h"


bool DynamicObj::Move(const Position diff)
{
	Position pos;
	Position sector;

	{
		shared_lock lck(PositionLock);
		pos = GetPos();
		sector = GetSectorIdx();
	}

	pos += diff;

	if (IsOverFlowedPosition(pos))
	{
		pos -= diff;
		return false;
	}


	auto newSector = GetSectorByPosition(pos);

	{
		unique_lock lck(PositionLock);
		SetPos(pos);
	}

	if (sector != newSector)
	{
		World::Get().ChangeSector(this, newSector);
	}

	return true;
}

bool StaticObj::IsInSight(Position target)
{
	auto diff = target - Pos_;
	auto maxCompDiff = glm::compMax(glm::abs(diff));
	return maxCompDiff <= SightRange_ / 2;
}


