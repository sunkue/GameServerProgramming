#pragma once

#include "Obj.h"

/////////////////////////////////
// 
//			Monster
// 
/////////////////////////////////

class Monster : public DynamicObj
{
public:
	virtual ~Monster() = default;
	virtual bool Move(Position diff) override;
	virtual void Update() override;
	virtual bool Enable() override;
	virtual bool Disable() override;
private:
public:
private:
};

