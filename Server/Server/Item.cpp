#include "stdafx.h"
#include "Item.h"
#include "TimerEvent.h"

/*=======================================
*
*				ItemBase
*
=======================================*/

pair<bool, int> ItemBase::TryUse(ID agent)
{
	if (Num_ <= 0) return make_pair(false, 0);
	Num_--;
	return make_pair(true, 1);
}

/*=======================================
*
*				Inventory
*
=======================================*/

void Inventory::CreateItem(ItemBase item)
{
	shared_lock lck{ ItemLock };
	auto it = Items_.find(item.GetType());
	if (it == Items_.end())
		Items_[item.GetType()] = (move(ItemBase::CreateItem(item)));
	else
		it->second->SumNum(item.GetNum());
}

void Inventory::AddItem(unique_ptr<ItemBase>&& item)
{
	shared_lock lck{ ItemLock };
	auto it = Items_.find(item->GetType());
	if (it == Items_.end())
	{
		Items_[item->GetType()] = move(item);
	}
	else
	{
		it->second->SumNum(item->GetNum());
	}
}

int Inventory::UseItem(ID agent, eItemType type, int num)
{
	int used = 0;
	shared_lock lck{ ItemLock };
	if (Items_.find(type) == Items_.end())
		return used;

	for (int i = 0; i < num; i++)
	{
		auto res = Items_[type]->TryUse(agent);
		if (res.first)
			used += res.second;
	}

	return used;
}

/*=======================================
*
*				ConsumableItem
*
=======================================*/

pair<bool, int> ConsumableItem::TryUse(ID agent)
{
	auto res = ItemBase::TryUse(agent);
	if (!res.first) return res;
	ItemEffectTable[GetType()](agent);
	return res;
}

/*=======================================
*
*				EquipmentItem
*
=======================================*/

pair<bool, int> EquipmentItem::TryUse(ID agent)
{
	if (Num_ <= 0) return make_pair(false, 0);
	ItemEffectTable[GetType()](agent);
	return make_pair(true, 0);
}



/*=======================================
*
*				EquipmentState
*
=======================================*/

eEquimentablePart EquipmentState::Equip(eItemType type)
{
	auto part = ItemTypeDecoder::GetPartOfEquipmentItem(type);
	if (part != eEquimentablePart::_Unable)
		EquimentStates_[static_cast<int>(part)] = type;
	return part;
}

int EquipmentState::GetLevelOfPart(eEquimentablePart part) const
{
	return ItemBase::GetItemLevel(EquimentStates_[static_cast<int>(part)]);
}

eItemType EquipmentState::GetTypeOfPart(eEquimentablePart part) const
{
	return EquimentStates_[static_cast<int>(part)];
}