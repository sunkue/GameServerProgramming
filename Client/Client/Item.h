#pragma once


// 아이템 상세 정보는 서버에서만 존재..

/*=======================================
*
*				ItemInstance
*
=======================================*/

struct ItemInstance
{
	eItemType Type{ eItemType::hpPotion };
	Position Pos;
	bool operator==(const ItemInstance& other)const { return Type == other.Type && Pos == other.Pos; }
};

namespace std
{
	template<> struct hash<ItemInstance>
	{
		size_t operator()(const ItemInstance& _Keyval) const
		{
			return std::hash<int>()(_Keyval.Pos.x);
		}
	};
}

/*=======================================
*
*				Inventory
*
=======================================*/

class Inventory
{
public:
private:
public:
	GET_REF_UNSAFE(Items);
private:
	concurrent_unordered_map<eItemType, int> Items_; // 0개는 없는 취급
};


/*=======================================
*
*				Inventory
*
=======================================*/

class EquipmentState
{
	friend class GameGuiManager;
public:
	eEquimentablePart Equip(eItemType type);
	eItemType GetTypeOfPart(eEquimentablePart part) const;
protected:
	GET_REF(EquimentStates);
private:
	array<eItemType, static_cast<int>(eEquimentablePart::_Count)> EquimentStates_{ eItemType::_None };
};
