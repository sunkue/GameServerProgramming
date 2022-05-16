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
	StaticObj(OBJ_TYPE type, Position pos) : Type_{ type }, Pos_{ pos } {}
public:
	GET(Pos);
	SET(Pos);
	GET(Type);
protected:
	OBJ_TYPE Type_{};
	Position Pos_{};
};


inline int RequireExp(int HP) { return 50 * pow(2, HP); }
inline int MaxHP(int Level) { return Level * 100; }

class DynamicObj : public StaticObj
{
public:
	DynamicObj(OBJ_TYPE type, Position pos) : StaticObj{ type ,pos } {}
private:
public:
	GET(Hp);
	SET(Hp);
	GET(Level);
	SET(Level);
protected:
	int Hp_{};
	int Level_{};

private:
};

