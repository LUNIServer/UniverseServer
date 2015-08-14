#pragma once
//#include "Replica.h"
#include "ReplicaData.h"
#include "ReplicaComponents.h"

#include "SUtil\Common.h"
#include "RakNet\Replica.h"

#include <string>

class PlayerObject : public Replica{
private:
	//Client reference
	SystemAddress clientIP;
	ZoneId zone;

	bool isCreated = false;
	// -- Specific Values -- 
	//REPLICA_OBJECT_POSITION position;
	long long objid;
	std::wstring name;
	// -- Components --
	Component1 *component1;
	Component7 *component7;
	Component4 *component4;
	Component17 *component17;
public:
	PlayerObject(long long objid, std::wstring name);
	~PlayerObject();

	// -- Specific Values -- 
	std::wstring getName();
	// -- Components --
	Component1 *getComponent1();
	Component7 *getComponent7();
	Component4 *getComponent4();
	Component17 *getComponent17();

	void doCreation(SystemAddress playerController, ZoneId playerZone, bool createOthers = true);
	void create(SystemAddress address, bool himself = false);
	void doSerialization(SystemAddress playerController, ZoneId playerZone);
	void serialize(SystemAddress address);
	void serialize();
	void doDestruction(SystemAddress playerController, ZoneId playerZone, bool destructOthers = true);
	void destruct();
	void destruct(SystemAddress address);

	ReplicaReturnResult SendConstruction(RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp);
	ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
	ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress);
	int GetSortPriority(void) const { return 0; }	
};