#pragma once

#include "Common.h"

#include <iostream>

// This is the ZoneID enum containing the World IDs
enum ZoneId : ushort { 
	NO_ZONE = 0,
	VENTURE_EXPLORER = 1000, VENTURE_EXPLORER_RETURN = 1001,
	AVANT_GARDENS = 1100, AVANT_GARDENS_SURVIVAL = 1101, SPIDER_QUEEN_BATTLE = 1102, BLOCK_YARD = 1150, AVANT_GROVE = 1151,
	NIMBUS_STATION = 1200, PET_COVE = 1201, VERTIGO_LOOP_RACETRACK = 1203, BATTLE_OF_NIMBUS_STATION = 1204, NIMBUS_ROCK = 1250, NIMBUS_ISLE = 1251,
	GNARLED_FOREST = 1300, CANYON_COVE = 1302, KEELHAUL_CANYON = 1303, CHANTEY_SHANTEY = 1350,
	FORBIDDEN_VALLEY = 1400, FORBIDDEN_VALLEY_DRAGON = 1402, DRAGONMAW_CHASM = 1403, RAVEN_BLUFF = 1450,
	STARBASE_3001 = 1600, DEEP_FREEZE = 1601, ROBOT_CITY = 1602, MOON_BASE = 1603, PORTABELLO = 1604,
	LEGO_CLUB = 1700,
	CRUX_PRIME = 1800,
	NEXUS_TOWER = 1900,
	NINJAGO_MONASTERY = 2000, FRAKJAW_BATTLE = 2001
};

// Prints World/Zone name
std::ostream& operator<<(std::ostream& out, const ZoneId value);
std::string& operator+(std::string& str, const ZoneId value);

// This is the World struct which stores a ZoneId, a checksum in single bytes and a boolean which indicates if the world is a battle
struct World
{
	uchar checksum[4]; // Array to store the checksum bytes of the specific world for you to loop through
	ushort zoneId;
	bool isBattle = false;

