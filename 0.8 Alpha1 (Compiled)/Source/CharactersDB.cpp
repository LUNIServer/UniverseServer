#include "CharactersDB.h"
#include "Database.h"
#include "Logger.h"

#include <sstream>
#include <iostream>

long long CharactersTable::getObjidFromCharacter(std::string name){
	std::stringstream iq;
	iq << "SELECT `objectID` FROM `characters` WHERE `name` = '" << name << "';";
	std::string qrs = iq.str();
	auto iqr = Database::Query(qrs);
	if (mysql_num_rows(iqr) > 0){
		auto ir = mysql_fetch_row(iqr);
		long long objid = std::stoll(ir[0]);
		return objid;
	}
	else{
		return -1; //-1 as return value
	}
}

unsigned int CharactersTable::getAccountFromObjid(long long objid){
	auto qr = Database::Query("SELECT `accountID` FROM `characters` WHERE `objectID` = '" + std::to_string(objid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) {
		return 0;
	}
	auto r = mysql_fetch_row(qr);
	return std::stoi(r[0]);
}

std::vector<long long> CharactersTable::getCharacters(unsigned int accountid){
	std::vector<long long> chars;
	auto qr = Database::Query("SELECT `id`, `objectID` FROM `characters` WHERE `accountID` = " + std::to_string(accountid) + " LIMIT 4"); // Load chars from MySQL DB
	MYSQL_ROW row;
	chars.reserve(4);
	if (qr == NULL){
		return chars;
		Logger::logError("CHDB", "MYSQL", "getting characters", mysql_error(Database::getConnection()));
	}
	while (row = mysql_fetch_row(qr)) {
		std::stringstream s(row[1]);
		long long cid;
		s >> cid;
		ushort id = std::stoi(row[0]);
		chars.push_back(cid);
	}
	return chars;
}

/*ListCharacterInfo CharactersTable::getCharacterInfo(CharacterOwner owner){
	std::stringstream qrs;
	qrs << "SELECT ";
	qrs << "`objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, ";
	qrs << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, ";
	qrs << "`lastZoneId`, `mapInstance`, `mapClone`, `x`, `y`, `z` ";
	qrs << "FROM `characters` WHERE `id` = '" << std::to_string(owner.charIndex) << "' AND `accountID` = '" << std::to_string(owner.accountid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);
	return CharactersTable::getCharacterInfo(qr);
}*/

ListCharacterInfo CharactersTable::getCharacterInfo(long long objid){
	std::stringstream qrs;
	qrs << "SELECT ";
	qrs << "`objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, ";
	qrs << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, ";
	qrs << "`lastZoneId`, `mapInstance`, `mapClone`, `x`, `y`, `z` ";
	qrs << "FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);
	return CharactersTable::getCharacterInfo(qr);
}

ListCharacterInfo CharactersTable::getCharacterInfo(std::string name){
	std::stringstream qrs;
	qrs << "SELECT ";
	qrs << "`objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, ";
	qrs << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, ";
	qrs << "`lastZoneId`, `mapInstance`, `mapClone`, `x`, `y`, `z` ";
	qrs << "FROM `characters` WHERE `name` = '" << name << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);
	return CharactersTable::getCharacterInfo(qr);
}

ListCharacterInfo CharactersTable::getCharacterInfo(MYSQL_RES *res){
	ListCharacterInfo i;
	if (mysql_num_rows(res) == 0) {
		i.info.objid = 0;
		return i;
	}
	else {
		//Info
		auto r = mysql_fetch_row(res);
		i.info.objid = std::stoll(r[0]);
		i.info.name = r[1];
		i.info.unapprovedName = r[2];
		if (std::stoi(r[3]) == 1) i.info.nameRejected = true;
		if (std::stoi(r[4]) == 1) i.info.isFreeToPlay = true;
		//Style
		i.style.shirtColor = std::stoul(r[5]);
		i.style.shirtStyle = std::stoul(r[6]);
		i.style.pantsColor = std::stoul(r[7]);
		i.style.hairStyle = std::stoul(r[8]);
		i.style.hairColor = std::stoul(r[9]);
		i.style.lh = std::stoul(r[10]);
		i.style.rh = std::stoul(r[11]);
		i.style.eyebrows = std::stoul(r[12]);
		i.style.eyes = std::stoul(r[13]);
		i.style.mouth = std::stoul(r[14]);
		//Place
		i.lastPlace.zoneID = std::stoi(r[15]);
		i.lastPlace.mapInstance = std::stoi(r[16]);
		i.lastPlace.mapClone = std::stoul(r[17]);
		i.lastPlace.x = std::stof(r[18]);
		i.lastPlace.y = std::stof(r[19]);
		i.lastPlace.z = std::stof(r[20]);
	}
	return i;
}

