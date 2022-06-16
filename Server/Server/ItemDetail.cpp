#include "stdafx.h"
#include "Item.h"
#include "TimerEvent.h"
#include "Character.h"
#include "Player.h"

/*=======================================
*
*	 ItemBase::InitItemEffectTable
*
=======================================*/

// Dont get singleton object in this func. could be infinity loop.
void ItemBase::InitItemEffectTable()
{
	ItemEffectTable[eItemType::hpPotion] = [](ID agent)
	{
		auto& characters = CharacterManager::Get().GetCharacters();
		characters[agent]->HpIncrease(agent, 150);
	};

	ItemEffectTable[eItemType::expPotion] = [](ID agent)
	{
		auto& characters = CharacterManager::Get().GetCharacters();
		if (auto p = dynamic_cast<Player*>(characters[agent].get()))
		{
			p->ExpSum(agent, 200);
		}
	};

	{
		auto b = static_cast<int>(eItemType::_EquipmentItemStartLine);
		auto e = static_cast<int>(eItemType::_EquipmentItemEndLine);
		for (auto i = b; i != e; i++) {
			auto type = static_cast<eItemType>(i);
			ItemEffectTable[type] = [type](ID agent)
			{
				auto& characters = CharacterManager::Get().GetCharacters();
				if (auto p = dynamic_cast<Player*>(characters[agent].get()))
				{
					p->Equip(type);
				}
			};
		}
	}
}

void ItemBase::InitItemLevelTable()
{
	ItemLevelTable[eItemType::stoneSword] = 1;
	ItemLevelTable[eItemType::ironSword] = 2;
	ItemLevelTable[eItemType::goldSword] = 3;
	ItemLevelTable[eItemType::diamondSword] = 4;

	ItemLevelTable[eItemType::ironHelmet] = 1;
	ItemLevelTable[eItemType::steelHelmet] = 2;

	ItemLevelTable[eItemType::leatherArmor] = 1;
	ItemLevelTable[eItemType::steelArmor] = 2;
	ItemLevelTable[eItemType::goldArmor] = 5;

	ItemLevelTable[eItemType::robberBoots] = 1;
	ItemLevelTable[eItemType::wingShoes] = 4;
}

/*=======================================
*
*			ItemBase::CreateItem
*
=======================================*/

unique_ptr<ItemBase> ItemBase::CreateItem(ItemBase base)
{
	auto type = base.GetType();
	auto num = base.GetNum();

	if (eItemType::_ConsumableItemStartLine < type && type < eItemType::_ConsumableItemEndLine)
	{
		return make_unique<ConsumableItem>(base);
	}
	if (eItemType::_EquipmentItemStartLine < type && type < eItemType::_EquipmentItemEndLine)
	{
		return make_unique<EquipmentItem>(base);
	}
	cerr << "[ERR] ItemBase::CreateItem" << endl;
	return make_unique<ItemBase>(base);
}