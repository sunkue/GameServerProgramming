#pragma once
#include "Obj.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "ReplicateHelper.h"
#include "Item.h"

string VisualizationId(ID id);

enum class eCharacterState
{
	enable,
	disable,
	garbage,
};

class Character : public DynamicObj
{
	friend class Networker;
	friend class Game;
	friend class GameGuiManager;
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
	GET_REF(Equipment);
	GET_REF_UNSAFE(Inventory);
protected:
	SET(SpeechBubble);
	SET(Exp);
	SET(Name);
	SET(Money);
	GET(State);
	SET(State);
protected:
	int MaxHp() const { return ::MaxHp(Level_) + AdditionalHp_; }
	int ArmorPoint_{};
	int AttackPoint_{};
	int AdditionalHp_{};
private:
	int Exp_{ -1 };
	int Money_{ -1 };
	string Name_;
	array<ID, MAX_PARTY - 1> PartyCrews_{ -1 };
	pair<string, system_clock::time_point> SpeechBubble_;
	Inventory Inventory_;
	EquipmentState Equipment_;
	eCharacterState State_{ eCharacterState::enable };
};