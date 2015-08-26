
#include "ReplicaData.h"
#include "ReplicaComponents.h"

#include <vector>
#include <map>

std::map<ushort, std::vector<uchar>> zonePacketData;
std::map<ushort, COMPONENT1_POSITION> zoneDefaultSpawn;
bool zoneDataInitialized = false;

/* -- NEXUS TOWER: Teleportation -- 
	Paradox Arrivals:
		Right Top:		-598.837 1076.14 334.392
		Right Middle:	-558.746 1078.94 261.92
		Right Bottom:	-484.06  1081.57 228.291

		Left Top:		-399.213 1075.52 454
		Left Middle:	-344.333 1078.01 394.265
		Left Bottom:	-336.69  1080.23 312.124

		Main Hall		-515.955 1063.3 424.354

	Paradox Departures
		Right Nexus:	-450.591 1082.52 214.319
		Right Sentinels	-599.272 1079.94 255.039
		Right Paradox	-616.472 1077.04 365.313

		Left Paradox	-423.872 1076.43 479.24
		Left Assembly	-323.638 1079.0  429.542
		Left Nexus		-333.865 1081.18 275.604

		Left Hall		-505.456 1064.45 496.331
		Right Hall		-589.149 1064.45 438.654

	Paradox Intakes:
		Form Sentinel	-641.175 1117.75 181.414
		From Nexus R	-438.273 1099.1  278.434
		From Nexus L	-413.61  1099.29 296.354
		From Assembly	-283.592 1122.22 452.148

	Sentinel Upper Level	-520 1158.39 -31

	Assembly Left Stream	-185.303 1128.84 317.99
	Assembly Left Target	-212.679 1218.34 263.548
	Assembly Left Top Warp	-230.705 1218.34 291.677

	Assembly Right Stream	-5.0912  1128.89 59.2409
	Assembly Right Target	-64.2514 1218.35 46.5634
	Assembly Right Top Warp	-64.1482 1218.35 17.8295
	Assembly RightWallInset	-59.2903 1196.75 100.131

	Venture League Observatory Arrival
						4.4828 1205.29 -62.6502

	Forbidden Valley
		189 300 -40
		74 335 84
	
*/

