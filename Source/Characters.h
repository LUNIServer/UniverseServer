#pragma once
#include "RakNet\BitStream.h"

class Characters{
public:
	static bool CreateCharacter(RakNet::BitStream *packet, SystemAddress address, unsigned int accountid);
};