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
	GET(name);
	SET(name);
	
private:
	glm::vec2 pos_;
	string name_;

public:
	shared_mutex m{}; // MyLock 가져와서 쓰자.
};
