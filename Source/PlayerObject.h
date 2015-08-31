#pragma once
//#include "Replica.h"
#include "ReplicaData.h"
#include "ReplicaComponents.h"

#include "SUtil\Common.h"
#include "RakNet\Replica.h"

#include "ReplicaObject.h"

#include <string>

class PlayerObject : public ReplicaObject{
public:
	PlayerObject(long long objid, std::wstring name);

	// -- Components --
	Component1 *getComponent1();
	Component7 *getComponent7();
	Component4 *getComponent4();
	Component17 *getComponent17();
	Component9 *getComponent9();
	Component2 *getComponent2();
	Component107 *getComponent107();
};