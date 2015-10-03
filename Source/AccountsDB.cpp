#include "AccountsDB.h"
#include "CharactersDB.h"
#include "Database.h"
#include "Encryption.h"
#include "Logger.h"
#include "World.h"

#include <sstream>
#include <iostream>
#include <algorithm>

unsigned int AccountsTable::getAccountID(std::string username){
	auto qr = Database::Query("SELECT `id` FROM `accounts` WHERE `name` = '" + username + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) return 0;
	auto ftc = mysql_fetch_row(qr);
	std::istringstream o(ftc[0]);
	unsigned int r;
	o >> r;
	return r;
}

unsigned long long AccountsTable::addAccount(std::string name, std::string password){
	unsigned int id = AccountsTable::getAccountID(name);
	if (id > 0) return 0; //Account name already exists
	std::string hpw = hashPassword(password); //Hash password
	Database::Query("INSERT INTO `accounts` (`id`, `name`, `password`, `email`, `ip`, `rank`, `numChars`, `frontChar`, `lastLog`, `activeSub`, `subTime`, `legoClub`, `locked`, `banned`, `loginTries`) VALUES (NULL, '" + name + "', '" + hpw + "', '', '127.0.0.1', '0', '0', '0', CURRENT_TIMESTAMP, '0', '', '', '0', '0', '0');");
	unsigned long long accountid = mysql_insert_id(Database::getConnection());
	return accountid;
}

bool AccountsTable::checkPassword(std::string password, unsigned int accountid){
	auto qr = Database::Query("SELECT `password` FROM `accounts` WHERE `id` = '" + std::to_string(accountid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) return false; //Actually this should NEVER happen
	auto r = mysql_fetch_row(qr);
	std::string pwhash = r[0];
	std::string testhash = hashPassword(password);
	if (pwhash == testhash) return true;
	return false;
}

AccountAccessInfo AccountsTable::getAccessInfo(unsigned int accountid){
	AccountAccessInfo accinf;
	auto qr = Database::Query("SELECT `locked`, `banned`, `loginTries` FROM `accounts` WHERE `id` = '" + std::to_string(accountid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) { //Actually this should NEVER happen
		accinf.loginTries = -1; //If it does set login tries to an impossible value
		return accinf;
	}
	auto r = mysql_fetch_row(qr);
	if (std::stoi(r[0]) == 1) accinf.locked = true;
	if (std::stoi(r[1]) == 1) accinf.banned = true;
	accinf.loginTries = std::stoi(r[2]);
	return accinf;
}

void AccountsTable::setAccessInfo(unsigned int accountid, AccountAccessInfo info){
	std::stringstream str;
	str << "UPDATE `accounts` SET `locked` = '";
	if (info.locked) str << "1"; else str << "0";
	str << "', `banned` = '";
	if (info.banned) str << "1"; else str << "0";
	str << "', `loginTries` = '";
	str << std::to_string(info.loginTries);
	str << "' WHERE `accounts`.`id` = ";
	str << std::to_string(accountid);
	str << ";";
}

/*CharacterCount AccountsTable::getCharCountInfo(unsigned int accountid){
	CharacterCount c;
	auto qr = Database::Query("SELECT `numChars`, `frontChar` FROM `accounts` WHERE `id` = '" + std::to_string(accountid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) {
		return c;
	}
	auto r = mysql_fetch_row(qr);
	c.count = std::stoi(r[0]);
	c.front = std::stoll(r[1]);
	if (c.count > 4) c.count = 0;
	if (c.front > 3) c.front = 0;
	return c;
}*/

bool AccountsTable::setFrontChar(long long charid){
	unsigned int accountid = CharactersTable::getAccountFromObjid(charid);
	if (accountid > 0){
		Logger::log("ACDB", "CHARS", "Setting char " + std::to_string(charid) + " as front char");
		Database::Query("UPDATE `accounts` SET `frontChar` = " + std::to_string(charid) + " WHERE `id` = '" + std::to_string(accountid) + "'");
		return true;
	}
	else{
		return false;
	}
}

void AccountsTable::unsetFrontChar(unsigned int accountid){
	Database::Query("UPDATE `accounts` SET `frontChar` = '0' WHERE `id` = '" + std::to_string(accountid) + "'");
}

long long AccountsTable::getFrontChar(unsigned int accountid){
	std::stringstream str;
	str << "SELECT `frontChar` FROM `accounts` WHERE `id` = '" << accountid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "searching account " + std::to_string(accountid), mysql_error(Database::getConnection()));
		return 0;
	}
	if (mysql_num_rows(qr) == 0){
		Logger::logError("ACDB", "MYSQL", "searching account " + std::to_string(accountid), "Account not found");
		return 0;
	}
	else{
		auto row = mysql_fetch_row(qr);
		return std::stoll(row[0]);
	}
}

