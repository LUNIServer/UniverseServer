#pragma once
#include <vector>

struct InventoryItem{
	long long objid = 0;
	long slot = 0;
	int qnt = 1;
	bool linked = false;
};

struct ObjectInfo{
	long long objid = 0;
	long lot = 0;
	long long spawnerid = 0;
	ObjectInfo(long long id, long t, long long s){
		this->objid = id;
		this->lot = t;
		this->spawnerid = s;
	}
};

struct RocketInfo{
	long nose_cone_template = 0;
	long cockpit_template = 0;
	long engine_template = 0;
	RocketInfo(long cone, long cockpit, long engine){
		this->nose_cone_template = cone;
		this->cockpit_template = cockpit;
		this->engine_template = engine;
	}
};

class InventoryTable{
private:
	static void updateSlotOfItem(long long objid, long long charid, unsigned long newslot);
public:
	static bool moveItemToSlot(long long objid, long long charid, unsigned long slot);
	static unsigned long getSlotFromItem(long long objid, long long charid);
	static long long getItemFromSlot(long long charid, unsigned long slot);
	static void deleteInventory(long long charid);
	static void insertItem(long long charid, long long objid, unsigned long qnt, unsigned long slot, bool linked);
	static void deleteItem(long long charid, long long objid);
	static std::vector<InventoryItem> getItems(long long charid);
};

class ObjectsTable{
public:
	//Get the LOT of an object
	static long getTemplateOfItem(long long objid);

	static ObjectInfo getItemInfo(long long objid);
	//Create an object with the specified LOT
	static long long createObject(long lot);
	static void deletObject(long long objid);

	//Custom Objects:
	//Rocket (6416)
	static RocketInfo getRocketInfo(long long objid);
};

class EquipmentTable{
public:
	static std::vector<long long> getItems(long long charid);
	static void equipItem(long long charid, long long objectid);
	static void unequipItem(long long charid, long long objectid);
	static void deleteEquipment(long long charid);
};