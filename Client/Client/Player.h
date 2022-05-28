#pragma once
#include "Obj.h"
#include "KeyboardEvent.h"

class Player : public DynamicObj
{
public:
	Player(Position pos = {}) :DynamicObj{ pos } {};
	bool ProcessInput(const KEY_BOARD_EVENT_MANAGER::key_event& key);
	GET(Exp);
protected:
	int Exp_{};
private:
	array<ID, MAX_PARTY - 1> PartyCrews_{ -1 };
};