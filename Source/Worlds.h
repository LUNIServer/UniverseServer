#pragma once
#include "RakNet\RakNetTypes.h"
#include "World.h"
#include "ReplicaData.h"
#include "ReplicaObject.h"

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
};