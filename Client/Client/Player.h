#pragma once
#include "Obj.h"
#include "KeyboardEvent.h"

class Character : public DynamicObj
{
	friend class Networker;
public:
	Character(Position pos = {}) :DynamicObj{ pos } {};
	bool ProcessInput(const KEY_BOARD_EVENT_MANAGER::key_event& key);
	GET(Exp);
	GET(Name);
	GET_REF(SpeechBubble);
	GET(Money);
protected:
	SET(SpeechBubble);
	SET(Exp);
	SET(Name);
	SET(Money);
private:
	int Exp_{};
	int Money_{};
	string Name_;
	array<ID, MAX_PARTY - 1> PartyCrews_{ -1 };
	pair<string, system_clock::time_point> SpeechBubble_;
};