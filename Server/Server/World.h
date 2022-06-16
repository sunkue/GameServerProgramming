#pragma once

#include "Obj.h"
#include "Player.h"
#include "Monster.h"
#include "Item.h"

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
public:
	void EraseObjFromSector(StaticObj* obj);
	void InsertObjSector(StaticObj* obj);
	GET_REF(Players);
	GET_REF(Monsters);
	GET_REF(Obstacles);
	GET_REF(Npcs);
	GET_REF_UNSAFE(ItemInstances);
public:
	shared_mutex PlayerLock;
	shared_mutex MonsterLock;
	shared_mutex ItemInstanceLock;
private:
	vector<DynamicObj*> Obstacles_;					// 고정데이터..
	concurrent_unordered_set<ItemInstance> ItemInstances_;
	concurrent_unordered_set<Monster*> Monsters_;	
	concurrent_unordered_set<DynamicObj*> Npcs_;		
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
	array<Sector*, 4> GetNearSectors4(Position pos);
	array<Sector*, 4> GetNearSectors4(Position pos, Position sector);
	array<Sector*, 9> GetNearSectors9(Position pos, Position sector);
public:

private:
	Sector Sectors_[SECTOR_NUM][SECTOR_NUM];
};

