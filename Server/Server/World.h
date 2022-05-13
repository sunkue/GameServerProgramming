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
	GET_REF(Players);
public:
	shared_mutex PlayerLock;
	shared_mutex MonsterLock;
private:
	vector<StaticObj> Obstacles_; // ����������..
	concurrent_vector<DynamicObj> Monsters_; // ����ũ��..
	concurrent_unordered_set<Player*> Players_;
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
	Sector& GetSector(Position sector) { return Sectors_[sector.y][sector.x]; }
	array<Sector*, 4> GetNearSectors4(Position pos, Position sector);
public:

private:
	Sector Sectors_[SECTOR_NUM][SECTOR_NUM];
};