std::string AccountsTable::getAccountName(unsigned int accountid){
	std::stringstream str;
	str << "SELECT `name` FROM `accounts` WHERE `id` = '" << accountid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "searching account " + std::to_string(accountid), mysql_error(Database::getConnection()));
		return "";
	}
	if (mysql_num_rows(qr) == 0){
		Logger::logError("ACDB", "MYSQL", "searching account " + std::to_string(accountid), "Account not found");
		return "";
	}
	else{
		auto row = mysql_fetch_row(qr);
		return row[0];
	}
}

unsigned char AccountsTable::getRank(unsigned int accountid){
	std::stringstream str;
	str << "SELECT `rank` FROM `accounts` WHERE `id` = '" << accountid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "getting rank " + std::to_string(accountid), mysql_error(Database::getConnection()));
		return 0;
	}
	if (mysql_num_rows(qr) == 0){
		Logger::logError("ACDB", "MYSQL", "getting rank " + std::to_string(accountid), "Account not found");
		return 0;
	}
	else{
		auto row = mysql_fetch_row(qr);
		return (unsigned char) std::stoul(row[0]);
	}
}

std::string AccountsTable::getName(){
	return "accounts";
}

void AccountsTable::mapTable(std::unordered_map<std::string, compare<ColData *> *> * data){
	Database::addColToMap(data, "id", new ColData("int(10) unsigned", false, "PRI", "NULL", "auto_increment"));
	Database::addColToMap(data, "name", new ColData("varchar(35)", false, "UNI", "NULL", ""));
	Database::addColToMap(data, "password", new ColData("varchar(64)", false, "", "NULL", ""));
	Database::addColToMap(data, "email", new ColData("varchar(30)", false, "", "NULL", ""));
	Database::addColToMap(data, "ip", new ColData("varchar(32)", false, "", "127.0.0.1", ""));
	Database::addColToMap(data, "rank", new ColData("smallint(1)", false, "", "0", ""));
	Database::addColToMap(data, "numChars", new ColData("tinyint(4)", false, "", "NULL", ""));
	Database::addColToMap(data, "frontChar", new ColData("bigint(20)", false, "", "NULL", ""));
	Database::addColToMap(data, "lastLog", new ColData("timestamp", false, "", "CURRENT_TIMESTAMP", ""));
	Database::addColToMap(data, "activeSub", new ColData("smallint(1)", false, "", "0", ""));
	Database::addColToMap(data, "subTime", new ColData("int(32)", false, "", "NULL", ""));
	Database::addColToMap(data, "legoClub", new ColData("smallint(1)", false, "", "NULL", ""));
	Database::addColToMap(data, "locked", new ColData("tinyint(4)", false, "", "NULL", ""));
	Database::addColToMap(data, "banned", new ColData("tinyint(4)", false, "", "NULL", ""));
	Database::addColToMap(data, "loginTries", new ColData("int(1)", false, "", "NULL", ""));
}

/*void AccountsTable::setNumChars(CharacterOwner o){
	Database::Query("UPDATE `accounts` SET `numChars` = " + std::to_string(o.charIndex) + " WHERE `id` = '" + std::to_string(o.accountid) + "'");
}*/

//std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher> SessionsTable::sessions = std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>();

//Connection
SessionInfo SessionsTable::connect(SystemAddress address){
	//sessions.insert(std::make_pair(address, SessionInfo(address)));
	SessionInfo session = SessionsTable::getClientSession(address);
	if (session.phase != SessionPhase::PHASE_NONE){
		Database::Query("DELETE FROM `sessions` WHERE `ipaddress` = '" + std::string(address.ToString()) + "';");
	}

	std::stringstream str;
	str << "INSERT INTO `sessions` (`ipaddress`) VALUES ('" << address.ToString() << "')";
	Database::Query(str.str());
	return SessionsTable::getClientSession(address);
}

