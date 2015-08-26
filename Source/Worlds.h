#pragma once
#include "RakNet\RakNetTypes.h"
#include "World.h"
#include "ReplicaData.h"

class Worlds{
public:
	static bool loadWorld(SystemAddress address, ZoneId zone, COMPONENT1_POSITION pos, unsigned short instance = 0, unsigned long clone = 0);
};