void InitZoneData(){
	zonePacketData[VENTURE_EXPLORER] = { 0xe8, 0x03, 0x7c, 0x08, 0xb8, 0x20 };
	zonePacketData[VENTURE_EXPLORER_RETURN] = { 0xe9, 0x03, 0x3c, 0x0a, 0x68, 0x26 };
	zonePacketData[AVANT_GARDENS] = { 0x4c, 0x04, 0x11, 0x55, 0x52, 0x49 };
	zonePacketData[AVANT_GARDENS_SURVIVAL] = { 0x4c, 0x04, 0x11, 0x55, 0x52, 0x49 };
	zonePacketData[SPIDER_QUEEN_BATTLE] = { 0x4e, 0x04, 0xda, 0x03, 0xd4, 0x0f };
	zonePacketData[BLOCK_YARD] = { 0x7e, 0x04, 0xda, 0x03, 0xd4, 0x0f };
	zonePacketData[AVANT_GROVE] = { 0x7f, 0x04, 0x03, 0x03, 0x89, 0x0a };
	zonePacketData[NIMBUS_STATION] = { 0xb0, 0x04, 0x30, 0x6b, 0x1e, 0xda };
	zonePacketData[PET_COVE] = { 0xb1, 0x04, 0x30, 0x13, 0x6e, 0x47 };
	zonePacketData[VERTIGO_LOOP_RACETRACK] = { 0xb3, 0x04, 0x02, 0x05, 0xfc, 0x10 };
	zonePacketData[BATTLE_OF_NIMBUS_STATION] = { 0xb4, 0x04, 0x58, 0x02, 0xd4, 0x07 };
	zonePacketData[NIMBUS_ROCK] = { 0xe2, 0x04, 0x91, 0x01, 0x8d, 0x05 };
	zonePacketData[NIMBUS_ISLE] = { 0xe3, 0x04, 0x5d, 0x04, 0x4f, 0x09 };
	zonePacketData[GNARLED_FOREST] = { 0x14, 0x05, 0x90, 0xc2, 0xea, 0x12 };
	zonePacketData[CANYON_COVE] = { 0x16, 0x05, 0xef, 0x02, 0x77, 0x0b };
	zonePacketData[CHANTEY_SHANTEY] = { 0x46, 0x05, 0x5c, 0x01, 0xb6, 0x04 };
	zonePacketData[FORBIDDEN_VALLEY] = { 0x78, 0x05, 0x0d, 0x76, 0x19, 0x85 };
	zonePacketData[FORBIDDEN_VALLEY_DRAGON] = { 0x7a, 0x05, 0x87, 0x01, 0xf5, 0x02 };
	zonePacketData[DRAGONMAW_CHASM] = { 0x7b, 0x05, 0x4e, 0x0f, 0x85, 0x81 };
	zonePacketData[RAVEN_BLUFF] = { 0xaa, 0x05, 0x26, 0x01, 0xf0, 0x03 };
	zonePacketData[STARBASE_3001] = { 0x40, 0x06, 0xee, 0x02, 0xc2, 0x07 };
	zonePacketData[DEEP_FREEZE] = { 0x41, 0x06, 0x06, 0x01, 0x32, 0x02 };
	zonePacketData[ROBOT_CITY] = { 0x42, 0x06, 0x7f, 0x03, 0x93, 0x07 };
	zonePacketData[MOON_BASE] = { 0x43, 0x06, 0xad, 0x01, 0x3b, 0x04 };
	zonePacketData[PORTABELLO] = { 0x44, 0x06, 0xdd, 0x07, 0x15, 0x18 };
	zonePacketData[LEGO_CLUB] = { 0xa4, 0x06, 0x38, 0x01, 0x04, 0x02 };
	zonePacketData[CRUX_PRIME] = { 0x08, 0x07, 0x99, 0xa3, 0x17, 0x4b };
	zonePacketData[NEXUS_TOWER] = { 0x6c, 0x07, 0x3c, 0xf4, 0x4a, 0x9e };
	zonePacketData[NINJAGO_MONASTERY] = { 0xd0, 0x07, 0x74, 0x2c, 0x69, 0x4d };
	zonePacketData[FRANKJAW_BATTLE] = { 0xd1, 0x07, 0xef, 0x00, 0xeb, 0x09 };

	zoneDefaultSpawn[VENTURE_EXPLORER] = COMPONENT1_POSITION(-627.1862F, 613.326233F, -47.2231674F);
	zoneDefaultSpawn[VENTURE_EXPLORER_RETURN] = COMPONENT1_POSITION(-187.2391F, 608.2743F, 54.5554352F);
	zoneDefaultSpawn[AVANT_GARDENS] = COMPONENT1_POSITION(522.9949F, 406.040375F, 129.992722F);
	zoneDefaultSpawn[AVANT_GARDENS_SURVIVAL] = COMPONENT1_POSITION(35.0297F, 365.780426F, -201.578369F);
	zoneDefaultSpawn[SPIDER_QUEEN_BATTLE] = COMPONENT1_POSITION(-18.7062054F, 440.20932F, 37.5326424F);
	zoneDefaultSpawn[BLOCK_YARD] = COMPONENT1_POSITION(-18.7062054F, 440.20932F, 37.5326424F);
	zoneDefaultSpawn[AVANT_GROVE] = COMPONENT1_POSITION(25.0526543F, 472.215027F, -24.318882F);
	zoneDefaultSpawn[NIMBUS_STATION] = COMPONENT1_POSITION(-40.0F, 293.047F, -16.0F);
	zoneDefaultSpawn[PET_COVE] = COMPONENT1_POSITION(111.670906F, 229.282776F, 179.87793F);
	zoneDefaultSpawn[VERTIGO_LOOP_RACETRACK] = COMPONENT1_POSITION(0.0F,0.0F,0.0F);
	zoneDefaultSpawn[BATTLE_OF_NIMBUS_STATION] = COMPONENT1_POSITION(-12.1019106F, 212.900024F, 191.147964F);
	zoneDefaultSpawn[NIMBUS_ROCK] = COMPONENT1_POSITION(-17.8299046F, 440.509674F, 30.0326862F);
	zoneDefaultSpawn[NIMBUS_ISLE] = COMPONENT1_POSITION(31.55009F, 470.885254F, 193.457321F);
	zoneDefaultSpawn[GNARLED_FOREST] = COMPONENT1_POSITION(-329.965881F, 302.470184F, -470.232758F);
	zoneDefaultSpawn[CANYON_COVE] = COMPONENT1_POSITION(-293.072571F, 233.0F, -4.16148F);
	zoneDefaultSpawn[CHANTEY_SHANTEY] = COMPONENT1_POSITION(-19.713892F, 440.20932F, 26.935009F);
	zoneDefaultSpawn[FORBIDDEN_VALLEY] = COMPONENT1_POSITION(390.284363F, 229.452881F, -511.350983F);
	zoneDefaultSpawn[FORBIDDEN_VALLEY_DRAGON] = COMPONENT1_POSITION(-264.426575F, 290.3452F, 308.619049F);
	zoneDefaultSpawn[DRAGONMAW_CHASM] = COMPONENT1_POSITION(-1457.71826F, 794.0F, -332.2917F);
	zoneDefaultSpawn[RAVEN_BLUFF] = COMPONENT1_POSITION(-26.8431015F, 425.11496F, 53.7349777F);
	zoneDefaultSpawn[STARBASE_3001] = COMPONENT1_POSITION(34.6352119F, 1571.29309F, 48.0321465F); //From Nimbus Station
	zoneDefaultSpawn[DEEP_FREEZE] = COMPONENT1_POSITION(-90.30964F, 211.087067F, -126.3196F);
	zoneDefaultSpawn[ROBOT_CITY] = COMPONENT1_POSITION(-163.2F, 217.254913F, 172.0F);
	zoneDefaultSpawn[MOON_BASE] = COMPONENT1_POSITION(9.18036652F, 48.79997F, 109.610374F);
	zoneDefaultSpawn[PORTABELLO] = COMPONENT1_POSITION(-99.80103F, 231.916946F, -162.67955F);
	zoneDefaultSpawn[LEGO_CLUB] = COMPONENT1_POSITION(-359.979156F, 1066.328F, -369.287781F);
	zoneDefaultSpawn[CRUX_PRIME] = COMPONENT1_POSITION(-241.965515F, 92.78052F, 557.327942F);
	zoneDefaultSpawn[NEXUS_TOWER] = COMPONENT1_POSITION(165.355682F, 1164.17822F, -543.9093F);
	zoneDefaultSpawn[NINJAGO_MONASTERY] = COMPONENT1_POSITION(-446.79715F, 171.158859F, 1122.83545F);
	zoneDefaultSpawn[FRANKJAW_BATTLE] = COMPONENT1_POSITION(11.26009F, 211.05188F, 40.6721039F);

	zoneDataInitialized = true;
}

