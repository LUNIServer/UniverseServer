#pragma once

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
	unsigned long long CurrencyCollected = 0;
	unsigned long long BricksCollected = 0;
	unsigned long long SmashablesSmashed = 0;
	unsigned long long QuickBuildsCompleted = 0;
	unsigned long long EnemiesSmashed = 0;
	unsigned long long RocketsUsed = 0;
	unsigned long long MissionsCompleted = 0;
	unsigned long long PetsTamed = 22;
	unsigned long long ImaginationPowerupsCollected = 0;
	unsigned long long LifePowerupsCollected = 0;
	unsigned long long ArmorPowerupsCollected = 0;
	unsigned long long DistanceTraveled = 0;
	unsigned long long Smashed = 0;
	unsigned long long DamageTaken = 0;
	unsigned long long DamageHealed = 0;
	unsigned long long ArmorRepaired = 0;
	unsigned long long ImaginationRestored = 0;
	unsigned long long ImaginationUsed = 0;
	unsigned long long DistanceDriven = 0;
	unsigned long long AirborneInRacecar = 0;
	unsigned long long RacingImaginationPowerupsCollected = 0;
	unsigned long long RacingImaginationCratesSmashed = 0;
	unsigned long long RaceCarBoostActivated = 0;
	unsigned long long WrecksInRaceCar = 0;
	unsigned long long RacingSmashablesSmashed = 0;
	unsigned long long RacesFinished = 0;
	unsigned long long Place1RacesFinishes = 0;
};

struct PLAYER_STYLE{
	unsigned long hairColor = 0;
	unsigned long hairStyle = 0;
	unsigned long HD_HDC = 0;
	unsigned long shirtColor = 0;
	unsigned long pantsColor = 0;
	unsigned long CD = 0;
	unsigned long HDC_HD = 0;
	unsigned long eyebrowsStyle = 0;
	unsigned long eyesStyle = 0;
	unsigned long mouthStyle = 0;
};

struct PLAYER_INFO{
	unsigned long long accountID = 0;
	unsigned long long llog = 0;
	unsigned long long unknown = 0;
	unsigned long long legoScore = 0;
	bool isFreeToPlay = false;
};

struct REPLICA_INDEX29_6_DATA{
	bool d29_6_1 = false;
	bool d29_6_2 = false;
	unsigned char d29_6_3 = 0;
	bool d29_6_4 = false;
	unsigned char d29_6_5 = 0;
};

struct REPLICA_INDEX29_8_DATA{
	unsigned long long d29_8_1 = 0;
	unsigned char d29_8_2 = 0;
	bool d29_8_3 = true;
	unsigned long d29_8_4 = 0xFFFFFFFF;
};

struct REPLICA_OBJECT_BASE{
	unsigned long long objid;
	LOT LOT;
};

struct REPLICA_OBJECT_EQUIPMENT{
	REPLICA_OBJECT_BASE obj;
	unsigned short slot;
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