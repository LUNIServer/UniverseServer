#include "Characters.h"

#include "CharactersDB.h"
#include "AccountsDB.h"
#include "InventoryDB.h"
#include "Packet.h"
#include "UtfConverter.h"
#include "CharPackets.h"
#include "Logger.h"
#include "WorldServer.h"

bool Characters::CreateCharacter(RakNet::BitStream *packet, SystemAddress address, unsigned int accountid){
	unsigned char creationStatus = 0;
	
	std::string customName = UtfConverter::ToUtf8(PacketTools::ReadFromPacket(packet, 33));

	unsigned long firstName, middleName, lastName;
	packet->Read(firstName);
	packet->Read(middleName);
	packet->Read(lastName);

	std::string legoName = GetUnapprovedUsername(firstName, middleName, lastName);

	std::string name = legoName;
	std::string requestName = "";

	if (customName != ""){
		//We have multiple options here:
		//If we want this to work like the game, we have to request the custom name 
		requestName = customName;
		//If we want to leave it as is, we need to set the custom name as name if possible and the other one as the requested one
		//name = customName;
		//requestName = legoName;
	}

	Logger::log("GAME", "CHARS", "Requesting Character Creation");
	Logger::log("GAME", "CHARS", "CUSTOM NAME: " + customName);
	Logger::log("GAME", "CHARS", "LEGO NAME  : " + legoName);
	Logger::log("GAME", "CHARS", "NAME       : " + name);
	Logger::log("GAME", "CHARS", "REQUESTNAME: " + requestName);

	unsigned long long u1;
	unsigned char u2;
	packet->Read(u2);
	packet->Read(u1);

	CharacterStyle style;
	packet->Read(style.shirtColor);
	packet->Read(style.shirtStyle);
	packet->Read(style.pantsColor);
	packet->Read(style.hairStyle);
	packet->Read(style.hairColor);
	packet->Read(style.lh);
	packet->Read(style.rh);
	packet->Read(style.eyebrows);
	packet->Read(style.eyes);
	packet->Read(style.mouth);

	unsigned char u3;
	packet->Read(u3);

	long long extobjid = CharactersTable::getObjidFromCharacter(name);
	if (extobjid < 0){
		bool flag = true;
		if (requestName != ""){
			long long extobjidr = CharactersTable::getObjidFromCharacter(requestName);
			if (extobjidr > -1){
				flag = false;
			}
		}
		if (flag){
			bool flag2 = true;
			if (CharactersTable::unapprovedNameExists(requestName)){
				flag2 = false;
			}
			//if (flag2){ // Uncomment this line and comment the next line if you want to stop players from requesting character names that someone else already requested
			if (true){
				Logger::log("GAME", "CHARS", "Adding Char to Database");

				unsigned long shirtID = FindCharShirtID(style.shirtColor, style.shirtStyle);
				unsigned long pantsID = FindCharPantsID(style.pantsColor);
				
				CharacterInfo info;
				info.name = name;
				info.unapprovedName = requestName;

				long long charObjid = CharactersTable::add(style, accountid, info);
				AccountsTable::setFrontChar(charObjid);

				//To prepare the starter kit, create a shirtID and a pantsID object
				//Add them to the inventory and equip them
				long long shirtObjid = ObjectsTable::createObject(shirtID);
				InventoryTable::insertItem(charObjid, shirtObjid, 1, 0, true);
				EquipmentTable::equipItem(charObjid, shirtObjid);

				long long pantsObjid = ObjectsTable::createObject(pantsID);
				InventoryTable::insertItem(charObjid, pantsObjid, 1, 1, true);
				EquipmentTable::equipItem(charObjid, pantsObjid);
			}
			else{
				Logger::log("GAME", "CHARS", "The requested name was already requested by someone else");
				//The requested name was already requested by someone else
			}
		}
		else{
			Logger::log("GAME", "CHARS", "The requested name is already in use");
			creationStatus = 4;
			//The requested name is already in use
		}
	}
	else{
		Logger::log("GAME", "CHARS", "The name is already in use");
		creationStatus = 3;
		//The name is already in use
	}

	RakNet::BitStream *creationStatusPacket = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::CHARACTER_CREATION_RESPONSE);
	creationStatusPacket->Write(creationStatus);
	WorldServer::sendPacket(creationStatusPacket, address);
	
	return (creationStatus == 0);
}

void Characters::DeleteCharacter(unsigned int accountid, long long charid){
	CharactersTable::deleteCharacter(charid);
	AccountsTable::unsetFrontChar(accountid);
	EquipmentTable::deleteEquipment(charid); //Delete Equipment entries
	InventoryTable::deleteInventory(charid); //Delete inventory entries
}