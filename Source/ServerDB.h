#pragma once
#include "RakNet\RakNetTypes.h"

class InstancesTable{
public:
	//Registers a server instance on this ip&port, returns the instanceid
	static int registerInstance(SystemAddress addr);
	static int getInstanceId(SystemAddress addr);
	static void unregisterInstance(SystemAddress addr);
};