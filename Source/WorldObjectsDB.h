#pragma once
#include <vector>
#include "Database.h"
#include "NPCObject.h"

struct NPCInfo{
	std::wstring name = L"";
	long long objID;
	short zone = 0;
	float posX;
	float posY;
	float posZ;
	float rotX;
	float rotY;
	float rotZ;
	float rotW;
	NPCInfo(){}
};

class NPCTable : public MySQLTable{
public:
	static ulonglong registerNPC(NPCObject *object);
	static void deleteNPC(ulonglong id);
	static std::vector<NPCInfo*> getNPCs();
	static uint getLOT(long long objID);
	static long long NPCTable::getObjID(ulonglong id);
	static NPCInfo getNPC(long long objID);
	static ulonglong getID(long long objID);
};