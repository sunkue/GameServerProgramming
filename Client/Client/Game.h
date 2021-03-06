#pragma once

#include "Obj.h"
#include "Character.h"

template <class _Kty, class _Ty>
class UnorderedMapConstructWithKey : public unordered_map<_Kty, _Ty>
{
public:
	_Ty& operator[](_Kty _Keyval) {
		return this->_Try_emplace(_Keyval, _Keyval).first->_Myval.second;
	}
};

class Game
{
	SINGLE_TON(Game) { Characters_.reserve(MAX_PLAYER); }
public:
	void Update();
	void Init();
	GET(Id); SET(Id);
	GET_REF_UNSAFE(Characters);
	GET_REF_UNSAFE(ItemInstances);
	Character& GetPlayer() { return Characters_[Id_]; }
	bool IsInSight(Position target)
	{
		auto diff = target - GetPlayer().GetPos();
		auto maxCompDiff = glm::compMax(glm::abs(diff));
		return maxCompDiff <= SIGHT_SIZE / 2;
	}
private:
	void GarbageCollect();
private:
	ID Id_{};
	UnorderedMapConstructWithKey<ID, Character> Characters_;
	unordered_set<ItemInstance> ItemInstances_;
};
