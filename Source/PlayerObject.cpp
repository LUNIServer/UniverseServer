#include "Database.h"
#include "PlayerObject.h"
#include "Replica.h"

#include "RakNet\ReplicaManager.h"
#include "UsersPool.h"
#include "AccountsDB.h"
#include "Worlds.h"

#include "Logger.h"
#include "UtfConverter.h"

#include <map>

extern ReplicaManager replicaManager;
extern std::map<SystemAddress, ZoneId> Player;
extern Ref< UsersPool > WorldOnlineUsers;

PlayerObject::PlayerObject(long long objid, std::wstring name){
	this->objid = objid;
	this->name = name;

	//The order is VERY IMPORTANT for the packet 1,7,4,17,9,2,107
	this->addComponent(new Component1());
	this->addComponent(new Component7());
	this->addComponent(new Component4());
	this->addComponent(new Component17());
	this->addComponent(new Component9());
	this->addComponent(new Component2());
	this->addComponent(new Component107());
	ObjectsManager::registerObject(this);
}

PlayerObject::~PlayerObject(){
	this->deleteComponents();
	ObjectsManager::unregisterObject(this);
	replicaManager.DereferencePointer(this);
}

std::wstring PlayerObject::getName(){
	return this->name;
}

long long PlayerObject::getObjectID(){
	return this->objid;
}

// --- Components ---

Component1 *PlayerObject::getComponent1(){ return (Component1 *) this->getComponent(1); }
Component7 *PlayerObject::getComponent7(){ return (Component7 *) this->getComponent(7); }
Component4 *PlayerObject::getComponent4(){ return (Component4 *) this->getComponent(4); }
Component17 *PlayerObject::getComponent17(){ return (Component17 *) this->getComponent(17); }
Component9 *PlayerObject::getComponent9(){ return (Component9 *) this->getComponent(9); }
Component2 *PlayerObject::getComponent2(){ return (Component2 *) this->getComponent(2); }
Component107 *PlayerObject::getComponent107(){ return (Component107 *) this->getComponent(107); }

void PlayerObject::doCreation(SystemAddress playerController, ZoneId playerZone, bool createOthers){
	//Add information about the client
	this->clientIP = playerController;
	this->zone = playerZone;
	//Create the object for the client
	this->create(playerController, true);

	std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(playerZone);
	for (std::vector<SessionInfo>::iterator it = sess.begin(); it != sess.end(); ++it){
		if (it->addr != playerController){
			PlayerObject * other = (PlayerObject *) ObjectsManager::getObjectByID(it->activeCharId);
			if (other != NULL){ //Player not null
				if (createOthers) other->create(playerController); //Create Others for me
				this->create(it->addr); //Create me for others
			}
		}
	}
}

void PlayerObject::create(SystemAddress address, bool himself){
	replicaManager.Construct(this, false, address, false);
	if (himself){
		//replicaManager.DisableReplicaInterfaces(this, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE);
		this->isCreated = true;
	}
}

void PlayerObject::doSerialization(SystemAddress playerController, ZoneId playerZone){
	if (!isCreated){
		Logger::log("WRLD", "RMPL", "Player [objid:" + std::to_string(this->objid) + "] has not been created!", LOG_WARNING);
	}
	else{
		std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(playerZone);
		for (std::vector<SessionInfo>::iterator it = sess.begin(); it != sess.end(); ++it){
			this->serialize(it->addr);
		}
	}
}

void PlayerObject::serialize(SystemAddress address){
	replicaManager.SignalSerializeNeeded(this, address, false);
}

void PlayerObject::serialize(){
	this->doSerialization(this->clientIP, this->zone);
}

void PlayerObject::doDestruction(SystemAddress playerController, ZoneId playerZone, bool destructOthers){
	if (!isCreated){
		Logger::log("REPL", "PLAYER", "Player [objid:" + std::to_string(this->objid) + "] has not been created!", LOG_WARNING);
	}
	else{
		isCreated = false;
		std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(playerZone);
		for (std::vector<SessionInfo>::iterator it = sess.begin(); it != sess.end(); ++it){
			if (it->addr != playerController){
				this->destruct(it->addr);
				PlayerObject * p = (PlayerObject *) ObjectsManager::getObjectByID(it->activeCharId);
				if (destructOthers & (p != NULL)) p->destruct(playerController);
			}
		}
		this->destruct(playerController); //Destroying my char for me
	}
}

void PlayerObject::destruct(SystemAddress address){
	replicaManager.Destruct(this, address, false);
}

void PlayerObject::destruct(){
	if (isCreated){
		this->doDestruction(this->clientIP, this->zone);
	}
}