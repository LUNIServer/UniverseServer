#pragma once
//#include "Replica.h"
#include "ReplicaData.h"
#include "ReplicaComponents.h"

#include "SUtil\Common.h"
#include "RakNet\Replica.h"

#include "ReplicaObject.h"

#include <string>

class PlayerObject : public ReplicaObject{
private:
	//Client reference
	SystemAddress clientIP;
	ZoneId zone;

	bool isCreated = false;
	// -- Specific Values -- 
public:
	PlayerObject(long long objid, std::wstring name);
	~PlayerObject();

	// -- Specific Values -- 
	std::wstring getName();
	long long getObjectID();

	// -- Components --
	Component1 *getComponent1();
	Component7 *getComponent7();
	Component4 *getComponent4();
	Component17 *getComponent17();
	Component9 *getComponent9();
	Component2 *getComponent2();
	Component107 *getComponent107();

	void doCreation(SystemAddress playerController, ZoneId playerZone, bool createOthers = true);
	void create(SystemAddress address, bool himself = false);
	void doSerialization(SystemAddress playerController, ZoneId playerZone);
	void serialize(SystemAddress address);
	void serialize();
	void doDestruction(SystemAddress playerController, ZoneId playerZone, bool destructOthers = true);
	void destruct();
	void destruct(SystemAddress address);
};