#pragma once

#include "Obj.h"
#include "Player.h"

// [ min, max )
pair<Position, Position> GetSectorRange(Position sector);
Position GetSectorByPosition(Position pos);

/////////////////////////////////////////////
//
//					Sector
// 
/////////////////////////////////////////////

class Sector
{
	friend class World;
public:
	void Update();
private:

public:
	void EraseObjFromSector(StaticObj* obj);
	void InsertObjSector(StaticObj* obj);
	GET_REF(players);
public:
	shared_mutex playerLock;
	shared_mutex monsterLock;
private:
	vector<StaticObj> obstacles_; // 고정데이터..
	concurrent_vector<DynamicObj> monsters_; // 고정크기..
	concurrent_unordered_set<Player*> players_;
};

/////////////////////////////////////////////
//
//					World
// 
/////////////////////////////////////////////

class World
{
	SINGLE_TON(World) = default;
public:
	void ChangeSector(StaticObj* obj, Position newSector);
	Sector& GetSector(Position sector) { return sectors_[sector.y][sector.x]; }
	array<Sector*, 4> GetNearSectors4(Position pos, Position sector);
public:

private:
	Sector sectors_[SECTOR_NUM][SECTOR_NUM];
};

