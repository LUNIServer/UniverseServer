#include "User.h"
#include "Database.h"
#include "InventoryDB.h"
#include "CharactersDB.h"
#include "AccountsDB.h"
#include "LDF.h"

#include "Account.h"
#include "Social.h"

#include "UsersPool.h"
#include "serverLoop.h"
#include "Packet.h"
#include "Encryption.h"
#include "Logger.h"

#include <sstream>
#include <map>

using namespace std;

extern std::map<SystemAddress, ZoneId> Player;

// This is the user initalization
User::User(uint id, const string& username, const SystemAddress& systemAddress) {
	this->id = id; // Store User ID
	this->username = username; // Store Username
	this->ip = systemAddress; // Store systemAddress
	this->charctersData = std::vector< Ref<Character> >(4);
	for (int i = 0; i < 4; i++) characters[i] = NULL; // Initialize characters
	current = NULL; // Initialize current character
	player = NULL;

	ostringstream osid;
	osid << id;

	std::vector<long long> chars = CharactersTable::getCharacters(id);
	for (uchar k = 0; k < 4; k++){
		if (k < chars.size()) characters[k] = chars.at(k); else characters[k] = 0;
	}
}

// Initialize user with another user's data
User::User(const User& usr) {
	memcpy(this, &usr, sizeof(User));
}

// Load the characters
std::vector< Ref<Character> > User::LoadCharacters() {
	//vector< Ref<Character> > r(4);
	for (int i = 0; i < 4; i++) this->charctersData[i] = Ref<Character>( new Character(characters[i]) );
		//(characters[i] != 0) ? unique_ptr<Character>(new Character(characters[i])) : NULL;
	return this->charctersData;
}

void User::SetCharacter(long long cid){
	Logger::log("USER", "", "Set char id " + std::to_string(cid));
	AccountsTable::setFrontChar(cid);
	for (int i = 0; i < 4; i++){
		if (characters[i] == cid){
			if (this->charctersData[i] == NULL){
				this->charctersData[i] = Ref<Character>(new Character(cid));
			}
			current = this->charctersData[i];
			return;
		}
	}
	Logger::log("USER", "", "Loading char " + std::to_string(cid));
	current = Ref<Character>(new Character(cid));
}

void User::CreatePlayer(){
	if (this->player != NULL){
		delete this->player;
		this->player = NULL;
		cout << "[WARN] WARNING: Player Object was not destroyed";
	}
	
	ZoneId cZone = this->getWorld();
	//cout << "USER: " << this->ip.ToString() << ":" << cZone << std::endl;
	
	std::wstring name;
	COMPONENT1_POSITION pos = getZoneSpawnPoint(this->getWorld());
	Ref<Character> chr = this->GetCurrentCharacter();
	if (chr != NULL){
		name = StringToWString(chr->GetName());
		if (!chr->pos.isNull()) pos = chr->pos;
	}
	else{
		name = StringToWString(this->username);
	}
	//1152921507004579166
	//1152921504606846976
	this->player = new PlayerObject(this->GetCurrentCharacter()->charobjid, name);
	Component1 *c1 = this->player->getComponent1();
	c1->setPosition(pos);
	Component4 *c4 = this->player->getComponent4();
	c4->setLevel(this->current->level);
	PLAYER_INFO pi;
	pi.accountID = this->GetID();
	pi.isFreeToPlay = false;
	pi.legoScore = 600;
	c4->setInfo(pi);

	if (chr != NULL){
		CharacterData cd = chr->Data;
		PLAYER_STYLE style;
		style.eyebrowsStyle = cd.minifig.eyebrows;
		style.eyesStyle = cd.minifig.eyes;
		style.hairColor = cd.minifig.haircolor;
		style.hairStyle = cd.minifig.hairstyle;
		style.mouthStyle = cd.minifig.mouth;
		style.pantsColor = cd.minifig.pants;
		style.shirtColor = cd.minifig.shirtcolor;
		this->player->getComponent4()->setStyle(style);
	}
}

PlayerObject *User::GetPlayer(){
	return this->player;
}

