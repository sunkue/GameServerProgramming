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

	, CS_NONE = 10
	, CS_HI
	, CS_INPUT


	/* Server 2 Client */

	, SC_NONE = 100
	, SC_HI
	, SC_SET_POSITION

);



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
	uint8 n;
};

PACKET(sc_set_position)
{
	Net::nt<uint8, 2> pos; 
};

enum class move_oper : uint8
{
	none	= 0,
	up		= 1,
	down	= 2,
	right	= 3,
	left	= 4,
};

PACKET(cs_input)
{
	move_oper input;
};

#pragma pack(pop)
#pragma warning(pop)
