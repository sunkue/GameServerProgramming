#pragma once

#include "Obj.h"

class Player : public Obj
{
public:
	Player(glm::vec2 pos = glm::vec2(0)) :Obj{ pos } {};
};

