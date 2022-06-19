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
	friend class Renderer;
public:
	Character(ID id, Position pos = {}) :DynamicObj{ id, pos } { PartyCrews_.fill(-1); };
	bool ProcessInput(const KeyboardEventManager::KeyEvent& key);
	bool ProcessInput(const MouseEventManager::ScrollEvent& scroll);
	bool ProcessInput(const MouseEventManager::ButtonEvent& button);
	bool ProcessInput(const MouseEventManager::PosEvent& pos);

	REPLICATE(Exp);
	REPLICATE(Money);
	REPLICATE(Name);
	REPLICATE(AttackPoint);
	REPLICATE(ArmorPoint);
	REPLICATE(AdditionalHp);
	REPLICATE(MovemetSpeed);
	GET_REF(SpeechBubble);
	GET_REF(Equipment);
	GET_REF_UNSAFE(Inventory);
	GET_REF_UNSAFE(PartyCrews);
protected:
	SET(SpeechBubble);
	SET(Exp);
	SET(Name);
	SET(Money);
	SET(AttackPoint);
	SET(ArmorPoint);
	SET(AdditionalHp);
	SET(MovemetSpeed);
	SET(State);
	eCharacterState GetState()const { return State_.load(); };
protected:
	int MaxHp() { return ::MaxHp(Level_) + GetAdditionalHp(); }
	int AttackPoint_{ -1 };
	int ArmorPoint_{ -1 };
	int AdditionalHp_{ -1 };
	float MovemetSpeed_{ -1 };
private:
	int Exp_{ -1 };
	int Money_{ -1 };
	string Name_;
	array<ID, MAX_PARTY_CREW> PartyCrews_;
	pair<string, system_clock::time_point> SpeechBubble_;
	Inventory Inventory_;
	EquipmentState Equipment_;
	atomic<eCharacterState> State_{ eCharacterState::enable };
};