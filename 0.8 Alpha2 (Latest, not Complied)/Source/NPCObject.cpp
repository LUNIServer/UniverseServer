#include "Database.h"
#include "NPCObject.h"
#include "Replica.h"
#include "InventoryDB.h"

#include "RakNet\ReplicaManager.h"
#include "Worlds.h"
#include "WorldServer.h"

#include "Logger.h"
#include "UtfConverter.h"

#include <map>

NPCObject::NPCObject(long long objid, ulong lot, long long spawnerObjID, bool isVendor, bool addScript) {
	this->objid = objid;
	this->name = L"";
	this->LOT = lot;

	this->spawnerObjIDFlag = true;
	this->spawnerObjID = spawnerObjID;

	this->spawnerNodeIDFlag = true;
	this->spawnerNodeID = 0;

	this->serFlag1 = true;

	this->addComponent(new Component3()); // SimplePhysics
	this->addComponent(new Component7()); // Destructible
	this->addComponent(new Component17()); // Inventory
	//if (isVendor) { this->addComponent(new Component16()); this->isNPCVendor = true; } // Vendor	
	this->addComponent(new Component16());
	//if (addScript) { this->addComponent(new Component5()); this->isScriptAdded = true; } // Script
	this->addComponent(new Component2()); // Render

	createLOTInfo(lot);
}

void NPCObject::createLOTInfo(int lot) {
	switch (lot) {
	case 4968:
		{
		Component3 *c3 = this->getComponent3();

		COMPONENT3_POSITION pos = c3->getPosition();
		pos.posX = 192.54959106445312;
		pos.posY = 259.0438232421875;
		pos.posZ = -561.6395263671875;
		c3->setPosition(pos);

		COMPONENT3_ROTATION rot = c3->getRotation();
		rot.rotX = 0.0;
		rot.rotY = 0.7923046946525574;
		rot.rotZ = 0.0;
		rot.rotW = 0.6101256608963013;
		c3->setRotation(rot);

		COMPONENT7_DATA3 data3 = COMPONENT7_DATA3();
		data3.d1 = 0; data3.d2 = 0; data3.d3 = 0; data3.d4 = 0; data3.d5 = 0; data3.d6 = 0; data3.d7 = 0; data3.d8 = 0; data3.d9 = 0;

		Component7 *c7 = this->getComponent7();
		c7->setData3(data3);

		COMPONENT7_DATA4 d4 = c7->getData4();
		d4.health = 1;
		d4.maxHealthN = 1.0;
		d4.maxHealth = 1.0;

		c7->getData4_1Ref()->push_back(-1);

		c7->setData4(d4);
		c7->setData4_4_1(true);
		c7->setData5(false);

		Component17 *c17 = this->getComponent17();
		c17->equipItem(288300744895889716, 4924, 0, 0, true);
		c17->setData2(0);
		}
		break;
	/*case 12261:
		{
		Component17 *c17 = this->getComponent17();
		auto itemID = ObjectsTable::createObject(10154);
		c17->equipItem(itemID, 10154, 0, 0, true);
		c17->setData2(0);
		break;
		}*/
	}
}

NPCObject::~NPCObject() {
	this->deleteComponents();
	ObjectsManager::unregisterObject(this);
	WorldServer::getRM()->DereferencePointer(this);
}

long long NPCObject::getObjectID() {
	return this->objid;
}

long long NPCObject::getSpawnerObjID() {
	return this->spawnerObjID;
}

ulong NPCObject::getLOT() {
	return this->LOT;
}

// --- Components ---
Component3 *NPCObject::getComponent3(){ return (Component3 *) this->getComponent(3); }
Component7 *NPCObject::getComponent7(){ return (Component7 *) this->getComponent(7); }
Component17 *NPCObject::getComponent17(){ return (Component17 *) this->getComponent(17); }
Component5 *NPCObject::getComponent5(){ return (Component5 *) this->getComponent(5); }
Component2 *NPCObject::getComponent2(){ return (Component2 *) this->getComponent(2); }

void NPCObject::doCreation(SystemAddress address) {
	this->clientIP = address;

	this->create(address);
}

void NPCObject::create(SystemAddress address) {
	WorldServer::getRM()->Construct(this, false, address, false);
	this->isCreated = true;
}

void NPCObject::doSerialization(SystemAddress address){
	if (!isCreated) {
		Logger::log("WRLD", "RMNPC", "NPC [objid:" + std::to_string(this->objid) + "] has not been created!", LOG_WARNING);
	}
	else {
		this->serialize(address);
	}
}

void NPCObject::serialize(SystemAddress address) {
	WorldServer::getRM()->SignalSerializeNeeded(this, address, false);
}

void NPCObject::serialize() {
	this->doSerialization(this->clientIP);
}

void NPCObject::doDestruction(SystemAddress address) {
	if (!isCreated) {
		Logger::log("WRLD", "RMNPC", "NPC [objid:" + std::to_string(this->objid) + "] has not been created!", LOG_WARNING);
	}
	else {
		isCreated = false;

		this->destruct(address);
	}
}

void NPCObject::destruct(SystemAddress address) {
	WorldServer::getRM()->Destruct(this, address, false);
}

void NPCObject::destruct() {
	if (isCreated) {
		this->doDestruction(this->clientIP);
	}
}