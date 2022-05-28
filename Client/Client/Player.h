#pragma once
#include "Obj.h"
#include "KeyboardEvent.h"

class Character : public DynamicObj
{
public:
	Character(Position pos = {}) :DynamicObj{ pos } {};
	bool ProcessInput(const KEY_BOARD_EVENT_MANAGER::key_event& key);
	GET(Exp);
	SET(Exp);
	SET(SpeechBubble);
	GET_REF(SpeechBubble);
protected:
	int Exp_{};
private:
	array<ID, MAX_PARTY - 1> PartyCrews_{ -1 };
	pair<string, system_clock::time_point> SpeechBubble_;
};