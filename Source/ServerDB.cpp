#include "Database.h"
#include "ServerDB.h"

#include "Logger.h"

#include <sstream>
#include <iostream>

int InstancesTable::registerInstance(SystemAddress addr){
	std::stringstream str;
	str << "INSERT INTO `instances` (`server_address`) VALUES ('" << addr.ToString() << "');";
	Database::Query(str.str());
	return (int)mysql_insert_id(Database::getConnection());
}

int InstancesTable::getInstanceId(SystemAddress addr){
	std::stringstream str;
	str << "SELECT `instanceid` FROM `instances` WHERE `server_address` = '" << addr.ToString() << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("SVDB", "MYSQL", "getting instance id", mysql_error(Database::getConnection()));
		return -1;
	}
	else{
		if (mysql_num_rows(qr) == 0){
			return -1;
		}
		else{
			auto row = mysql_fetch_row(qr);
			return std::stoi(row[0]);
		}
	}
}

void InstancesTable::unregisterInstance(SystemAddress addr){
	std::stringstream str;
	str << "DELETE FROM `instances` WHERE `server_address` = '" << addr.ToString() << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("SVDB", "MYSQL", "unregistering instance", mysql_error(Database::getConnection()));
	}
}

std::string InstancesTable::getName(){
	return "instances";
}

void InstancesTable::mapTable(std::unordered_map<std::string, compare<ColData *> *> * data){
	Database::addColToMap(data, "instanceid", new ColData("int(11)", false, "PRI", "NULL", "auto_increment"));
	Database::addColToMap(data, "server_address", new ColData("varchar(22)", false, "", "NULL", ""));
}

Availability *AvailabilityTable::getAvailability(SystemAddress address){
	Availability *av = new Availability(false);
	std::stringstream str;
	str << "SELECT `available` FROM `availability` WHERE `server_address` = '" << address.ToString() << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return av;
	}
	else{
		if (mysql_num_rows(qr) == 0){
			return av;
		}
		else{
			auto row = mysql_fetch_row(qr);
			bool flag = (std::stoi(row[0]) != 0);
			if (!flag){
				av->message = AvailabilityTable::getMessage();
				return av;
			}
			else{
				av->isAvailable = true;
				return av;
			}
		}
	}
}

void AvailabilityTable::setAvailability(SystemAddress address, bool isAvailable, std::string message){
	std::stringstream eqqr;
	if (message == ""){
		eqqr << "UPDATE `availability` SET `available`='" << (int)isAvailable << "' WHERE `server_address` = '" << address.ToString() << "';";
	}
	else{
		eqqr << "UPDATE `availability` SET `available`='" << (int)isAvailable << "',`message`='" << message << "' WHERE `server_address` = '" << address.ToString() << "';";
	}
	Database::Query(eqqr.str());
}

std::string AvailabilityTable::getMessage(){
	std::stringstream str;
	str << "SELECT `message` FROM `availability` WHERE 1;";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return "";
	}
	else{
		if (mysql_num_rows(qr) == 0){
			return "";
		}
		else{
			auto row = mysql_fetch_row(qr);
			return (std::string) row[0];
		}
	}
}

void MySQL_Util_Functions::flushCache() {
	std::stringstream eqqr;
	eqqr << "RESET QUERY CACHE;";
	// Last: 'FLUSH QUERY CACHE;'
	Database::Query(eqqr.str());
}