	World(ZoneId _zoneId)
	{
		zoneId = _zoneId;

		switch (_zoneId) // This adds the bytes to the checksum array when the reference is created
		{

		case VENTURE_EXPLORER:
			checksum[0] = 0x7c;
			checksum[1] = 0x08;
			checksum[2] = 0xb8;
			checksum[3] = 0x20;
			break;

		case VENTURE_EXPLORER_RETURN:
			checksum[0] = 0x3c;
			checksum[1] = 0x0a;
			checksum[2] = 0x68;
			checksum[3] = 0x26;
			break;

		case AVANT_GARDENS:
			checksum[0] = 0x11;
			checksum[1] = 0x55;
			checksum[2] = 0x52;
			checksum[3] = 0x49;
			break;

		case AVANT_GARDENS_SURVIVAL:
			checksum[0] = 0xe2;
			checksum[1] = 0x14;
			checksum[2] = 0x82;
			checksum[3] = 0x53;
			break;

		case SPIDER_QUEEN_BATTLE:
			isBattle = true;

		case BLOCK_YARD:
			checksum[0] = 0xda;
			checksum[1] = 0x03;
			checksum[2] = 0xd4;
			checksum[3] = 0x0f;
			break;

		case AVANT_GROVE:
			checksum[0] = 0x03;
			checksum[1] = 0x03;
			checksum[2] = 0x89;
			checksum[3] = 0x0a;
			break;

		case NIMBUS_STATION:
			checksum[0] = 0x30;
			checksum[1] = 0x6b;
			checksum[2] = 0x1e;
			checksum[3] = 0xda;
			break;

		case PET_COVE:
			checksum[0] = 0x30;
			checksum[1] = 0x13;
			checksum[2] = 0x6e;
			checksum[3] = 0x47;
			break;

		case VERTIGO_LOOP_RACETRACK:
			checksum[0] = 0x02;
			checksum[1] = 0x05;
			checksum[2] = 0xfc;
			checksum[3] = 0x10;
			break;

		case BATTLE_OF_NIMBUS_STATION:
			checksum[0] = 0x58;
			checksum[1] = 0x02;
			checksum[2] = 0xd4;
			checksum[3] = 0x07;
			isBattle = true;
			break;

		case NIMBUS_ROCK:
			checksum[0] = 0x91;
			checksum[1] = 0x01;
			checksum[2] = 0x8d;
			checksum[3] = 0x05;
			break;

		case NIMBUS_ISLE:
			checksum[0] = 0x5d;
			checksum[1] = 0x04;
			checksum[2] = 0x4f;
			checksum[3] = 0x05;
			break;

		case GNARLED_FOREST:
			checksum[0] = 0x90;
			checksum[1] = 0xc2;
			checksum[2] = 0xea;
			checksum[3] = 0x12;
			break;

		case CANYON_COVE:
			checksum[0] = 0xef;
			checksum[1] = 0x02;
			checksum[2] = 0x77;
			checksum[3] = 0x0b;
			break;

		case CHANTEY_SHANTEY:
			checksum[0] = 0x5c;
			checksum[1] = 0x01;
			checksum[2] = 0xb6;
			checksum[3] = 0x04;
			break;

		case FORBIDDEN_VALLEY:
			checksum[0] = 0x0d;
			checksum[1] = 0x76;
			checksum[2] = 0x19;
			checksum[3] = 0x85;
			break;

		case FORBIDDEN_VALLEY_DRAGON:
			checksum[0] = 0x87;
			checksum[1] = 0x01;
			checksum[2] = 0xf5;
			checksum[3] = 0x02;
			break;

		case DRAGONMAW_CHASM:
			checksum[0] = 0x4e;
			checksum[1] = 0x0f;
			checksum[2] = 0x85;
			checksum[3] = 0x81;
			break;

		case RAVEN_BLUFF:
			checksum[0] = 0x26;
			checksum[1] = 0x01;
			checksum[2] = 0xf0;
			checksum[3] = 0x03;
			break;

		case STARBASE_3001:
			checksum[0] = 0xee;
			checksum[1] = 0x02;
			checksum[2] = 0xc2;
			checksum[3] = 0x07;
			break;

		case DEEP_FREEZE:
			checksum[0] = 0x06;
			checksum[1] = 0x01;
			checksum[2] = 0x32;
			checksum[3] = 0x02;
			break;

		case ROBOT_CITY:
			checksum[0] = 0x7f;
			checksum[1] = 0x03;
			checksum[2] = 0x93;
			checksum[3] = 0x07;
			break;

		case MOON_BASE:
			checksum[0] = 0xad;
			checksum[1] = 0x01;
			checksum[2] = 0x3b;
			checksum[3] = 0x04;
			break;

		case PORTABELLO:
			checksum[0] = 0xdd;
			checksum[1] = 0x07;
			checksum[2] = 0x15;
			checksum[3] = 0x18;
			break;

		case LEGO_CLUB:
			checksum[0] = 0x38;
			checksum[1] = 0x01;
			checksum[2] = 0x04;
			checksum[3] = 0x02;
			break;

		case CRUX_PRIME:
			checksum[0] = 0x99;
			checksum[1] = 0xa3;
			checksum[2] = 0x17;
			checksum[3] = 0x4b;
			break;

		case NEXUS_TOWER:
			checksum[0] = 0x3c;
			checksum[1] = 0xf4;
			checksum[2] = 0x4a;
			checksum[3] = 0x9e;
			break;

		case NINJAGO_MONASTERY:
			checksum[0] = 0x74;
			checksum[1] = 0x2c;
			checksum[2] = 0x69;
			checksum[3] = 0x4d;
			break;

		case FRAKJAW_BATTLE:
			checksum[0] = 0xef;
			checksum[1] = 0x00;
			checksum[2] = 0xeb;
			checksum[3] = 0x09;
			isBattle = true;
			break;

		}
	}
};