#include "WorldObjectsDB.h"
#include "Logger.h"
#include "UtfConverter.h"

ulonglong NPCTable::registerNPC(NPCObject *object){
	std::stringstream str;
	Component3 *c3 = object->getComponent3();
	COMPONENT3_POSITION pos = c3->getPosition();
	COMPONENT3_ROTATION rot = c3->getRotation();
	str << "INSERT INTO `luni`.`world_objects` (`id`, `name`, `objectID`, `world`, `posX`, `posY`, `posZ`, `rotX`, `rotY`, `rotZ`, `rotW`) VALUES (NULL, '" << UtfConverter::ToUtf8(object->name) << "', '" << object->objid << "', '" << (int)object->world.zone << "', '" << pos.posX << "', '" << pos.posY << "', '" << pos.posZ << "', '" << rot.rotX << "', '" << rot.rotY << "', '" << rot.rotZ << "', '" << rot.rotW << "');";
	Database::Query(str.str());
	ulonglong id = mysql_insert_id(Database::getConnection());
	return id;
}

std::vector<NPCInfo*> NPCTable::getNPCs(){
	std::vector<NPCInfo*> query;
	std::stringstream str;
	str << "SELECT `name`, `objectID`, `world`, `posX`, `posY`, `posZ`, `rotX`, `rotY`, `rotZ`, `rotW` FROM `world_objects` WHERE 1;";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return query;
	}
	auto num = mysql_num_rows(qr);
	if (num == NULL || num == 0){
		return query;
	}
	else{
		query.reserve((unsigned int)num);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr)){
			NPCInfo *npcInfo = new NPCInfo();
			if (row[0] != NULL) {
				std::string str = row[0];
				npcInfo->name = UtfConverter::FromUtf8(str);
			}
			npcInfo->objID = std::stoll(row[1]);
			npcInfo->zone = std::stoi(row[2]);
			npcInfo->posX = std::stof(row[3]);
			npcInfo->posY = std::stof(row[4]);
			npcInfo->posZ = std::stof(row[5]);
			npcInfo->rotX = std::stof(row[6]);
			npcInfo->rotY = std::stof(row[7]);
			npcInfo->rotZ = std::stof(row[8]);
			npcInfo->rotW = std::stof(row[9]);
			query.push_back(npcInfo);
		}
		return query;
	}
}

uint NPCTable::getLOT(long long objID){
	std::stringstream str;
	str << "SELECT `template` FROM `objects` WHERE `objectid` = '" << objID << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return -1;
	}
	if (mysql_num_rows(qr) > 0){
		auto row = mysql_fetch_row(qr);
		long lot = std::stol(row[0]);
		return lot;
	}
	return -1;
}

long long NPCTable::getObjID(ulonglong id){
	std::stringstream str;
	str << "SELECT `objectID` FROM `world_objects` WHERE `id` = '" << id << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return -1;
	}
	if (mysql_num_rows(qr) > 0){
		auto row = mysql_fetch_row(qr);
		long long objID = std::stoll(row[0]);
		return objID;
	}
	return -1;
}

void NPCTable::deleteNPC(ulonglong id){
	std::stringstream eqqr;
	eqqr << "DELETE FROM `objects` WHERE `objectID`='" << NPCTable::getObjID(id) << "';";
	Database::Query(eqqr.str());
	std::stringstream eqqr2;
	eqqr2 << "DELETE FROM `world_objects` WHERE `id`='" << id << "';";
	Database::Query(eqqr2.str());
}

NPCInfo getNPC(long long objID){
	NPCInfo *npcInfo = new NPCInfo();
	std::stringstream str;
	str << "SELECT `name`, `world`, `posX`, `posY`, `posZ`, `rotX`, `rotY`, `rotZ`, `rotW` FROM `world_objects` WHERE `objectID` = '" << objID << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return *npcInfo;
	}
	if (mysql_num_rows(qr) > 0){
		auto row = mysql_fetch_row(qr);
		if (row[0] != NULL) {
			std::string str = row[0];
			npcInfo->name = UtfConverter::FromUtf8(str);
		}
		npcInfo->objID = objID;
		npcInfo->zone = std::stoi(row[1]);
		npcInfo->posX = std::stof(row[2]);
		npcInfo->posY = std::stof(row[3]);
		npcInfo->posZ = std::stof(row[4]);
		npcInfo->rotX = std::stof(row[5]);
		npcInfo->rotY = std::stof(row[6]);
		npcInfo->rotZ = std::stof(row[7]);
		npcInfo->rotW = std::stof(row[8]);
		return *npcInfo;
	}
	return *npcInfo;
}

ulonglong NPCTable::getID(long long objID){
	std::stringstream str;
	str << "SELECT `id` FROM `world_objects` WHERE `objectID` = '" << objID << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return -1;
	}
	if (mysql_num_rows(qr) > 0){
		auto row = mysql_fetch_row(qr);
		ulonglong id = std::stoull(row[0]);
		return id;
	}
	return -1;
}