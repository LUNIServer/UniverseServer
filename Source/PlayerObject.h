#pragma once
//#include "Replica.h"
#include "ReplicaData.h"
#include "ReplicaComponents.h"

#include "RakNet\Replica.h"

#include "ReplicaObject.h"

#include <string>

class PlayerObject : public ReplicaObject{
public:
	PlayerObject(long long objid, std::wstring name);

	// -- Components --
	ControllablePhysicsComponent *getComponent1();
	DestructibleComponent *getComponent7();
	CharacterComponent *getComponent4();
	InventoryComponent *getComponent17();
	SkillComponent *getComponent9();
	RenderComponent *getComponent2();
	Index36Component *getComponent107();
};