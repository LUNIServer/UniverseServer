#pragma once

#include "Common.h"

enum LOT : long{
	LOT_NONE = 0,
	LOT_PLAYER = 1,
	LOT_SHIRT_LU_PURPLE = 16065,
	LOT_SHIRT_LU_LIGHT_GREY = 16066,
	LOT_SHIRT_LU_GREY = 16067,
	LOT_SHIRT_LU_GREEN = 16068,
	LOT_SHIRT_LU_ORANGE = 16069,
	LOT_SHIRT_LU_BEIGE = 16070,
	LOT_SHIRT_ARMOR_RED = 16071,
	LOT_SHIRT_ARMOR_BLACK = 16072,
	LOT_SHIRT_ARMOR_BLUE = 16073,
	LOT_SHIRT_ARMOR_WHITE = 16074,
	LOT_SHIRT_ARMOR_PURPLE = 16075,
	LOT_SHIRT_ARMOR_LIGHT_GREY = 16077,
	LOT_SHIRT_ARMOR_GREY = 16078,
	LOT_SHIRT_ARMOR_GREEN_PALE = 16079,
	LOT_SHIRT_ARMOR_ORANGE = 16080,
	LOT_SHIRT_ARMOR_BEIGE = 16081,
	LOT_SHIRT_OLD_GREY = 16082,
	LOT_JETPACK = 1727,
	LOT_KINGS_CROWN = 8544,
	LOT_UNKNOWN4 = 8642,
	LOT_SHIELD_WOLF = 12677,
	LOT_FANTASTIC_PILUM = 12730
};

enum REPLICA_PACKET_TYPE{
	REPLICA_CONSTRUCTION_PACKET = 0,
	REPLICA_SERIALIZATION_PACKET = 1,
	REPLICA_DESTRUCTION_PACKAGE = 2
};

#pragma pack(push, 1)
struct PLAYER_STATS{
	ulonglong CurrencyCollected = 0;
	ulonglong BricksCollected = 0;
	ulonglong SmashablesSmashed = 0;
	ulonglong QuickBuildsCompleted = 0;
	ulonglong EnemiesSmashed = 0;
	ulonglong RocketsUsed = 0;
	ulonglong MissionsCompleted = 0;
	ulonglong PetsTamed = 0;
	ulonglong ImaginationPowerupsCollected = 0;
	ulonglong LifePowerupsCollected = 0;
	ulonglong ArmorPowerupsCollected = 0;
	ulonglong DistanceTraveled = 0;
	ulonglong Smashed = 0;
	ulonglong DamageTaken = 0;
	ulonglong DamageHealed = 0;
	ulonglong ArmorRepaired = 0;
	ulonglong ImaginationRestored = 0;
	ulonglong ImaginationUsed = 0;
	ulonglong DistanceDriven = 0;
	ulonglong AirborneInRacecar = 0;
	ulonglong RacingImaginationPowerupsCollected = 0;
	ulonglong RacingImaginationCratesSmashed = 0;
	ulonglong RaceCarBoostActivated = 0;
	ulonglong WrecksInRaceCar = 0;
	ulonglong RacingSmashablesSmashed = 0;
	ulonglong RacesFinished = 0;
	ulonglong Place1RacesFinishes = 0;
};

struct PLAYER_STYLE{
	ulong hairColor = 0;
	ulong hairStyle = 0;
	ulong HD_HDC = 0;
	ulong shirtColor = 0;
	ulong pantsColor = 0;
	ulong CD = 0;
	ulong HDC_HD = 0;
	ulong eyebrowsStyle = 0;
	ulong eyesStyle = 0;
	ulong mouthStyle = 0;
};

struct PLAYER_INFO{
	ulonglong accountID = 0;
	ulonglong llog = 0;
	ulonglong unknown = 0;
	ulonglong legoScore = 0;
	bool isFreeToPlay = false;
};

struct REPLICA_INDEX29_6_DATA{
	bool d29_6_1 = false;
	bool d29_6_2 = false;
	uchar d29_6_3 = 0;
	bool d29_6_4 = false;
	uchar d29_6_5 = 0;
};

struct REPLICA_INDEX29_8_DATA{
	ulonglong d29_8_1 = 0;
	uchar d29_8_2 = 0;
	bool d29_8_3 = true;
	ulong d29_8_4 = 0xFFFFFFFF;
};

struct REPLICA_OBJECT_BASE{
	ulonglong objid;
	LOT LOT;
};

struct REPLICA_OBJECT_EQUIPMENT{
	REPLICA_OBJECT_BASE obj;
	ushort slot;
};

struct REPLICA_OBJECT_POSITION{
	float posX = 0.0F;
	float posY = 0.0F;
	float posZ = 0.0F;
	void setPosition(float x, float y, float z){
		posX = x;
		posY = y;
		posZ = z;
	};
	REPLICA_OBJECT_POSITION(float x, float y, float z){
		posX = x;
		posY = y;
		posZ = z;
	}
	REPLICA_OBJECT_POSITION(){
		posX = 0;
		posY = 0;
		posZ = 0;
	}
};
#pragma pack(pop)