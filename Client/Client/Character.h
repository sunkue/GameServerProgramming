#pragma once
#include "Obj.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "ReplicateHelper.h"

string VisualizationId(ID id);

class Character : public DynamicObj
{
	friend class Networker;
public:
	Character(ID id, Position pos = {}) :DynamicObj{ id, pos } {};
	bool ProcessInput(const KeyboardEventManager::KeyEvent& key);
	bool ProcessInput(const MouseEventManager::ScrollEvent& scroll);
	bool ProcessInput(const MouseEventManager::ButtonEvent& button);
	bool ProcessInput(const MouseEventManager::PosEvent& pos);

	REPLICATE(Exp);
	REPLICATE(Money);
	REPLICATE(Name);
	GET_REF(SpeechBubble);
protected:
	SET(SpeechBubble);
	SET(Exp);
	SET(Name);
	SET(Money);
private:
private:
	int Exp_{ -1 };
	int Money_{ -1 };
	string Name_;
	array<ID, MAX_PARTY - 1> PartyCrews_{ -1 };
	pair<string, system_clock::time_point> SpeechBubble_;
};