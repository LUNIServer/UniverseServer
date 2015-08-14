#pragma once
#include "SUtil\Common.h"
#include "World.h"

#include <string>

enum ObjectType{
	NONE = -1,
	LaunchPad,
	PostBox,
	Binoculars,
	Minigame,
	Plaque,
	Portal,
	Vault,
	WaterPot,
	Machine,
	Object
};

struct ObjectInformation{
	long long objid;
	ZoneId zone;
	ObjectType type;
	std::string info;
	ObjectInformation(long long id, ZoneId z, ObjectType t, std::string s = ""){
		this->objid = id;
		this->zone = z;
		this->type = t;
		this->info = s;
	}
	ObjectInformation(){
		this->objid = 0;
		this->zone = ZoneId::NO_ZONE;
		this->type = ObjectType::NONE;
		this->info = "";
	}
};

std::string getSpeedchatMessage(ushort speedchatid);
ObjectInformation getObjectInformation(long long objectid);
std::string getObjectDescription(ObjectInformation obj, long long objid = -1);
ZoneId getLaunchPadTarget(ObjectInformation obj);