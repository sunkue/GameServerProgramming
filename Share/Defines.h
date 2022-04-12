#pragma once

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
using packet_size_t = uint8;

//====================================

const auto SERVER_IP = "127.0.0.1";
const uint16_t SERVER_PORT = 8282;

//====================================
const int MAX_PLAYER = 10;
static_assert(MAX_PLAYER < std::numeric_limits<ID>::max(), "Player ID could overflow");
const int MAX_PACKET_SIZE = std::numeric_limits<packet_size_t>::max();
const int MAX_BUFFER_SIZE = MAX_PACKET_SIZE + 1;
static_assert(MAX_PACKET_SIZE <= MAX_BUFFER_SIZE, "Net Buffer could overflow");