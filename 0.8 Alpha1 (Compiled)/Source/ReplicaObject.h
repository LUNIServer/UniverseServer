#pragma once
#include "ReplicaComponents.h"
#include "RakNet\Replica.h"
#include "World.h"

#include <vector>

class ReplicaObject : public Replica {
	std::vector<ReplicaComponent *> components;
public:
	long long objid;
	std::wstring name;
	unsigned long LOT;
	unsigned char gmlevel = 0;
	World world;

	~ReplicaObject();

	// -- Specific Values -- 
	std::wstring getName();
	long long getObjectID();

	ReplicaComponent *getComponent(unsigned int componentid);
	void addComponent(ReplicaComponent * component);
	void writeToPacket(RakNet::BitStream * packet, REPLICA_PACKET_TYPE packetType);
	void deleteComponents();

	ReplicaReturnResult SendConstruction(RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp);
	ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
	ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress);
	int GetSortPriority(void) const { return 0; }
};