#pragma once


#include "Defines.h"
#include "enum.h"
#include "PacketZip.h"
//====================================

#pragma warning(push)
#pragma warning(disable: 26812)
#pragma warning(disable: 26495)

BETTER_ENUM
(
	PACKET_TYPE, uint8

	, NONE = 0
	/* Client 2 Server */

	, Cs_none = 10
	, Cs_login
	, Cs_signup
	, Cs_input
	, Cs_input_timestamp
	, Cs_chat
	, Cs_request_name
	, Cs_request_hp
	, Cs_request_money
	, Cs_request_exp
	, Cs_request_level
	, Cs_use_skill
	, Cs_use_item
	, Cs_invite_to_party
	, Cs_leave_party
	, Cs_accept_party_invite
	, Cs_request_AttackPoint
	, Cs_request_ArmorPoint
	, Cs_request_AdditionalHp
	, Cs_request_MovemetSpeed

	/* Server 2 Client */

	, Sc_none = 100
	, Sc_login_result
	, Sc_ready
	, Sc_set_position
	, Sc_set_position_timestamp
	, Sc_remove_obj
	, Sc_set_name
	, Sc_set_hp
	, Sc_set_money
	, Sc_set_exp
	, Sc_set_level
	, Sc_chat
	, Sc_signup_result
	, Sc_use_skill
	, Sc_set_iteminstance_position
	, Sc_remove_iteminstance
	, Sc_sum_item
	, Sc_equip_item
	, Sc_insert_partycrew
	, Sc_erase_partycrew
	, Sc_join_party_request
	, Sc_set_attack_point
	, Sc_set_armor_point
	, Sc_set_additional_hp
	, Sc_set_movement_speed

);

// ������� ��Ŷ 
// FIXED_PACKET // DYNAMIC_PACKET => chat, �� �ε� ��,,,

#pragma pack (push, 1)
//================ BASE ================

template<class T>
struct packet_base
{
	packet_size_t size = sizeof(T);
	PACKET_TYPE packet_type = +PACKET_TYPE::_from_string_nocase(typeid(T).name() + 7);
};
#define PACKET(name) struct name : packet_base<name>										

PACKET(none)
{
};

//============== packet =================

// do not trim '\0'
PACKET(cs_login)
{
	char login_id[MAX_LOGIN_ID_BUFFER_SIZE]{};
	char login_password[MAX_LOGIN_PASSWORD_BUFFER_SIZE]{};
};

// do not trim '\0'
PACKET(cs_signup)
{
	char signup_id[MAX_LOGIN_ID_BUFFER_SIZE]{};
	char signup_password[MAX_LOGIN_PASSWORD_BUFFER_SIZE]{};
};

PACKET(sc_signup_result)
{
	// 'S' success, 'E' exist, 'N' unable char
	char result;
};

PACKET(sc_login_result)
{
	// -1 if no id, -2 if wrong password, -3 if unable char
	NetID id;
};

PACKET(sc_ready)
{
	int level;
	int hp;
	int money;
	int exp;
	char name[MAX_CHARACTER_NAME_BUFFER_SIZE]{};
};

///////////////////////////////////////
// 
//			REQUEST PACKET
// 
///////////////////////////////////////

PACKET(cs_request_Name)
{
	NetID id;
};

PACKET(cs_request_Exp)
{
	NetID id;
};

PACKET(cs_request_Money)
{
	NetID id;
};

PACKET(cs_request_Hp)
{
	NetID id;
};

PACKET(cs_request_Level)
{
	NetID id;
};

PACKET(cs_request_AttackPoint)
{
	NetID id;
};

PACKET(cs_request_ArmorPoint)
{
	NetID id;
};

PACKET(cs_request_AdditionalHp)
{
	NetID id;
};

PACKET(cs_request_MovemetSpeed)
{
	NetID id;
};

///////////////////////////////////////
// 
//				SET PACKET
// 
///////////////////////////////////////

PACKET(sc_set_name)
{
	NetID id;
	char name[MAX_CHARACTER_NAME_BUFFER_SIZE]{};
};

PACKET(sc_set_position)
{
	NetID id;
	Position pos;
};

PACKET(sc_set_hp)
{
	NetID id;
	int hp;
};

PACKET(sc_set_exp)
{
	NetID id;
	int exp;
};

PACKET(sc_set_money)
{
	NetID id;
	int money;
};

PACKET(sc_set_level)
{
	NetID id;
	int level;
};

PACKET(sc_set_attack_point)
{
	NetID id;
	int attackPoint;
};

PACKET(sc_set_armor_point)
{
	NetID id;
	int armorPoint;
};

PACKET(sc_set_additional_hp)
{
	NetID id;
	int additionalHp;
};

PACKET(sc_set_movement_speed)
{
	NetID id;
	float movemetSpeed; // 1/cooltime
};

///////////////////////////////////////

PACKET(sc_set_position_timestamp)
{
	NetID id;
	Position pos;
	milliseconds timestamp;
};

PACKET(sc_remove_obj)
{
	NetID id = -1;
};

PACKET(sc_set_iteminstance_position)
{
	eItemType itemType{ eItemType::hpPotion };
	Position pos;
};

PACKET(sc_remove_iteminstance)
{
	eItemType itemType{ eItemType::hpPotion };
	Position pos;
};

PACKET(cs_input)
{
	eMoveOper input;
};

PACKET(cs_input_timestamp)
{
	eMoveOper input;
	milliseconds timestamp;
};

PACKET(cs_use_skill)
{
	eSkill skill;
};

PACKET(sc_use_skill)
{
	NetID id;
	eSkill skill;
};

PACKET(cs_chat)
{
	char chat[MAX_CHAT_BUFFER_SIZE]{};
};

PACKET(sc_chat)
{
	NetID id;
	system_clock::time_point time;
	char chat[MAX_CHAT_BUFFER_SIZE]{};
};

PACKET(sc_sum_item)
{
	eItemType type;
	int changed;
};

PACKET(cs_use_item)
{
	eItemType type;
};

PACKET(sc_equip_item)
{
	eItemType type;
};

PACKET(cs_invite_to_party)
{
	NetID targetId;
};

PACKET(cs_accept_party_invite)
{
	NetID partyId;
};

PACKET(cs_leave_party)
{
};

PACKET(sc_join_party_request)
{
	NetID inviterId;
	NetID partyId;
};

PACKET(sc_insert_partycrew)
{
	NetID crewId;
};

PACKET(sc_erase_partycrew)
{
	NetID crewId;
};

#pragma pack(pop)
#pragma warning(pop)
