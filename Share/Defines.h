#pragma once

#undef max
#undef min

#define RINGBUFFER
//#define GQCPEX

//====================================

using BYTE = uint8_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = DWORD;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

//====================================

using ID = int;
using NetID = ID; // nt<type,1> :: ID
using DbCharacterID = ID; // nt<type,1> :: ID
//using CharacterID = _ID; // nt<type,1> :: ID
using packet_size_t = uint8; //  need to be unsigned
using Position = glm::ivec2;
//====================================

const uint16_t SERVER_PORT = 4000;

//====================================
constexpr int MAX_PARTY = 4;
constexpr int MAP_SIZE = 2000;
constexpr int SIGHT_SIZE = 15;
constexpr int WINDOW_SIZE = 20;
constexpr int SECTOR_SIZE = 50;
constexpr int SECTOR_NUM = MAP_SIZE / SECTOR_SIZE;

static_assert(SIGHT_SIZE % 2);
static_assert(SIGHT_SIZE < SECTOR_SIZE);
static_assert(0 == MAP_SIZE % 2);
static_assert(0 == SECTOR_SIZE % 2);
static_assert(0 == WINDOW_SIZE % 2);
static_assert(0 == MAP_SIZE % SECTOR_SIZE);

constexpr int MAX_PLAYER = 1'0000;
//constexpr int MAX_MONSTER = 20'0000;
//constexpr int MAX_MONSTER = 10'0000;
constexpr int MAX_MONSTER = 10'0;
constexpr int MAX_NPC = 10;
constexpr int MAX_OBSTACLE_PER_SECTOR = 10000;
constexpr int MAX_OBSTACLE = MAX_OBSTACLE_PER_SECTOR * SECTOR_NUM;
constexpr int MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER + MAX_NPC;
constexpr int MAX_OBJECT = MAX_CHARACTER + MAX_OBSTACLE;

static_assert(MAX_PLAYER < std::numeric_limits<ID>::max(), "Player ID could overflow");
constexpr int MAX_PACKET_SIZE = std::numeric_limits<packet_size_t>::max();
constexpr int MAX_BUFFER_SIZE = MAX_PACKET_SIZE * 4;
constexpr int MAX_CHAT_SIZE = 50;
constexpr int MAX_CHAT_BUFFER_SIZE = MAX_CHAT_SIZE + 1;
constexpr int MAX_CHARACTER_NAME_SIZE = 10 * (sizeof(wchar_t) / sizeof(char));
constexpr int MAX_CHARACTER_NAME_BUFFER_SIZE = MAX_CHARACTER_NAME_SIZE + 1;
constexpr int MAX_LOGIN_ID_SIZE = 10 * (sizeof(wchar_t) / sizeof(char));
constexpr int MAX_LOGIN_ID_BUFFER_SIZE = MAX_LOGIN_ID_SIZE + 1;
constexpr int MAX_LOGIN_PASSWORD_SIZE = 10 * (sizeof(wchar_t) / sizeof(char));
constexpr int MAX_LOGIN_PASSWORD_BUFFER_SIZE = MAX_LOGIN_PASSWORD_SIZE + 1;
static_assert(MAX_PACKET_SIZE <= MAX_BUFFER_SIZE, "Net Buffer could overflow");





constexpr ID SYSTEM_ID = -10;

#include "enum.h"

/*=======================================
*
*				eItemType
*
=======================================*/

enum class eItemType : ID
{
	_None,
	_ConsumableItemStartLine,

	hpPotion,
	expPotion,

	_ConsumableItemEndLine,
	_EquipmentItemStartLine,
	_WeaponStartLine,

	stoneSword,
	ironSword,
	goldSword,
	diamondSword,

	_WeaponEndLine,
	_HeadStartLine,

	ironHelmet,
	steelHelmet,

	_HeadEndLine,
	_BodyStartLine,

	leatherArmor,
	steelArmor,
	goldArmor,

	_BodyEndLine,
	_ShoesStartLine,

	robberBoots,
	wingShoes,

	_ShoesEndLine,
	_EquipmentItemEndLine,
};

enum class eEquimentablePart : int
{
	_Unable = -1,
	head,
	body,
	shoes,
	weapon,
	_Count
};

#include <unordered_map>

class ItemTypeDecoder
{
private:
	static bool InitOnce;
	inline static unordered_map<string, eItemType> StringToTypeTable;
	inline static unordered_map<eItemType, string> TypeToStringTable;
	inline static void Init(eItemType type, string name)
	{
		StringToTypeTable[name] = type;
		TypeToStringTable[type] = name;
	}
public:
	inline static eItemType toType(string name)
	{
		return StringToTypeTable[name];
	}

	inline static string toString(eItemType type)
	{
		return TypeToStringTable[type];
	}

	inline static eEquimentablePart GetPartOfEquipmentItem(eItemType type)
	{
		if (eItemType::_WeaponStartLine < type && type < eItemType::_WeaponEndLine)
			return eEquimentablePart::weapon;
		else if (eItemType::_HeadStartLine < type && type < eItemType::_HeadEndLine)
			return eEquimentablePart::head;
		else if (eItemType::_BodyStartLine < type && type < eItemType::_BodyEndLine)
			return eEquimentablePart::body;
		else if (eItemType::_ShoesStartLine < type && type < eItemType::_ShoesEndLine)
			return eEquimentablePart::shoes;
		return eEquimentablePart::_Unable;
	}
};

bool ItemTypeDecoder::InitOnce = []()
{
	Init(eItemType::hpPotion, "HpPotion");
	Init(eItemType::expPotion, "ExpPotion");

	Init(eItemType::stoneSword, "StoneSword");
	Init(eItemType::ironSword, "IronSword");
	Init(eItemType::goldSword, "GoldSword");
	Init(eItemType::diamondSword, "DiamondSword");

	Init(eItemType::ironHelmet, "IronHelmet");
	Init(eItemType::steelHelmet, "SteelHelmet");

	Init(eItemType::leatherArmor, "LeatherArmor");
	Init(eItemType::steelArmor, "SteelArmor");
	Init(eItemType::goldArmor, "GoldArmor");

	Init(eItemType::robberBoots, "RobberBoots");
	Init(eItemType::wingShoes, "WingShoes");

	return true;
}();




/*=======================================
*
*				eSkill
*
=======================================*/

enum class eSkill : uint8
{
	accquireItem,
	attack,
	heal,
	haste,
	set_teleport,
	teleport,
};

/*=======================================
*
*				eMoveOper
*
=======================================*/

enum class eMoveOper : uint8
{
	up,
	down,
	right,
	left
};

