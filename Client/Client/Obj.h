#pragma once

#include "ReplicateHelper.h"

enum class OBJ_TYPE : int
{
	Wlook = 0,
	Wknight = 1,
	Wbishop = 2,
	Wking = 3,
	Wqueen = 4,
	Wpawn = 5,

	Blook = 15,
	Bknight = 16,
	Bbishop = 17,
	Bking = 18,
	Bqueen = 19,
	Bpawn = 10,
};

class StaticObj
{
public:
	StaticObj(ID id, Position pos) : Id_{id}, Pos_ { pos } {}
public:
	GET(Id);
	GET(Pos);
	SET(Pos);
protected:
	Position Pos_{};
	ID Id_{ -1 };
};


inline int RequireExp(int HP) { return static_cast<int>(50 * pow(2, HP)); }
inline int MaxHp(int Level) { return Level * 100; }

class DynamicObj : public StaticObj
{
public:
	DynamicObj(ID id, Position pos) : StaticObj{ id, pos } {}
private:
public:
	REPLICATE(Hp);
	SET(Hp);
	REPLICATE(Level);
	SET(Level);
protected:
	int Hp_{ -1 };
	int Level_{ -1 };
	int Exp_{ -1 };

private:
};

