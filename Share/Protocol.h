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
);

// 가용길이 패킷 
// FIXED_PACKET // DYNAMIC_PACKET => chat, 맵 로딩 등,,,

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
	char result;
};

PACKET(sc_login_result)
{
	// -1 if no id, -2 if wrong password
	NetID id = -1;
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

enum class eMoveOper : uint8
{
	up,
	down,
	right,
	left
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

PACKET(cs_chat)
{
	char chat[MAX_CHAT_BUFFER_SIZE]{};
};

// dynamic size packet
PACKET(sc_chat)
{
	NetID id;
	system_clock::time_point time;
	char chat[MAX_CHAT_BUFFER_SIZE]{};
};

#pragma pack(pop)
#pragma warning(pop)