void User::DestructPlayer(){ //TODO: investigate player getting transparent when rejoining the same world
	if (this->player != NULL){
		this->player->destruct();
		delete this->player;
		this->player = NULL;
	}
}

bool User::ChangeWorld(ZoneId zone, RakPeerInterface* rakServer, SystemAddress &systemAddress){
	if (zone == KEELHAUL_CANYON){
		return false;
	}
	else {
		RakNet::BitStream worldLoad;
		bool flag = this->LoadWorld(zone, &worldLoad);
		if (!flag) return false; //World is not accessible
		Player.erase(systemAddress);
		Session::leave(this->current->charobjid);
		
		this->current->pos = getZoneSpawnPoint(zone, this->getWorld());
		this->current->Data.lastZone = zone;

		WorldPlace place;
		place.zoneID = zone;
		place.mapClone = 0;
		place.mapInstance = 0;
		place.x = this->current->pos.x;
		place.y = this->current->pos.y;
		place.z = this->current->pos.z;
		CharactersTable::setCharactersPlace(this->current->charobjid, place);

		rakServer->Send(&worldLoad, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
		
		this->DestructPlayer();
		return true;
	}
}

bool User::LoadWorld(ZoneId zone, RakNet::BitStream *stream){
	stream->Write((uchar)83);
	stream->Write((ushort)5);
	stream->Write((ulong)2);
	stream->Write((uchar)0);

	if (zone == ZoneId::NO_ZONE){
		Logger::log("USER", "WARNING", "No zone selected, default to 1200 NIMBUS_STATION");
		zone = ZoneId::NIMBUS_STATION;
	}

	stream->Write((ushort)zone);
	stream->Write((ushort)0); //Instance
	stream->Write((ulong)0); //Clone

	std::vector<uchar> worldTarget = getWorldTarget(zone); //TODO: Replace this with actual numbers
	if (worldTarget.size() == 0) return false;
	stream->Write(worldTarget[2]); //Checksum
	stream->Write(worldTarget[3]);
	stream->Write(worldTarget[4]);
	stream->Write(worldTarget[5]); //End Checksum

	COMPONENT1_POSITION pos = this->GetCurrentCharacter()->pos;
	if (pos.isNull()){
		pos = getZoneSpawnPoint(zone);
	}
	stream->Write(pos.x);
	stream->Write(pos.y);
	stream->Write(pos.z);
	stream->Write((ulong)0);
	return true;
}

ZoneId User::getWorld(){
	if (this->current != NULL){
		return this->current->Data.lastZone;
	}
	else{
		return ZoneId::NO_ZONE;
	}
}

// Login a user
/*Ref<User> User::Login(const string& nikname, const string& password, const SystemAddress& systemAddress) {
	UserSuccess currentLoginStatus = UserSuccess::SUCCESS;

	unsigned int accountid = AccountsTable::getAccountID(nikname);
	if (accountid == 0){
		currentLoginStatus = UserSuccess::INVALID_USER;
		return NULL;
	}
	bool passwordCorrect = AccountsTable::checkPassword(password, accountid);
	AccountAccessInfo info = AccountsTable::getAccessInfo(accountid);

	if (!passwordCorrect && info.loginTries < 2){
		currentLoginStatus = UserSuccess::INVALID_PASS;	// Set the loginStatus to INVALID_PASS
		info.loginTries++; // Increase the loginTries
		if (info.loginTries == 3) {
			currentLoginStatus = UserSuccess::LOCKED; // Change account status to 'locked'
			info.locked = true;
		}
		AccountsTable::setAccessInfo(accountid, info);
	} else {
		if (info.loginTries > 0){
			info.loginTries = 0;
			AccountsTable::setAccessInfo(accountid, info);
		}
		
		// Simple check to see if account is banned. If it is, return BANNED (for client to show)
		if (info.banned) {
			Logger::log("USER", "LOGIN", "User is BANNED");
			currentLoginStatus = UserSuccess::BANNED;
		}
	}

	// If account is locked, whether user entered in correct password or not,
	// Show this (because each username is unique)
	if (info.locked) {
		Logger::log("USER", "LOGIN", "User is LOCKED");

		// Change currentStatus to LOCKED
		currentLoginStatus = UserSuccess::LOCKED;
	}

	// Print current status to console (just to check, to delete later)
	Logger::log("USER", "LOGIN", "Current login status is " + std::to_string(currentLoginStatus));
	return Ref<User>( new User(accountid, nikname, systemAddress, currentLoginStatus) );
}*/

uint User::GetID() { return id; }
Ref<Character> User::GetCurrentCharacter() { return current; }
SystemAddress User::GetIp() { return ip; }
std::string User::GetUsername() { return username; }

RakNet::BitStream *User::sendMessage(std::wstring message, std::wstring sender, bool isMythran){
	RakNet::BitStream *aw = new RakNet::BitStream();
	CreatePacketHeader(ID_USER_PACKET_ENUM, 2, 1, aw);

	uchar u8 = 0;
	ushort u16 = 0;
	ulong u32 = 0;
	ulonglong u64 = 0;

	aw->Write(u64);
	aw->Write((uchar)0x01);
	int len = message.size();
	aw->Write((uchar)(len+1));
	aw->Write(u16);
	aw->Write(u8);

	//Static size of sender name (66 bytes), gets cropped if too long
	for (uint k = 0; k < 33; k++){
		if (k < sender.size()){
			aw->Write(sender.at(k));
		}else{
			aw->Write(u16);
		}
	}

	aw->Write((int)1); //sender Object id
	aw->Write(u32);
	aw->Write(u16); //??
	aw->Write((uchar)isMythran);

	for (uint k = 0; k < message.size(); k++){
		aw->Write(message.at(k));
	}
	aw->Write(u16);

	return aw;
}

void writeXML(RakNet::BitStream *xml, std::string str){
	for (ulong i = 0; i < str.size(); i++){
		xml->Write(str.at(i));
	}
}

void User::getChardata(long long objid, RakNet::BitStream *packet){
	packet->Write((uchar)83);
	packet->Write((ushort)5);
	packet->Write((ulong)4);
	packet->Write((uchar)0);

	LDF *ldf = new LDF();
	ldf->writeS64(L"accountID", this->id);
	ldf->writeS32(L"chatmode", 0); //0 - Normal, 1 - Super
	ldf->writeBOOL(L"editor_enabled", false);
	ldf->writeS32(L"editor_level", 0);
	ldf->writeBOOL(L"freetrial", false);
	ldf->writeS32(L"gmlevel", 0); //0 - Normal, 1 - Mythran
	ldf->writeBOOL(L"legoclub", true);
	ldf->writeS64(L"levelid", 0); //Try e3 04 f4 74 95 51 08 20 if this breaks

	std::wstring name;
	Ref<Character> chr = this->GetCurrentCharacter();
	if (chr != NULL){
		name = StringToWString(chr->GetName());
	}
	else{
		name = StringToWString(this->GetUsername());
	}

	ldf->writeWSTRING(L"name", name);
	ldf->writeID(L"objid", objid);
	ldf->writeFLOAT(L"position.x", this->current->pos.x);
	ldf->writeFLOAT(L"position.y", this->current->pos.y);
	ldf->writeFLOAT(L"position.z", this->current->pos.z);
	ldf->writeS64(L"reputation", 100); //Set reputation here
	ldf->writeS32(L"template", 1);

	RakNet::BitStream *xml = new RakNet::BitStream();
	writeXML(xml, "<?xml version=\"1.0\"?>");
	writeXML(xml, "<obj v=\"1\">");
	writeXML(xml, "<buff/>");
	writeXML(xml, "<skil/>");
	writeXML(xml, "<inv>");
	writeXML(xml, "<bag>");
	writeXML(xml, "<b t=\"0\" m=\"24\"/>"); //Items, Default size: 20
	writeXML(xml, "</bag>");
	writeXML(xml, "<items>");
	writeXML(xml, "<in>");
	//Adding items to the inventory:
	//l: LOT, template, type of item
	//id: objid, unique, this object
	//s: slot in inventory
	//b: present and set to "1" if object is linked to the player
	//c: amout of this item
	std::vector<InventoryItem> items = InventoryTable::getItems(objid);
	for (uint k = 0; k < items.size(); k++){
		//long lot = ObjectsTable::getTemplateOfItem(items.at(k).objid);
		ObjectInfo oinfo = ObjectsTable::getItemInfo(items.at(k).objid);
		Logger::log("USER", "CHARDATA", "Adding item " + std::to_string(oinfo.lot) + "[" + std::to_string(oinfo.objid) + "] " + std::to_string(oinfo.spawnerid));

		if (oinfo.lot > -1){
			stringstream adddata;
			adddata << "<i l=\"" << oinfo.lot << "\" id=\"" << items.at(k).objid << "\" s=\"" << items.at(k).slot << "\"";
			if (items.at(k).qnt > 1){
				adddata << " c=\"" << items.at(k).qnt << "\"";
			}
			if (oinfo.spawnerid > -1){
				adddata << " sk=\"" << oinfo.spawnerid << "\"";
			}
			if (items.at(k).linked){
				adddata << " b=\"1\"";
			}
			if (oinfo.spawnerid > -1){
				adddata << ">";

				ObjectInfo sinfo = ObjectsTable::getItemInfo(oinfo.spawnerid);
				if (sinfo.spawnerid == -1){
					//Not an item for an object itself
					if (sinfo.lot == 6416){
						//This is a custom Rocket
						RocketInfo rinfo = ObjectsTable::getRocketInfo(sinfo.objid);
						if (rinfo.cockpit_template > 0 && rinfo.engine_template > 0 && rinfo.nose_cone_template > 0){
							//adddata << "<x ma=\"0:1:" << rinfo.nose_cone_template << "+1:" << rinfo.cockpit_template << "+1:" << rinfo.engine_template << "\"/>";
							Logger::log("USER", "CHARDATA", "Adding Rocket");
						}
					}
				}

				adddata << "</i>";
			}else{
				adddata << "/>";
			}
			writeXML(xml, adddata.str());
		}
	}
	writeXML(xml, "</in>");
	writeXML(xml, "</items>");
	writeXML(xml, "</inv>");
	writeXML(xml, "<mf/>");
	writeXML(xml, "<char cc=\"");
	writeXML(xml, std::to_string(100).c_str());
	writeXML(xml, "\"></char>");
	std::stringstream adddata;
	adddata << "<lvl";
	adddata << " l=\"" << std::to_string(this->current->level) << "\"";
	adddata << "/>";
	writeXML(xml, adddata.str());
	writeXML(xml, "<flag/>");
	writeXML(xml, "<pet/>");
	
	std::vector<MISSION_DATA> missions = MissionsTable::getMissions(objid);
	if (missions.size() > 0){
		writeXML(xml, "<mis>");
		writeXML(xml, "<done>");
		for (unsigned int k = 0; k < missions.size(); k++){
			stringstream adddata;
			adddata << "<m";
			adddata << " id=\"" << std::to_string(missions.at(k).missionid) << "\"";
			adddata << " cts=\"" << std::to_string(missions.at(k).timestamp) << "\"";
			adddata << " cct=\"" << std::to_string(missions.at(k).missioncount) << "\"";
			adddata << "/>";
			writeXML(xml, adddata.str());
		}
		writeXML(xml, "</done>");
		writeXML(xml, "</mis>");
	}
	else{
		writeXML(xml, "<mis/>");
	}
	writeXML(xml, "<mnt/>");
	writeXML(xml, "<dest/>");
	writeXML(xml, "</obj>");

	ldf->writeBYTES(L"xmlData", xml);

	packet->Write((ulong)(ldf->getSize() + 1));
	packet->Write((uchar)0);
	ldf->writeToPacket(packet);
}

User::destructor User() { current = NULL; }


void sendGameMessage(long long objid, RakNet::BitStream *packet, ushort message){
	packet->Write((uchar)83);
	packet->Write((ushort)5);
	packet->Write((ulong)12);
	packet->Write((uchar)0);
	packet->Write(objid);
	packet->Write(message);
}