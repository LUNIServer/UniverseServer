#pragma once

//#include "Common.h"

#include <iostream>

// This is the ZoneID enum containing the World IDs
enum ZoneId : unsigned short { 
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
	NINJAGO_MONASTERY = 2000, FRANKJAW_BATTLE = 2001
};

// Prints World/Zone name
std::ostream& operator<<(std::ostream& out, const ZoneId value);
std::string& operator+(std::string& str, const ZoneId value);

// This is the World struct
struct World {
	unsigned short zone = 0;
	unsigned short instance = 0;
	unsigned long clone = 0;

	World(unsigned short zoneid, unsigned short instanceid, unsigned long cloneid){
		this->zone = zoneid;
		this->instance = instanceid;
		this->clone = cloneid;
	}

	World(){}
};

bool operator==(const World& left, const World& right);
