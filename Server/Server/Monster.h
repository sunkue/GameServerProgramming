#pragma once

#include "Character.h"

/////////////////////////////////
// 
//			Monster
// 
/////////////////////////////////

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
private:
public:
protected:
private:
};