bool CharactersTable::setCharactersPlace(long long objid, WorldPlace place){
	std::stringstream qr;
	qr <<  "UPDATE `characters` SET ";
	qr << "`lastZoneId` = '" << std::to_string(place.zoneID);
	qr << "', `mapInstance` = '" << std::to_string(place.mapInstance);
	qr << "', `mapClone` = '" << std::to_string(place.mapClone);
	qr << "', `x` = '" << std::to_string(place.x);
	qr << "', `y` = '" << std::to_string(place.y);
	qr << "', `z` = '" << std::to_string(place.z);
	qr << "' WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrs = qr.str();
	auto a = Database::Query(qrs);
	if (a == NULL){
		return false;
	}
	return true;
}

void CharactersTable::deleteCharacter(long long objid){
	std::stringstream query;
	query << "DELETE FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	Database::Query(query.str());
}

std::vector<unsigned char> CharactersTable::getCharacterIndices(unsigned int accountid){
	std::vector<unsigned char> chars;
	auto qr = Database::Query("SELECT `id`, `objectID` FROM `characters` WHERE `accountID` = " + std::to_string(accountid) + " LIMIT 4"); // Load chars from MySQL DB
	MYSQL_ROW row;
	while (row = mysql_fetch_row(qr)) {
		uchar id = std::stoi(row[0]);
		chars.push_back(id);
	}
	return chars;
}

long long CharactersTable::add(CharacterStyle style, unsigned int accountid, CharacterInfo names){
	std::stringstream query2;
	query2 << "INSERT INTO `characters` (`accountID`, `objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, ";
	query2 << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, `lastZoneId`) ";
	query2 << "VALUES('" << std::to_string(accountid) << "', NULL, '" << names.name << "', '" << names.unapprovedName << "', '0', '0', '";
	query2 << std::to_string(style.shirtColor) << "', '" << std::to_string(style.shirtStyle) << "', '";
	query2 << std::to_string(style.pantsColor) << "', '" << std::to_string(style.hairStyle) << "', '" << std::to_string(style.hairColor) << "', '";
	query2 << std::to_string(style.lh) << "', '" << std::to_string(style.rh) << "', '" << std::to_string(style.eyebrows) << "', '";
	query2 << std::to_string(style.eyes) << "', '" << std::to_string(style.mouth) << "', '1000'); ";
	Database::Query(query2.str());
	return mysql_insert_id(Database::getConnection());
}

bool CharactersTable::unapprovedNameExists(std::string unapprovedname){
	auto qr2 = Database::Query("SELECT `unapprovedName` FROM `characters` WHERE `unapprovedName` = '" + unapprovedname + "';");
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL) 
		return false;
	else
		return true;
}

void FriendsTable::requestFriend(long long sender, long long reciever){
	std::stringstream str;
	str << "INSERT INTO `friends` (`id`, `charida`, `charidb`) VALUES( NULL, '" << sender << "', '" << reciever << "');";
	Database::Query(str.str());
}

void FriendsTable::requestBestFriend(long long sender, long long reciever){
	std::stringstream str;
	str << "UPDATE `friends` SET `status` = 'BEST_FRIEND_REQUEST', ";
	str << "`charida` = '" << std::to_string(sender) << "', ";
	str << "`charidb` = '" << std::to_string(reciever) << "' ";
	str << "WHERE (" << "`charida` = '" << std::to_string(sender) << "' AND `charidb` = '" << std::to_string(reciever) << "') ";
	str << "OR  (" << "`charidb` = '" << std::to_string(sender) << "' AND `charida` = '" << std::to_string(reciever) << "');";
	Database::Query(str.str());
}

