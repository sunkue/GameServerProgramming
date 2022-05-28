#pragma once

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
using packet_size_t = uint8; //  need to be unsigned
using Position = glm::ivec2;
//====================================

const uint16_t SERVER_PORT = 8282;

//====================================
constexpr int MAX_PLAYER = 5'0000;
constexpr int MAX_MONSTER = 20'0000;
constexpr int MAX_NPC = 10;
constexpr int MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER + MAX_NPC;

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

#undef max
#undef min

static_assert(MAX_PLAYER < std::numeric_limits<ID>::max(), "Player ID could overflow");
constexpr int MAX_PACKET_SIZE = std::numeric_limits<packet_size_t>::max();
constexpr int MAX_BUFFER_SIZE = MAX_PACKET_SIZE * 4;
constexpr int MAX_CHAT_SIZE = 50;
constexpr int MAX_CHAT_BUFFER_SIZE = MAX_CHAT_SIZE + 1;
static_assert(MAX_PACKET_SIZE <= MAX_BUFFER_SIZE, "Net Buffer could overflow");