bool SessionsTable::disconnect(SystemAddress address){
	/*auto fnd = sessions.find(address);
	if (fnd == sessions.end()) return false;
	sessions.erase(address);*/
	//TODO: Some kind of check if this server is responsible for the session
	std::stringstream str;
	str << "DELETE FROM `sessions` WHERE `ipaddress` = '" << address.ToString() << "'";
	Database::Query(str.str());
	return true;
}

SessionInfo SessionsTable::getClientSession(SystemAddress address){
	SessionInfo sinfo;
	std::stringstream str;
	str << "SELECT `ipaddress`, `sessionkey`, `phase`, `instanceid`, `accountid`, `charid`, `zoneid` FROM `sessions` WHERE `ipaddress` = '" << address.ToString() << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "getting session", mysql_error(Database::getConnection()));
		return sinfo;
	}
	if (mysql_num_rows(qr) == 0 || mysql_num_rows(qr) == NULL)
		return sinfo;
	else{
		MYSQL_ROW row = mysql_fetch_row(qr);
		sinfo.addr.SetBinaryAddress(row[0]);
		if (row[1] != NULL){
			sinfo.sessionkey = row[1];
		}
		sinfo.phase = static_cast<SessionPhase>(std::stoi(row[2]));
		if (row[3] != NULL){
			sinfo.instanceid = std::stoi(row[3]);
		}
		if (row[4] != NULL){
			sinfo.accountid = std::stoi(row[4]);
		}
		if (row[5] != NULL){
			sinfo.activeCharId = std::stoll(row[5]);
		}
		if (row[6] != NULL){
			sinfo.zone = std::stoi(row[6]);
		}
		return sinfo;
	}
	
	//auto fnd = sessions.find(address);
	//if (fnd == sessions.end()) return SessionInfo();
	//return fnd->second;
}

//Authentification
SessionInfo SessionsTable::login(SystemAddress address, unsigned int accountid, std::string key, int instanceid){
	SessionInfo s = SessionsTable::getClientSession(address);
	if (s.phase != SessionPhase::PHASE_NONE){
		s.phase = SessionPhase::PHASE_AUTHENTIFIED;
		s.accountid = accountid;
		std::stringstream str;
		str << "UPDATE `sessions` SET `phase` = '" << std::to_string((unsigned char)s.phase) << "', `instanceid` = '" << std::to_string(instanceid) << "', `accountid` = '" << std::to_string(s.accountid) << "', `sessionkey` = '" << key << "' WHERE `ipaddress` = '" << address.ToString() << "'";
		auto qr = Database::Query(str.str());
		if (qr == NULL){
			Logger::logError("ACDB", "MYSQL", "logging in", mysql_error(Database::getConnection()));
		}
	}
	return s;
	
	/*auto session = sessions.find(address);
	if (session == sessions.end()) return SessionInfo();

	SessionInfo info = session->second;
	info.phase = SessionPhase::PHASE_AUTHENTIFIED;
	info.accountid = accountid;

	sessions.at(address) = info;
	return info;*/
}

SessionInfo SessionsTable::logout(unsigned int accountid){
	SystemAddress addr = SessionsTable::findAccount(accountid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase > SessionPhase::PHASE_CONNECTED){
			s.phase = SessionPhase::PHASE_CONNECTED;
			s.accountid = 0;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((unsigned char)s.phase) << "', `accountid` = '" << std::to_string(s.accountid) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				Logger::logError("ACDB", "MYSQL", "logging out", mysql_error(Database::getConnection()));
			}
		}

		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.accountid = 0;
		info.phase = SessionPhase::PHASE_CONNECTED;
		sessions.at(addr) = info;*/
		return s;
	}
	else{
		return SessionInfo();
	}
}