//This query ONLY regards the status in one direction, meaning it is only useful if the requester is known
std::string FriendsTable::getRequestStatus(long long sender, long long reciever){
	auto qr = Database::Query("SELECT `status` FROM `friends` WHERE `charida` = '" + std::to_string(sender) + "' AND `charidb` = '" + std::to_string(reciever) + "';");
	if (mysql_num_rows(qr) == 0 || mysql_num_rows(qr) == NULL){
		return "INVALID";
	}
	else{
		auto row = mysql_fetch_row(qr);
		return row[0];
	}
}

std::string FriendsTable::getFriendsStatus(long long charidx, long long charidy){
	std::stringstream str;
	str << "SELECT `status` FROM `friends` ";
	str << "WHERE (" << "`charida` = '" << std::to_string(charidx) << "' AND `charidb` = '" << std::to_string(charidy) << "') ";
	str << "OR  (" << "`charidb` = '" << std::to_string(charidx) << "' AND `charida` = '" << std::to_string(charidy) << "');";
	auto qr = Database::Query(str.str());
	if (mysql_num_rows(qr) == 0 || mysql_num_rows(qr) == NULL){
		return "INVALID";
	}
	else{
		auto row = mysql_fetch_row(qr);
		return row[0];
	}
}

std::vector<long long> FriendsTable::getFriends(long long charobjid){
	std::string qr = "SELECT `charida`, `charidb` FROM `friends` WHERE (`charida` = '" + std::to_string(charobjid) + "' OR `charidb` = '" + std::to_string(charobjid) + "') AND `status` IN ('FRIENDS', 'BEST_FRIEND_REQUEST', 'ACCEPTED');";
	auto qr2 = Database::Query(qr);
	std::vector<long long> friends;	
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return friends;
	else{
		unsigned int numrows = (unsigned int) mysql_num_rows(qr2);
		friends.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			long long charb = std::stoll(row[1]);
			long long fr = 0;
			if (chara == charobjid){
				fr = charb;
			}
			else{
				fr = chara;
			}
			friends.push_back(fr);
		}
		return friends;
	}
}

std::vector<long long> FriendsTable::getBestFriends(long long charobjid){
	std::string qr = "SELECT `charida`, `charidb` FROM `friends` WHERE (`charida` = '" + std::to_string(charobjid) + "' OR `charidb` = '" + std::to_string(charobjid) + "') AND `status` IN ('BEST_FRIENDS', 'BEST_FRIENDS_ACCEPTED');";
	auto qr2 = Database::Query(qr);
	std::vector<long long> friends;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return friends;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		friends.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			long long charb = std::stoll(row[1]);
			long long fr = 0;
			if (chara == charobjid){
				fr = charb;
			}
			else{
				fr = chara;
			}
			friends.push_back(fr);
		}
		return friends;
	}
}

std::vector<std::pair<long long, std::string>> FriendsTable::getFriendResponses(long long charobjid){
	std::string qr = "SELECT `charidb`, `status` FROM `friends` WHERE `charida` = '" + std::to_string(charobjid) + "' AND `status` in ('ACCEPTED', 'DECLINED');";
	auto qr2 = Database::Query(qr);
	std::vector<std::pair<long long, std::string>> requests;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return requests;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		requests.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long charb = std::stoll(row[0]);
			std::string status = row[1];
			requests.push_back(std::make_pair(charb, status));
		}
		return requests;
	}
}

std::vector<long long> FriendsTable::getFriendRequests(long long charobjid){
	std::string qr = "SELECT `charida` FROM `friends` WHERE `charidb` = '" + std::to_string(charobjid) + "' AND `status` = 'REQUEST';";
	auto qr2 = Database::Query(qr);
	std::vector<long long> requests;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return requests;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		requests.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			requests.push_back(chara);
		}
		return requests;
	}
}

