#include "World.h"
using namespace std;

// Write the World IDs of the worlds
ostream& operator<<(ostream& out, const ZoneId value) {

// Define WRITE_ELEMENT to get the worlds
#define WRITE_ELEMENT(p) case p: return out << #p;

	switch (value) {
	WRITE_ELEMENT(VENTURE_EXPLORER) WRITE_ELEMENT(VENTURE_EXPLORER_RETURN)
	WRITE_ELEMENT(AVANT_GARDENS) WRITE_ELEMENT(AVANT_GARDENS_SURVIVAL) WRITE_ELEMENT(SPIDER_QUEEN_BATTLE) WRITE_ELEMENT(BLOCK_YARD) WRITE_ELEMENT(AVANT_GROVE)
	WRITE_ELEMENT(NIMBUS_STATION) WRITE_ELEMENT(PET_COVE) WRITE_ELEMENT(VERTIGO_LOOP_RACETRACK) WRITE_ELEMENT(BATTLE_OF_NIMBUS_STATION) WRITE_ELEMENT(NIMBUS_ROCK) WRITE_ELEMENT(NIMBUS_ISLE)
	WRITE_ELEMENT(GNARLED_FOREST) WRITE_ELEMENT(CANYON_COVE) WRITE_ELEMENT(KEELHAUL_CANYON) WRITE_ELEMENT(CHANTEY_SHANTEY)
	WRITE_ELEMENT(FORBIDDEN_VALLEY) WRITE_ELEMENT(FORBIDDEN_VALLEY_DRAGON) WRITE_ELEMENT(DRAGONMAW_CHASM) WRITE_ELEMENT(RAVEN_BLUFF)
	WRITE_ELEMENT(STARBASE_3001) WRITE_ELEMENT(DEEP_FREEZE) WRITE_ELEMENT(ROBOT_CITY) WRITE_ELEMENT(MOON_BASE) WRITE_ELEMENT(PORTABELLO)
	WRITE_ELEMENT(LEGO_CLUB)
	WRITE_ELEMENT(CRUX_PRIME)
	WRITE_ELEMENT(NEXUS_TOWER)
	WRITE_ELEMENT(NINJAGO_MONASTERY) WRITE_ELEMENT(FRANKJAW_BATTLE)
	default: return out << "UNKNOWN_ZONE with ID: " << (int)value;
	}

// Undefine WRITE_ELEMENT
#undef WRITE_ELEMENT
}

std::string& operator+(std::string& str, const ZoneId value){
#define WRITE_ELEMENT(p) case p: { str.append(#p); return str; }

	switch (value) {
		WRITE_ELEMENT(VENTURE_EXPLORER) WRITE_ELEMENT(VENTURE_EXPLORER_RETURN)
			WRITE_ELEMENT(AVANT_GARDENS) WRITE_ELEMENT(AVANT_GARDENS_SURVIVAL) WRITE_ELEMENT(SPIDER_QUEEN_BATTLE) WRITE_ELEMENT(BLOCK_YARD) WRITE_ELEMENT(AVANT_GROVE)
			WRITE_ELEMENT(NIMBUS_STATION) WRITE_ELEMENT(PET_COVE) WRITE_ELEMENT(VERTIGO_LOOP_RACETRACK) WRITE_ELEMENT(BATTLE_OF_NIMBUS_STATION) WRITE_ELEMENT(NIMBUS_ROCK) WRITE_ELEMENT(NIMBUS_ISLE)
			WRITE_ELEMENT(GNARLED_FOREST) WRITE_ELEMENT(CANYON_COVE) WRITE_ELEMENT(KEELHAUL_CANYON) WRITE_ELEMENT(CHANTEY_SHANTEY)
			WRITE_ELEMENT(FORBIDDEN_VALLEY) WRITE_ELEMENT(FORBIDDEN_VALLEY_DRAGON) WRITE_ELEMENT(DRAGONMAW_CHASM) WRITE_ELEMENT(RAVEN_BLUFF)
			WRITE_ELEMENT(STARBASE_3001) WRITE_ELEMENT(DEEP_FREEZE) WRITE_ELEMENT(ROBOT_CITY) WRITE_ELEMENT(MOON_BASE) WRITE_ELEMENT(PORTABELLO)
			WRITE_ELEMENT(LEGO_CLUB)
			WRITE_ELEMENT(CRUX_PRIME)
			WRITE_ELEMENT(NEXUS_TOWER)
			WRITE_ELEMENT(NINJAGO_MONASTERY) WRITE_ELEMENT(FRANKJAW_BATTLE)
	default: { str.append("UNKNOWN_ZONE[ " + std::to_string((int)value) + "]"); return str; }
	}

	// Undefine WRITE_ELEMENT
#undef WRITE_ELEMENT
}
