#pragma once

#include "Obj.h"
#include "Player.h"

/////////////////////////////////////////////
//
//					Sector
// 
/////////////////////////////////////////////

class Sector
{
public:
	void update();
private:

public:

private:
	vector<DynamicObj> obstacles_; // ����������..
	concurrent_vector<DynamicObj> monsters_; // ����ũ��..
	concurrent_unordered_multiset<Player*> players_;
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

private:

public:

private:
	Sector sectors_[SECTOR_NUM - 2][SECTOR_NUM - 2];
};