std::vector<long long> FriendsTable::getBestFriendRequests(long long charobjid){
	std::string qr = "SELECT `charida` FROM `friends` WHERE `charidb` = '" + std::to_string(charobjid) + "' AND `status` = 'BEST_FRIEND_REQUEST';";
	auto qr2 = Database::Query(qr);
	std::vector<long long> requests;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return requests;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		requests.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			requests.push_back(chara);
		}
		return requests;
	}
}

void FriendsTable::setRequestStatus(long long charida, long long charidb, const std::string& status){
	std::stringstream str;
	str << "UPDATE `friends` SET `status` = '" << status << "' ";
	str << "WHERE " << "`charida` = '" << std::to_string(charida) << "' AND `charidb` = '" << std::to_string(charidb) << "';";
	Database::Query(str.str());
}

void FriendsTable::accept(long long requester, long long accepter){
	FriendsTable::setRequestStatus(requester, accepter, "ACCEPTED");

}

void FriendsTable::decline(long long requester, long long accepter){
	FriendsTable::setRequestStatus(requester, accepter, "DECLINED");
}

std::vector<MISSION_DATA> MissionsTable::getMissions(long long charid){
	std::string qr = "SELECT `missionid`, `count`, UNIX_TIMESTAMP(`time`) FROM `missions` WHERE `character` = '" + std::to_string(charid) + "';";
	auto qr2 = Database::Query(qr);
	std::vector<MISSION_DATA> missions;
	if (qr2 == NULL){
		std::cout << "[CHDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
	}
	if (qr2 == NULL || mysql_num_rows(qr2) == 0)
		return missions;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		missions.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			MISSION_DATA m;
			m.missionid = std::stoi(row[0]);
			m.missioncount = std::stoi(row[1]);
			m.timestamp = std::stoi(row[2]);
			missions.push_back(m);
		}
		return missions;
	}
}

void MailsTable::addMail(MailData data){
	std::stringstream query2;
	query2 << "INSERT INTO `mails` (`sender`, `recipient_id`, `subject`, `text`, `attachment`, `attachment_count`) ";
	query2 << "VALUES('" << data.sender << "', '" << std::to_string(data.recipient) << "', '";
	query2 << data.subject << "', '" << data.text << "', '" << std::to_string(data.attachment) << "', '" << std::to_string(data.attachment_count) << "'); ";
	auto a = Database::Query(query2.str());
	if (a == NULL){
		std::cout << "[CHDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
	}
}

void MailsTable::setIsRead(long long mailid){
	std::stringstream str;
	str << "UPDATE `mails` SET `is_read` = '1' ";
	str << "WHERE " << "`id` = '" << std::to_string(mailid) << "';";
	Database::Query(str.str());
}

std::vector<MailData> MailsTable::getMails(long long charid){
	std::string qr = "SELECT `id`, `subject`, `text`, `sender`, `attachment`, `attachment_count`, UNIX_TIMESTAMP(`sent_time`), `is_read` FROM `mails` WHERE `recipient_id` = '" + std::to_string(charid) + "';";
	auto qr2 = Database::Query(qr);
	std::vector<MailData> mails;
	if (qr2 == NULL){
		std::cout << "[CHDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
		return mails;
	}
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return mails;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		mails.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			MailData mail;
			mail.id = std::stoll(row[0]);
			mail.subject = row[1];
			mail.text = row[2];
			mail.sender = row[3];
			mail.attachment = std::stoll(row[4]);
			mail.attachment_count = std::stoi(row[5]);
			mail.sent_time = std::stoi(row[6]);
			unsigned char readN = std::stoi(row[7]);
			if (readN == 1) mail.read = true; else mail.read = false;
			mails.push_back(mail);
		}
		return mails;
	}
}

void MailsTable::deleteMail(long long mailid){
	std::stringstream str;
	str << "DELETE FROM `mails` WHERE `id` = '" << std::to_string(mailid) << "';";
	Database::Query(str.str());
}