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
using packet_size_t = uint8; //  need to be unsigned

//====================================

const uint16_t SERVER_PORT = 8282;

//====================================
constexpr int MAX_PLAYER = 5000 * 2;
constexpr size_t MAP_SIZE = 2000;
constexpr int SIGHT_SIZE = 15;
constexpr int WINDOW_SIZE = 20;
static constexpr size_t SECTOR_SIZE = 100;
static constexpr size_t SECTOR_NUM = MAP_SIZE / SECTOR_SIZE;

#undef max
#undef min

static_assert(MAX_PLAYER < std::numeric_limits<ID>::max(), "Player ID could overflow");
constexpr int MAX_PACKET_SIZE = std::numeric_limits<packet_size_t>::max();
constexpr int MAX_BUFFER_SIZE = MAX_PACKET_SIZE * 4;
static_assert(MAX_PACKET_SIZE <= MAX_BUFFER_SIZE, "Net Buffer could overflow");