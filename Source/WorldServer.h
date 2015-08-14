#pragma once
#include "RakNet\RakPeerInterface.h"
#include "RakNet\BitStream.h"

#include "legoPackets.h"

class WorldServer{
private:
	static RakPeerInterface* rakServer;
public:
	static void publishWorldServer(RakPeerInterface* peer);
	static RakNet::BitStream *initPacket(RemoteConnection conntype, unsigned long packetid);
	static void sendPacket(RakNet::BitStream * packet, SystemAddress address);
	static void savePacket(RakNet::BitStream * packet, std::string filename);
	//Some helper functions
	static void writeText(RakNet::BitStream * aw, std::string txt);
	static void writeText2(RakNet::BitStream * aw, std::string txt);
};