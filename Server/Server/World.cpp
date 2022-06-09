#include "stdafx.h"
#include "World.h"
#include "Obj.h"
#include "Player.h"

Position GetSectorByPosition(Position pos)
{
	return Position{ pos.x / SECTOR_SIZE, pos.y / SECTOR_SIZE };
}

pair<Position, Position> GetSectorRange(Position sector)
{
	Position min;
	Position max;

	min.x = sector.x * SECTOR_SIZE;
	min.y = sector.y * SECTOR_SIZE;
	max = min + SECTOR_SIZE;

	return pair<Position, Position>{min, max};
}

/////////////////////////////////////////////
//
//					Sector
// 
/////////////////////////////////////////////

void Sector::Update()
{
	// player는 업데이트 하지 않음.
}

void Sector::EraseObjFromSector(StaticObj* obj)
{
	if (auto monster = dynamic_cast<Monster*>(obj))
	{
		unique_lock lck{ MonsterLock };
		Monsters_.unsafe_erase(monster);
	}
	else if (auto player = dynamic_cast<Player*>(obj))
	{
		unique_lock lck{ PlayerLock };
		Players_.unsafe_erase(player);
	}
}

void Sector::InsertObjSector(StaticObj* obj)
{
	if (auto monster = dynamic_cast<Monster*>(obj))
	{
		shared_lock lck{ MonsterLock };
		Monsters_.insert(monster);
	}
	else if (auto player = dynamic_cast<Player*>(obj))
	{
		shared_lock lck{ PlayerLock };
		Players_.insert(player);
	}
	else if (auto obstracles = dynamic_cast<DynamicObj*>(obj))
	{
		Obstacles_.push_back(obstracles);
	}
}

/////////////////////////////////////////////
//
//					World
// 
/////////////////////////////////////////////
void World::ChangeSector(StaticObj* obj, Position newSectorIdx)
{
	if (!obj) return;
	auto& oldSector = GetSector(obj->GetSectorIdx());
	auto& newSector = GetSector(newSectorIdx);

	oldSector.EraseObjFromSector(obj);
	// 여기서 오브젝트는 어느섹터에도 존재하지 않을 수 있음.
	newSector.InsertObjSector(obj);

	obj->SetSectorIdx(newSectorIdx);
}

array<Sector*, 4> World::GetNearSectors4(Position pos)
{
	return GetNearSectors4(pos, pos / SECTOR_SIZE);
}

array<Sector*, 4> World::GetNearSectors4(Position pos, Position sectorIdx)
{
	auto range = GetSectorRange(sectorIdx);
	auto midPoint = (range.first + range.second) / 2;
	auto isLess = glm::lessThan(pos, midPoint);

	// 대부분의 경우 섹터 끝에 도달하지 않으므로, nullptr을 보내주는 것 보다 중복섹터를 보내주는게 좋아뵌다.
	Position isNotLimit_DiffMultyflier{ !IsLimitSectorIdx(sectorIdx.x) ,  !IsLimitSectorIdx(sectorIdx.y) };

	if (isLess.x)
	{
		if (isLess.y)
		{
			// LU
			return array<Sector*, 4>{&GetSector(sectorIdx), & GetSector(sectorIdx + Position{ -1, -1 }*isNotLimit_DiffMultyflier)
				, & GetSector(sectorIdx + Position{ -1, 0 }*isNotLimit_DiffMultyflier), & GetSector(sectorIdx + Position{ 0, -1 }*isNotLimit_DiffMultyflier) };
		}
		else
		{
			// LD
			return array<Sector*, 4>{&GetSector(sectorIdx), & GetSector(sectorIdx + Position{ -1, 1 }*isNotLimit_DiffMultyflier)
				, & GetSector(sectorIdx + Position{ -1, 0 }*isNotLimit_DiffMultyflier), & GetSector(sectorIdx + Position{ 0, 1 }*isNotLimit_DiffMultyflier) };
		}
	}
	else
	{
		if (isLess.y)
		{
			// RU
			return array<Sector*, 4>{&GetSector(sectorIdx), & GetSector(sectorIdx + Position{ 1, -1 }*isNotLimit_DiffMultyflier)
				, & GetSector(sectorIdx + Position{ 1, 0 }*isNotLimit_DiffMultyflier), & GetSector(sectorIdx + Position{ 0, -1 }*isNotLimit_DiffMultyflier) };
		}
		else
		{
			// RD
			return array<Sector*, 4>{&GetSector(sectorIdx), & GetSector(sectorIdx + Position{ 1, 1 }*isNotLimit_DiffMultyflier)
				, & GetSector(sectorIdx + Position{ 1, 0 }*isNotLimit_DiffMultyflier), & GetSector(sectorIdx + Position{ 0, 1 }*isNotLimit_DiffMultyflier) };
		}
	}
}

array<Sector*, 9> World::GetNearSectors9(Position pos, Position sectorIdx)
{
	Position isNotLimitMin{ !IsLimitSectorIdxMin(sectorIdx.x) ,  !IsLimitSectorIdxMin(sectorIdx.y) };
	Position isNotLimitMax{ !IsLimitSectorIdxMax(sectorIdx.x) ,  !IsLimitSectorIdxMax(sectorIdx.y) };

	return array<Sector*, 9>{&GetSector(sectorIdx)
		, & GetSector(sectorIdx + Position{ -1, -1 } *Position{ isNotLimitMin.x	, isNotLimitMin.y })
		, & GetSector(sectorIdx + Position{ +0, -1 } *Position{ 0				, isNotLimitMin.y })
		, & GetSector(sectorIdx + Position{ +1, -1 } *Position{ isNotLimitMax.x	, isNotLimitMin.y })
		, & GetSector(sectorIdx + Position{ -1, +0 } *Position{ isNotLimitMin.x	, 0 })
		, & GetSector(sectorIdx + Position{ +1, +0 } *Position{ isNotLimitMax.x	, 0 })
		, & GetSector(sectorIdx + Position{ -1, +1 } *Position{ isNotLimitMin.x	, isNotLimitMax.y })
		, & GetSector(sectorIdx + Position{ +0, +1 } *Position{ 0				, isNotLimitMax.y })
		, & GetSector(sectorIdx + Position{ +1, +1 } *Position{ isNotLimitMax.x	, isNotLimitMax.y })
	};
}


