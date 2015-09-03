#pragma once
#include "RakNet\RakPeerInterface.h"
#include "RakNet\BitStream.h"
#include "RakNet\ReplicaManager.h"

#include "legoPackets.h"

class WorldServer{
private:
	static RakPeerInterface* rakServer;
	static ReplicaManager *replicaManager;
public:
	static ReplicaManager * getRM();
	static void publishWorldServer(RakPeerInterface* peer, ReplicaManager * rm);
	static RakNet::BitStream *initPacket(RemoteConnection conntype, unsigned long packetid);
	static void sendPacket(RakNet::BitStream * packet, const SystemAddress& address);
	static void sendPacket(const std::vector<unsigned char>& msg, const SystemAddress& address);
	static void savePacket(RakNet::BitStream * packet, std::string filename);
	//Some helper functions
	static void writeText(RakNet::BitStream * aw, std::string txt);
	static void writeText2(RakNet::BitStream * aw, std::string txt);
};