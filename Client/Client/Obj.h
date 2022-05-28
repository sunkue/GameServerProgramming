#pragma once

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
	StaticObj(Position pos) : Pos_{ pos } {}
public:
	GET(Pos);
	SET(Pos);
protected:
	Position Pos_{};
};


inline int RequireExp(int HP) { return static_cast<int>(50 * pow(2, HP)); }
inline int MaxHp(int Level) { return Level * 100; }

class DynamicObj : public StaticObj
{
public:
	DynamicObj(Position pos) : StaticObj{ pos } {}
private:
public:
	GET(Hp);
	SET(Hp);
	GET(Level);
	SET(Level);
protected:
	int Hp_{};
	int Level_{};
	int Exp_{};

private:
};

