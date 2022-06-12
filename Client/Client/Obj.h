#pragma once

#include "ReplicateHelper.h"


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


inline int RequireExp(int Level) { return static_cast<int>(50 * pow(2, Level)); }
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

