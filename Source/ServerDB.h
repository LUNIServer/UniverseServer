#pragma once
#include "RakNet\RakNetTypes.h"
#include "Database.h"

struct Availability{
	bool isAvailable;
	std::string message;
	Availability(bool isAvailable, std::string message = ""){
		this->isAvailable = isAvailable;
		this->message = message;
	}
};

class InstancesTable : public MySQLTable{
public:
	//Registers a server instance on this ip&port, returns the instanceid
	static int registerInstance(SystemAddress addr);
	static int getInstanceId(SystemAddress addr);
	static void unregisterInstance(SystemAddress addr);

	std::string getName();
	void mapTable(std::unordered_map<std::string, compare<ColData *> *> * data);
};

class AvailabilityTable : public MySQLTable{
public:
	static Availability *getAvailability(SystemAddress address);
	static void setAvailability(SystemAddress address, bool isAvailable, std::string message = "");
private:
	static std::string getMessage();
};

class MySQL_Util_Functions{
public:
	static void flushCache();
};