SystemAddress SessionsTable::findAccount(unsigned int accountid){
	std::stringstream str;
	str << "SELECT `ipaddress` FROM `sessions` WHERE `accountid` = '" << std::to_string(accountid) << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "finding session for accountid " + std::to_string(accountid), mysql_error(Database::getConnection()));
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		if (mysql_num_rows(qr) > 0){
			SystemAddress addr;
			auto row = mysql_fetch_row(qr);
			addr.SetBinaryAddress(row[0]);
			Logger::log("ACDB", "DEBUG", std::string(row[0]) + " loaded as " + std::string(addr.ToString()) + " (p:" + std::to_string(addr.port) + ")", LOG_DEBUG);
			return addr;
		}
		else{
			return UNASSIGNED_SYSTEM_ADDRESS;
		}
		
	}
	/*std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>::iterator it = std::find_if(sessions.begin(), sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
		return o.second.accountid == accountid;
	});
	if (it == sessions.end()){
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		return it->first;
	}*/
}

//Character selection
SessionInfo SessionsTable::play(unsigned int accountid, long long charid){
	SystemAddress addr = SessionsTable::findAccount(accountid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase == SessionPhase::PHASE_AUTHENTIFIED){
			s.phase = SessionPhase::PHASE_PLAYING;
			s.activeCharId = charid;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((unsigned char)s.phase) << "', `charid` = '" << std::to_string(s.activeCharId) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				Logger::logError("ACDB", "MYSQL", "playing character", mysql_error(Database::getConnection()));
			}
		}
		return s;
		
		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.activeCharId = charid;
		info.phase = SessionPhase::PHASE_PLAYING;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

SessionInfo SessionsTable::quit(long long charid){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase > SessionPhase::PHASE_AUTHENTIFIED){
			s.phase = SessionPhase::PHASE_AUTHENTIFIED;
			s.worldJoin = 0;
			s.activeCharId = -1;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((unsigned char)s.phase) << "', `charid` = '" << std::to_string(s.activeCharId) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				Logger::logError("ACDB", "MYSQL", "quitting character", mysql_error(Database::getConnection()));
			}
		}
		return s;
		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.activeCharId = -1;
		info.worldJoin = 0;
		info.phase = SessionPhase::PHASE_AUTHENTIFIED;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

SystemAddress SessionsTable::findCharacter(long long charid){
	std::stringstream str;
	str << "SELECT `ipaddress` FROM `sessions` WHERE `charid` = '" << std::to_string(charid) << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "finding character " + std::to_string(charid), mysql_error(Database::getConnection()));
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		if (mysql_num_rows(qr) > 0){
			SystemAddress addr;
			auto row = mysql_fetch_row(qr);
			addr.SetBinaryAddress(row[0]);
			return addr;
		}
		else{
			return UNASSIGNED_SYSTEM_ADDRESS;
		}
		
	}
	/*std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>::iterator it = std::find_if(sessions.begin(), sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
		return o.second.activeCharId == charid;
	});
	if (it == sessions.end()){
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		return it->first;
	}*/
}

std::vector<SessionInfo> SessionsTable::getClientsInWorld(unsigned short zoneid){
	std::vector<SessionInfo> query;
	std::stringstream str;
	str << "SELECT `ipaddress`, `sessionkey`, `phase`, `instanceid`, `accountid`, `charid`, `zoneid` FROM `sessions` WHERE `zoneid` = '" << std::to_string(zoneid) << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "getting clients in " + (ZoneId)zoneid, mysql_error(Database::getConnection()));
		return query;
	}
	auto num = mysql_num_rows(qr);
	if (num == NULL || num == 0)
		return query;
	else{
		query.reserve((unsigned int) num);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr)){
			SessionInfo sinfo;
			sinfo.addr.SetBinaryAddress(row[0]);
			sinfo.phase = static_cast<SessionPhase>(std::stoi(row[2]));
			if (row[3] != NULL){
				sinfo.instanceid = std::stoi(row[3]);
			}			
			if (row[4] != NULL){
				sinfo.accountid = std::stoi(row[4]);
			}
			if (row[5] != NULL){
				sinfo.activeCharId = std::stoll(row[5]);
			}
			if (row[6] != NULL){
				sinfo.zone = std::stoi(row[6]);
			}
			query.push_back(sinfo);
		}
		return query;
	}
	
	/*std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>::iterator it = std::find_if(sessions.begin(), sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
		return o.second.zone == zoneid;
	});
	
	while (it != sessions.end()){
		query.push_back(it->second);
		++it;
		it = std::find_if(it, sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
			return o.second.zone == zoneid;
		});
	}
	return query;*/
}

