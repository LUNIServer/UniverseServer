#include "Database.h"
#include "PlayerObject.h"
#include "Replica.h"

#include "RakNet\ReplicaManager.h"
#include "AccountsDB.h"
#include "Worlds.h"

#include "Logger.h"
#include "UtfConverter.h"

#include <map>

//extern ReplicaManager replicaManager;
//extern std::map<SystemAddress, ZoneId> Player;

PlayerObject::PlayerObject(long long objid, std::wstring name){
	this->objid = objid;
	this->name = name;
	this->LOT = 1UL;

	//The order is VERY IMPORTANT for the packet: 1,7,4,17,9,2,107
	this->addComponent(new Component1());
	this->addComponent(new Component7());
	this->addComponent(new Component4());
	this->addComponent(new Component17());
	this->addComponent(new Component9());
	this->addComponent(new Component2());
	this->addComponent(new Component107());
	ObjectsManager::registerObject(this);
}

Component1 *PlayerObject::getComponent1(){ return (Component1 *) this->getComponent(1); }
Component7 *PlayerObject::getComponent7(){ return (Component7 *) this->getComponent(7); }
Component4 *PlayerObject::getComponent4(){ return (Component4 *) this->getComponent(4); }
Component17 *PlayerObject::getComponent17(){ return (Component17 *) this->getComponent(17); }
Component9 *PlayerObject::getComponent9(){ return (Component9 *) this->getComponent(9); }
Component2 *PlayerObject::getComponent2(){ return (Component2 *) this->getComponent(2); }
Component107 *PlayerObject::getComponent107(){ return (Component107 *) this->getComponent(107); }