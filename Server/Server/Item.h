#pragma once

/*=======================================
*
*				ItemBase
*
=======================================*/

class ItemBase
{
	friend class ConsumableItem;
	friend class EquipmentItem;
protected:
	inline static unordered_map<eItemType, function<void(ID)>> ItemEffectTable;
	inline static unordered_map<eItemType, int> ItemLevelTable;
private:
	static void InitItemEffectTable();
	static void InitItemLevelTable();
public:
	inline static int GetItemLevel(eItemType t) { return ItemLevelTable[t]; }
	static bool InitOnce;
public:
	ItemBase(eItemType t, int n = 1) :Type_{ t }, Num_{ n }{}
	ItemBase(const ItemBase& o) :Type_{ o.Type_ }, Num_{ o.Num_.load() } {}
	static unique_ptr<ItemBase> CreateItem(ItemBase);
	virtual ~ItemBase() = default;
	virtual pair<bool, int> TryUse(ID agent);
	bool operator==(const ItemBase& other)const { return other.Type_ == Type_; }
private:
public:
	GET(Type);
	int GetNum()const { return Num_.load(); }
	void SumNum(int n)const { Num_ += n; }
private:
	eItemType Type_{ eItemType::hpPotion };
	mutable atomic_int Num_{};
};

// 알아서 단 한 번 초기화 해 줄 것이야.
inline bool ItemBase::InitOnce = []() { InitItemEffectTable(); InitItemLevelTable(); return true; } ();

/*=======================================
*
*				ItemInstance
*
=======================================*/

// 맵에 떨궈진 아이템 (섹터에서 관리)
class ItemInstance
{
public:
	ItemInstance(ItemBase base, Position pos) :OriginalItem_{ base }, Pos_{ pos }{}
	ItemInstance(ItemInstance&&) = default;
	ItemInstance(const ItemInstance&) = default;
	bool operator==(const ItemInstance& other)const { return other.OriginalItem_ == OriginalItem_ && other.Pos_ == Pos_; }
	GET_REF(OriginalItem);
	GET(Pos);
private:
	mutable ItemBase OriginalItem_;
	Position Pos_;
};

namespace std
{
	template<> struct hash<ItemInstance>
	{
		size_t operator()(const ItemInstance& _Keyval) const
		{
			return std::hash<int>()(_Keyval.GetPos().x);
		}
	};
}

/*=======================================
*
*				ConsumableItem
*
=======================================*/

class ConsumableItem : public ItemBase
{
public:
	static unordered_map<eItemType, milliseconds> CoolTimeTable;
	ConsumableItem(ItemBase base) :ItemBase(base) {};
	virtual ~ConsumableItem() = default;
public:
	virtual pair<bool, int> TryUse(ID agent) override;
private:
public:
	milliseconds GetCoolTime() const { return CoolTimeTable[GetType()]; };
private:
	atomic_bool Useable_{ true };
};

/*=======================================
*
*				EquipmentItem
*
=======================================*/


class EquipmentItem : public ItemBase
{
public:
	EquipmentItem(ItemBase base) :ItemBase(base) {};
	virtual ~EquipmentItem() = default;
public:
	virtual pair<bool, int> TryUse(ID agent) override;
private:
public:
private:
};

/*=======================================
*
*				Inventory
*
=======================================*/

class Inventory
{
public:
	void CreateItem(ItemBase);
	void AddItem(unique_ptr<ItemBase>&&);
	int UseItem(ID agent, eItemType type, int num);
	void Clear() { unique_lock lck{ ItemLock }; Items_.clear(); }
private:
public:
	GET_REF(Items);
	shared_mutex ItemLock;
private:
	concurrent_unordered_map<eItemType, unique_ptr<ItemBase>> Items_;
};

/*=======================================
*
*				EquipmentState
*
=======================================*/

class EquipmentState
{
public:
	eEquimentablePart Equip(eItemType type);
	int GetLevelOfPart(eEquimentablePart part) const;
	eItemType GetTypeOfPart(eEquimentablePart part) const;
private:
	shared_mutex EquimentStatesLock_;
	array<atomic<eItemType>, static_cast<int>(eEquimentablePart::_Count)> EquimentStates_{ eItemType::_None };
};





