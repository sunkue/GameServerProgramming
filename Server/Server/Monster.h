#pragma once

#include "Character.h"

/////////////////////////////////
// 
//			Monster
// 
/////////////////////////////////

enum class eMonsterAggressionType
{
	Peace,
	Agro
};

enum class eMonsterMovementType
{
	Fixed,
	Roaming
};

// 다형성을 스크립트로 구현해보자.
class Monster : public Character
{
public:
	Monster(ID id);
	virtual ~Monster();
	virtual bool Move(Position diff) override;
	virtual void Update() override;
	virtual bool Enable() override;
	virtual bool Disable() override;
	virtual void CompileScript() override;
	virtual void Regen() override;
	virtual void HpDecrease(ID agent, int amount) override;
	virtual void HpIncrease(ID agent, int amount) override;
	void Die(ID agent); 
private:
public:
protected:
private:
	eMonsterAggressionType AggressionType_;
	eMonsterMovementType MovementType_;
	atomic_bool Died_{ false };
};

