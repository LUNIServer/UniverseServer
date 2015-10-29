#pragma once
#include "RakNet\RakNetTypes.h"

#include "ReplicaData.h"
#include "ReplicaObject.h"
//#include "AccountsDB.h"
#include "World.h"

#include <unordered_map>

class Worlds{
public:
	static bool loadWorld(SystemAddress address, ZoneId zone, COMPONENT1_POSITION pos, unsigned short instance = 0, unsigned long clone = 0);
};

class Instances{
public:
	//Registers the instance for this address, clears up sessions if necessary
	static int registerInstance(SystemAddress address);
};

class ObjectsManager{
	static std::unordered_map<long long, ReplicaObject *> objects;
	
	
public:
	static void registerObject(ReplicaObject * object);
	static void unregisterObject(ReplicaObject * object);
	static ReplicaObject * getObjectByID(long long objid);

	//This initiates creation
	static void create(long long objid);
	static void serialize(long long objid);
	static void destruct(long long objid);
	static void create(ReplicaObject * object);
	static void serialize(ReplicaObject * object);
	static void destruct(ReplicaObject * object);

	//This manages player
	static void clientLeaveWorld(long long objid, SystemAddress addr);
	static void clientJoinWorld(ReplicaObject * player, SystemAddress addr);
};