std::vector<SessionInfo> SessionsTable::getClientsInInstance(int instanceid){
	//TODO: implement instances into database
	std::vector<SessionInfo> query;
	std::stringstream str;
	str << "SELECT `ipaddress`, `sessionkey`, `phase`, `accountid`, `charid`, `zoneid` FROM `sessions` WHERE `instanceid` = '" << std::to_string(instanceid) << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "getting clients in instance #" + std::to_string(instanceid), mysql_error(Database::getConnection()));
		return query;
	}
	auto num = mysql_num_rows(qr);
	if (num == NULL || num == 0)
		return query;
	else{
		query.reserve((unsigned int)num);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr)){
			SessionInfo sinfo;
			sinfo.addr.SetBinaryAddress(row[0]);
			sinfo.phase = static_cast<SessionPhase>(std::stoi(row[2]));
			if (row[3] != NULL){
				sinfo.accountid = std::stoi(row[3]);
			}
			if (row[4] != NULL){
				sinfo.activeCharId = std::stoll(row[4]);
			}
			if (row[5] != NULL){
				sinfo.zone = std::stoi(row[5]);
			}
			query.push_back(sinfo);
		}
		return query;
	}
}

void SessionsTable::setInstanceId(unsigned int accountid, int instanceid){
	SystemAddress addr = SessionsTable::findAccount(accountid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		std::stringstream str;
		str << "UPDATE `sessions` SET `instanceid` = '" << std::to_string(instanceid) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
		auto qr = Database::Query(str.str());
	}
}

//Worlds
SessionInfo SessionsTable::enter(long long charid, unsigned short zoneId){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase == SessionPhase::PHASE_PLAYING){
			s.phase = SessionPhase::PHASE_INWORLD;
			s.zone = zoneId;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((unsigned char)s.phase) << "', `zoneid` = '" << std::to_string(s.zone) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				Logger::logError("ACDB", "MYSQL", "entering world", mysql_error(Database::getConnection()));
			}
		}
		return s;

		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.zone = zoneId;
		info.worldJoin++;
		info.phase = SessionPhase::PHASE_INWORLD;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

SessionInfo SessionsTable::leave(long long charid){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase > SessionPhase::PHASE_PLAYING){
			s.phase = SessionPhase::PHASE_PLAYING;
			s.zone = 0;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((unsigned char)s.phase) << "', `zoneid` = '" << std::to_string(s.zone) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				Logger::logError("ACDB", "MYSQL", "leaving world", mysql_error(Database::getConnection()));
			}
		}
		return s;

		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.zone = 0;
		info.phase = SessionPhase::PHASE_PLAYING;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

unsigned int SessionsTable::count(){
	std::stringstream str;
	str << "SELECT `ipaddress` FROM `sessions`";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("ACDB", "MYSQL", "counting sessions", mysql_error(Database::getConnection()));
		return 0;
	}
	auto num = mysql_num_rows(qr);
	if (num == NULL)
		return 0;
	else{
		return (unsigned int) num;
	}
	//return sessions.size();
}

std::string SessionsTable::getName(){
	return "sessions";
}

void SessionsTable::mapTable(std::unordered_map<std::string, compare<ColData *> *> * data){
	Database::addColToMap(data, "sessionid", new ColData("int(20)", false, "PRI", "NULL", "auto_increment"));
	Database::addColToMap(data, "ipaddress", new ColData("varchar(21)", false, "", "", ""));
	Database::addColToMap(data, "phase", new ColData("smallint(6)", false, "", "1", ""));
	Database::addColToMap(data, "sessionkey", new ColData("varchar(33)", false, "", "", ""));
	Database::addColToMap(data, "instanceid", new ColData("int(11)", true, "", "NULL", ""));
	Database::addColToMap(data, "accountid", new ColData("int(10) unsigned", false, "", "0", ""));
	Database::addColToMap(data, "login_time", new ColData("timestamp", false, "", "CURRENT_TIMESTAMP", ""));
	Database::addColToMap(data, "charid", new ColData("bigint(20)", true, "", "NULL", ""));
	Database::addColToMap(data, "zoneid", new ColData("int(11)", false, "", "0", ""));
	Database::addColToMap(data, "cloneid", new ColData("int(10) unsigned", false, "", "0", ""));
}