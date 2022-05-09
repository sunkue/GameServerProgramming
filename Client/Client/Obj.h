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
	StaticObj(OBJ_TYPE type, Position pos) : type_{ type }, pos_{ pos } {}
public:
	GET(pos);
	SET(pos);
	GET(type);
protected:
	OBJ_TYPE type_{};
	Position pos_{};
};

class DynamicObj : public StaticObj
{
public:
	DynamicObj(OBJ_TYPE type, Position pos) : StaticObj{ type ,pos } {}
private:
public:
protected:
	shared_mutex lock;
private:
};

