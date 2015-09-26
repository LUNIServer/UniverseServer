#include "InventoryDB.h"
#include "Database.h"
#include "Logger.h"

#include <sstream>

void InventoryTable::updateSlotOfItem(long long objid, long long charid, unsigned long newslot){
	std::stringstream oi;
	oi << "UPDATE `inventory` SET `slot` = '" << newslot << "' WHERE `object` = " << objid << ";";
	Database::Query(oi.str());
}

bool InventoryTable::moveItemToSlot(long long objid, long long charid, unsigned long slot){
	unsigned long oldslot = InventoryTable::getSlotFromItem(objid, charid);
	if (oldslot != ~((unsigned long)0)){
		long long ot_objid = InventoryTable::getItemFromSlot(charid, slot);
		if (ot_objid > -1){
			InventoryTable::updateSlotOfItem(ot_objid, charid, oldslot);
		}
		InventoryTable::updateSlotOfItem(objid, charid, slot);
	}
	return true;
}

unsigned long InventoryTable::getSlotFromItem(long long objid, long long charid){
	std::stringstream sq;
	sq << "SELECT `slot` FROM `inventory` WHERE `owner` = '" << charid << "' AND `object` = '" << objid << "';";
	auto sqr = Database::Query(sq.str());
	if (mysql_num_rows(sqr) > 0){
		auto row = mysql_fetch_row(sqr);
		unsigned long slot = std::stoi(row[0]);
		return slot;
	}
	else{
		return ~((unsigned long)0); //return max as negative value
	}
}

long long InventoryTable::getItemFromSlot(long long charid, unsigned long slot){
	std::stringstream iq;
	iq << "SELECT `object` FROM `inventory` WHERE `owner` = '" << charid << "' AND `slot` = '" << slot << "';";
	auto iqr = Database::Query(iq.str());
	if (mysql_num_rows(iqr) > 0){
		auto ir = mysql_fetch_row(iqr);
		long long objid = std::stoll(ir[0]);
		return objid;
	}
	else{
		return -1; //-1 as return value
	}
}

void InventoryTable::deleteInventory(long long charid){
	std::stringstream eqqr;
	eqqr << "DELETE FROM `inventory` WHERE `owner`='" << charid << "';";
	Database::Query(eqqr.str());
}

void InventoryTable::insertItem(long long charid, long long objid, unsigned long qnt, unsigned long slot, bool linked){
	std::stringstream str;
	str << "INSERT INTO `inventory` (`owner`, `object`, `qnt`, `slot`, `linked`) VALUES('" << charid << "', '" << objid << "', '" << std::to_string(qnt) << "', '" << std::to_string(slot) << "', '";
	if (linked) str << "1"; else str << "0";
	str << "');";
	Database::Query(str.str());
}

void InventoryTable::deleteItem(long long charid, long long objid){
	Database::Query("DELETE FROM `inventory` WHERE `owner` = '" + std::to_string(charid) + "' AND `object` = '" + std::to_string(objid) + "';");
}

std::vector<InventoryItem> InventoryTable::getItems(long long charid){
	std::string qr = "SELECT `object`, `qnt`, `slot`, `linked` FROM `inventory` WHERE `owner` = " + std::to_string(charid);
	auto qrr = Database::Query(qr);
	unsigned long long num = mysql_num_rows(qrr);
	std::vector<InventoryItem> items;
	items.reserve((int) num);
	MYSQL_ROW row;
	while (row = mysql_fetch_row(qrr)) {
		InventoryItem i;
		i.objid = std::stoll(row[0]);
		i.qnt = std::stoi(row[1]);
		i.slot = std::stol(row[2]);
		if (std::stoi(row[3]) == 1) i.linked = true;
		items.push_back(i);
	}
	return items;
}

long ObjectsTable::getTemplateOfItem(long long objid){
	std::stringstream str;
	str << "SELECT `template` FROM `objects` WHERE `objectid` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return -1;
		Logger::logError("IVDB", "MYSQL", "getting LOT", mysql_error(Database::getConnection()));
	}
	if (mysql_num_rows(qr) > 0){
		auto row = mysql_fetch_row(qr);
		long lot = std::stol(row[0]);
		return lot;
	}
	return -1;
}

ObjectInfo ObjectsTable::getItemInfo(long long objid){
	std::stringstream str;
	str << "SELECT `template`, `spawnid` FROM `objects` WHERE `objectid` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		
	}else{
		if (mysql_num_rows(qr) > 0){
			auto row = mysql_fetch_row(qr);
			long lot = std::stol(row[0]);
			long long spawnid = -1;
			if (row[1] != NULL){
				spawnid = std::stoll(row[1]);
			}
			return ObjectInfo(objid, lot, spawnid);
		}
	}
	return ObjectInfo(0, 0, 0);
}

long long ObjectsTable::createObject(long lot){
	std::stringstream str;
	str << "INSERT INTO `luni`.`objects` (`objectid`, `template`) VALUES(NULL, '" << lot << "');";
	Database::Query(str.str());
	long long objid = mysql_insert_id(Database::getConnection());
	return objid;
}

void ObjectsTable::deleteObject(long long objid){
	Database::Query("DELETE FROM `objects` WHERE `objectid` = '" + std::to_string(objid) + "';");
}

RocketInfo ObjectsTable::getRocketInfo(long long objid){
	std::stringstream str;
	str << "SELECT `nose_cone_template`, `cockpit_template`, `engine_template` FROM `objects` WHERE `objectid` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (mysql_num_rows(qr) > 0){
		auto row = mysql_fetch_row(qr);
		long cone = std::stol(row[0]);
		long cockpit = std::stol(row[1]);
		long engine = std::stol(row[2]);
		return RocketInfo(cone, cockpit, engine);
	}
	return RocketInfo(0,0,0);
}

std::vector<long long> EquipmentTable::getItems(long long charid){
	auto qr = Database::Query("SELECT `object` FROM `equipment` WHERE `owner` = '" + std::to_string(charid) + "';");
	
	unsigned int numrows = (uint)mysql_num_rows(qr);
	std::vector<long long> items;
	items.reserve(numrows);
	for (unsigned int k = 0; k < numrows; k++){
		auto ftc = mysql_fetch_row(qr);
		long long itemid = std::stoll(ftc[0]);
		items.push_back(itemid);
	}
	return items;
}

void EquipmentTable::equipItem(long long charid, long long objid){
	std::stringstream eqqr;
	eqqr << "INSERT INTO `luni`.`equipment` (`id`, `owner`, `object`) VALUES(NULL, '" << charid << "', " << objid << ");";
	Database::Query(eqqr.str());
}

void EquipmentTable::unequipItem(long long charid, long long objid){
	std::stringstream eqqr;
	eqqr << "DELETE FROM `luni`.`equipment` WHERE `owner`='" << charid << "' AND `object`='" << objid << "';";
	Database::Query(eqqr.str());
}

void EquipmentTable::deleteEquipment(long long charid){
	std::stringstream eqqr;
	eqqr << "DELETE FROM `equipment` WHERE `owner`='" << charid << "';";
	Database::Query(eqqr.str());
}