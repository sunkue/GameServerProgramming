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

class Obj
{
public:
	Obj(OBJ_TYPE type, glm::vec2 pos = glm::vec2(0)) :type_{ type }, pos_{ pos } {};

public:
	SET(pos);
	GET(pos);
	GET(type);

private:
	glm::vec2 pos_;
	OBJ_TYPE type_;
};

