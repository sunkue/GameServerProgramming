#pragma once

#include "Character.h"
#include "Item.h"

/////////////////////////////////
// 
//			Player
// 
/////////////////////////////////


class Player : public Character
{
	friend CharacterManager;
public:
	Player(ID id) : Character{ id } {};
	virtual ~Player() = default;
	virtual bool Move(Position diff) override;
	virtual bool Enable() override;
	virtual bool Disable() override;
	virtual void Update() override;
	virtual void Regen() override;
	void UpdateViewList();
	void ActivateSkill(eSkill skill);
	bool EraseFromViewList(ID);
	bool InsertToViewList(ID);
	GET_REF(Exp);
	// amount could be minus. ex) die-regen
	void ExpSum(ID agent, int amount, bool shareWithParty = false);
	virtual void HpDecrease(ID agent, int amount) override;
	virtual void HpIncrease(ID agent, int amount) override;
	virtual int UseItem(eItemType itemType, int num = 1) override;
	int MaxHp() const { return ::MaxHp(Level_) + AdditionalHp_; }
	int AttackPoint() const { return ::DefaultAttack(Level_) + AttackPoint_; }
	GET(AttackPoint);
	GET(ArmorPoint);
	GET(AdditionalHp);
	GET(DbId);
	GET(Name);
	GET_REF(Money);
	bool MoneySum(ID agent, int amount);
	bool Equip(eItemType itemType);
protected:
	virtual bool MoveForce(Position diff) override;
	SET(DbId);
	SET(Name);
	SET(Money);
	SET(Exp);
private:
	void Attack(eSkill animateSkill = eSkill::attack, function<bool(Position)> isInrange = [](Position diff)->bool { return glm::length(glm::vec2{ diff }) <= 1; }
	, float power = 1);
	void HpRegen();
public:
	int GetPartyId()const { return PartyId_.load(); }
	void SetPartyId(ID id) { PartyId_ = id; }
protected:
	shared_mutex ViewLock;
private:
	int ArmorPoint_{};
	int AttackPoint_{};
	int AdditionalHp_{};
	EquipmentState EquimentState_;
private:
	atomic_int Exp_{};
	atomic_int Money_{};
	concurrent_unordered_set<ID> ViewList_;
	unordered_map<eSkill, atomic_bool> IsSkillOnCooling_;
	DbCharacterID DbId_{ -1 };
	wstring Name_;
	atomic<ID> PartyId_{ -1 };
};
