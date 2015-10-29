#pragma once
#include "RakNet\RakNetTypes.h"
#include "Database.h"

class InstancesTable : public MySQLTable{
public:
	//Registers a server instance on this ip&port, returns the instanceid
	static int registerInstance(SystemAddress addr);
	static int getInstanceId(SystemAddress addr);
	static void unregisterInstance(SystemAddress addr);

	std::string getName();
	void mapTable(std::unordered_map<std::string, compare<ColData *> *> * data);
};