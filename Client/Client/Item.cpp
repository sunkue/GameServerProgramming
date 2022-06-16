#include "stdafx.h"
#include "Item.h"

/*=======================================
*
*				Inventory
*
=======================================*/

eEquimentablePart EquipmentState::Equip(eItemType type)
{
	auto part = ItemTypeDecoder::GetPartOfEquipmentItem(type);
	if (part != eEquimentablePart::_Unable)
		EquimentStates_[static_cast<int>(part)] = type;
	return part;
}

eItemType EquipmentState::GetTypeOfPart(eEquimentablePart part) const
{
	return EquimentStates_[static_cast<int>(part)];
}