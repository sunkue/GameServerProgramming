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
	, Cs_hi
	, Cs_input
	, Cs_chat


	/* Server 2 Client */

	, Sc_none = 100
	, Sc_hi
	, Sc_ready
	, Sc_set_position
	, Sc_remove_obj
	, Sc_set_hp
	, Sc_chat
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

PACKET(cs_hi)
{
};

PACKET(sc_hi)
{
	NetID id = -1;
};

PACKET(sc_ready)
{

};

PACKET(sc_set_position)
{
	NetID id;
	Position pos;
	milliseconds timestamp;
};

PACKET(sc_set_hp)
{
	NetID id;
	int hp;
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
	milliseconds timestamp;
};

PACKET(cs_chat)
{
	char chat[MAX_CHAT_SIZE]{};
};


// dynamic size packet
PACKET(sc_chat)
{
	NetID id;
	system_clock::time_point time;
	char chat[MAX_CHAT_SIZE]{}; // should be last member
};

#pragma pack(pop)
#pragma warning(pop)