std::vector<uchar> getWorldTarget(ZoneId zone_id){
	if (!zoneDataInitialized){
		InitZoneData();
	}
	try{
		std::vector<uchar> zpd = zonePacketData.at(zone_id);
		return zpd;
	}
	catch (...){
		return std::vector<uchar>();
	}
}

COMPONENT1_POSITION getZoneSpawnPoint(ZoneId zone_id, ZoneId from_zone_id){
	if (!zoneDataInitialized){
		InitZoneData();
	}
	switch (zone_id){
	case ZoneId::AVANT_GARDENS:
		switch (from_zone_id){
		case ZoneId::VENTURE_EXPLORER:
			return COMPONENT1_POSITION(-406.571442F, 350.836334F, -157.3197F);
		case ZoneId::AVANT_GARDENS_SURVIVAL:
			return COMPONENT1_POSITION(131.83F, 376.0F, -180.31F);
		case ZoneId::SPIDER_QUEEN_BATTLE:
			return COMPONENT1_POSITION(-562.3F, 352.8F, 246.9F);
		}
		break;
	case ZoneId::NIMBUS_STATION:
		switch (from_zone_id){
		case ZoneId::AVANT_GARDENS:
			return COMPONENT1_POSITION(-153.569275F, 369.9166F, 369.785126F);
		case ZoneId::FORBIDDEN_VALLEY:
			return COMPONENT1_POSITION(190.254669F, 331.9915F, -155.040436F);
		case ZoneId::PET_COVE:
			return COMPONENT1_POSITION(-415.1238F, 288.796082F, 185.24292F);
		case ZoneId::NIMBUS_ROCK:
			return COMPONENT1_POSITION(-386.289581F, 289.389221F, 36.612278F);
		case ZoneId::NIMBUS_ISLE:
			return COMPONENT1_POSITION(-367.905945F, 288.476074F, 117.637619F);
		case ZoneId::GNARLED_FOREST:
			return COMPONENT1_POSITION(-254.397614F, 291.5348F, -218.924835F);
		case ZoneId::NEXUS_TOWER:
			return COMPONENT1_POSITION(214.9495F, 299.542175F, 256.375946F);
		case ZoneId::BATTLE_OF_NIMBUS_STATION:
			return COMPONENT1_POSITION(-184.2F, 346.5F, 263.9F);
		case ZoneId::STARBASE_3001:
			return COMPONENT1_POSITION(-355.688873F, 287.545776F, 190.9118F);
		case ZoneId::VERTIGO_LOOP_RACETRACK:
			return COMPONENT1_POSITION(261.1F, 287.7F, 188.9F);
		case ZoneId::LEGO_CLUB:
			return COMPONENT1_POSITION(253.142761F, 249.076019F, -720.716736F);
		}
		break;
	case ZoneId::GNARLED_FOREST:
		switch (from_zone_id)
		{
		case ZoneId::CANYON_COVE:
			return COMPONENT1_POSITION(346.7F, 259.4F, -71.1F);
		case ZoneId::CHANTEY_SHANTEY:
			return COMPONENT1_POSITION(235.943985F, 265.722382F, -187.185852F);
		}
		break;
	case ZoneId::FORBIDDEN_VALLEY:
		switch (from_zone_id){
		case ZoneId::FORBIDDEN_VALLEY_DRAGON:
			return COMPONENT1_POSITION(-342.2F, -7.9F, -376.7F);
		case ZoneId::DRAGONMAW_CHASM:
			return COMPONENT1_POSITION(782.3F, 239.5F, 456.8F);
		case ZoneId::RAVEN_BLUFF:
			return COMPONENT1_POSITION(110.862823F, 260.9398F, -320.947968F);
		}
		break;
	case ZoneId::STARBASE_3001:
		switch (from_zone_id){
		case ZoneId::DEEP_FREEZE:
		case ZoneId::PORTABELLO:
		case ZoneId::MOON_BASE:
			return COMPONENT1_POSITION(117.833183F, 1535.99133F, 69.01117F);
		}
		break;
	case ZoneId::NEXUS_TOWER:
		switch (from_zone_id){
		case ZoneId::NINJAGO_MONASTERY:
			return COMPONENT1_POSITION(-70.96269F, 1180.6925F, -576.666F);
		}
		break;
	case ZoneId::NINJAGO_MONASTERY:
		switch (from_zone_id){
		case ZoneId::FRANKJAW_BATTLE:
			return COMPONENT1_POSITION(-347.0F, 144.9F, -461.0F);
		}
	}

	return zoneDefaultSpawn[zone_id];
}