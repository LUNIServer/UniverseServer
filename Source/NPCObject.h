#pragma once

#include "ReplicaData.h"
#include "ReplicaComponents.h"

#include "Common.h"
#include "RakNet\Replica.h"

#include "ReplicaObject.h"

#include <string>

class NPCObject : public ReplicaObject {
private:
	SystemAddress clientIP;

	bool isCreated = false; // Determines whether or not the object has been created on the server
	
	void createLOTInfo(int lot);
public:
	NPCObject(long long objid, ulong lot, long long spawnerObjID, bool isVendor = false, bool addScript = false);
	~NPCObject();

	long long getObjectID();
	long long getSpawnerObjID();
	ulong getLOT();

	Component3 *getComponent3();
	Component7 *getComponent7();
	Component17 *getComponent17();
	Component5 *getComponent5();
	Component2 *getComponent2();

	bool isNPCVendor();
	bool isScriptAdded();

	void doCreation(SystemAddress address);
	void create(SystemAddress address);
	void doSerialization(SystemAddress address);
	void serialize(SystemAddress address);
	void serialize();
	void doDestruction(SystemAddress address);
	void destruct();
	void destruct(SystemAddress address);
};