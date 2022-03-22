#pragma once

class Obj
{
public:
	Obj(glm::vec2 pos = glm::vec2(0)) : pos_{ pos } {};

public:
	bool move(glm::vec2 diff);

public:
	GET(pos);
	SET(pos);

private:
	glm::vec2 pos_;
};
