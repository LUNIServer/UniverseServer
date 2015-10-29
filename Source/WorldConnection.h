#pragma once
#include "RakNet\BitStream.h"
#include "RakNet\RakNetTypes.h"

struct ZoneAddress{
	unsigned short zoneid;
	unsigned short instanceid;
	unsigned long cloneid;
};

class ClientWorldPackets{
public:
	static void HandlePacket(RakNet::BitStream * data, SystemAddress sender);
	static bool HandleValidation(RakNet::BitStream * data, SystemAddress sender);
	static ZoneAddress HandleLevelLoadComplete(RakNet::BitStream * data, SystemAddress sender);
	//53-04-00-19 MSG_WORLD_CLIENT_STRING_CHECK
};

enum WorldServerPacketIds{
	MSG_CLIENT_CREATE_CHARACTER = 4,
	MSG_CLIENT_GAME_MSG = 12,
};

class WorldServerPackets{
public:
	//53-05-00-04 MSG_CLIENT_CREATE_CHARACTER
	static void CreateCharacter(SystemAddress address, long long charobjid);
	//53-05-00-0C MSG_CLIENT_GAME_MSG
	static void SendGameMessage(SystemAddress address, long long charobjid, unsigned short gmsgid);
	static RakNet::BitStream *InitGameMessage(long long charobjid, unsigned short gmsgid);
};