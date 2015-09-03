#include "ChatCommands.h"
#include "Social.h"
#include "WorldConnection.h"
#include "WorldServer.h"
#include "UtfConverter.h"
#include "Logger.h"
#include "World.h"
#include "GameMessages.h"
#include "ReplicaData.h"
#include "Worlds.h"
#include "Account.h"
#include "Packet.h"
#include "InventoryDB.h"
#include "PlayerObject.h"
#include "serverLoop.h"

std::vector<ChatCommandHandler *> ChatCommandManager::ChatCommandHandlers;
std::unordered_map<std::wstring, ChatCommandHandler *> ChatCommandManager::ChatCommands;

void ChatCommandManager::registerCommands(ChatCommandHandler * CommandHandler){
	ChatCommandManager::ChatCommandHandlers.push_back(CommandHandler);
	std::vector<std::wstring> names = CommandHandler->getCommandNames();
	for (unsigned char k = 0; k < names.size(); k++){
		ChatCommandManager::ChatCommands.insert(std::make_pair(names.at(k), CommandHandler));
	}
}

void ChatCommandManager::handleCommand(std::wstring command, SessionInfo *s, std::vector<std::wstring> * params){
	if (command == L"help"){
		bool flag = true;
		if (params->size() > 0){
			std::wstring com = params->at(0);
			if (com.at(0) == L'/'){
				com = com.substr(1, com.size() - 1);
			}
			std::unordered_map<std::wstring, ChatCommandHandler *>::iterator it = ChatCommandManager::ChatCommands.find(com);
			if (it != ChatCommandManager::ChatCommands.end()){
				Chat::sendChatMessage(s->addr, L"Syntax: /" + com + L" " + it->second->getSyntax());
				Chat::sendChatMessage(s->addr, it->second->getDescription());
			}
			else{
				Chat::sendChatMessage(s->addr, L"Command " + params->at(0) + L" was not found");
			}
			flag = false;
		}
		if (flag){
			for (unsigned int k = 0; k < ChatCommandManager::ChatCommandHandlers.size(); k++){
				std::wstring desc = ChatCommandManager::ChatCommandHandlers.at(k)->getShortDescription();
				std::wstring cmd = ChatCommandManager::ChatCommandHandlers.at(k)->getCommandNames().at(0);
				unsigned long stringvalue = cmd.size() * 4;
				short diff = 0;
				std::wstring space = L"\t\t";
				for (unsigned int l = 0; l < cmd.size(); l++){
					wchar_t c = cmd.at(l);
					if (c == L'm') diff = diff + 2;
					else if (c == L'w') diff = diff + 2;
					else if (c == L'r') diff = diff - 1;
					else if (c == L'f') diff = diff - 2;
					else if (c == L't') diff = diff - 2;
					else if (c == L'i') diff = diff - 3;
					else if (c == L'j') diff = diff - 3;
					else if (c == L'l') diff = diff - 3;
				}

				if ((stringvalue + diff) > 30){
					space = L"\t";
				}
				
				if (desc != L""){
					Chat::sendChatMessage(s->addr, L"/" + cmd + space + desc);
				}
			}
		}
	}
	else{
		std::unordered_map<std::wstring, ChatCommandHandler *>::iterator it = ChatCommandManager::ChatCommands.find(command);
		if (it != ChatCommandManager::ChatCommands.end()){
			it->second->handleCommand(s, params);
		}
		else{
			Chat::sendChatMessage(s->addr, L"Command " + command + L" does not exist!");
		}
	}
}

void FlightCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	bool f2 = true;
	if (params->size() > 0){
		if (params->at(0) == L"off" || params->at(0) == L"false"){
			WorldServerPackets::SendGameMessage(s->addr, s->activeCharId, 561);
			f2 = false;
		}
	}
	if (f2){
		RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(s->activeCharId, 561);
		pc->Write((ulong)0x70ba);
		pc->Write((ushort)0x8);
		pc->Write((uchar)0x5);
		pc->Write((uchar)0x2);
		pc->Write((ushort)0xc);
		pc->Write((uchar)0x3);
		pc->Write((ushort)0x6c1);
		pc->Write((uchar)0x0);
		pc->Write((uchar)0x1);
		pc->Write((uchar)0x80);
		pc->Write((uchar)0x7f);
		pc->Write((ulong)0xa7);
		WorldServer::sendPacket(pc, s->addr);
	}
}

std::vector<std::wstring> FlightCommandHandler::getCommandNames(){
	return { L"flight", L"jetpack", L"fly" };
}

std::wstring FlightCommandHandler::getDescription(){
	return L"Enables the player to fly around the world as if they were to wear a jetpack";
}
std::wstring FlightCommandHandler::getShortDescription(){
	return L"Enables flight";
}
std::wstring FlightCommandHandler::getSyntax(){
	return L"[off]";
}

void TeleportCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
	bs->Write(s->activeCharId);
	bs->Write((unsigned short)19);
	bs->Write(false);
	bs->Write(false);
	bs->Write(false);
	float x = 0.0F;
	float y = 0.0F;
	float z = 0.0F;
	if (params->size() > 2){
		x = std::stof(params->at(0));
		y = std::stof(params->at(1));
		z = std::stof(params->at(2));
	}
	bs->Write(x);
	bs->Write(y);
	bs->Write(z);
	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s->zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}

std::vector<std::wstring> TeleportCommandHandler::getCommandNames(){
	return{ L"teleport", L"tele", L"ot"};
}

std::wstring TeleportCommandHandler::getDescription(){
	return L"Teleports a player to the specified location or map origin";
}
std::wstring TeleportCommandHandler::getShortDescription(){
	return L"Teleport player";
}
std::wstring TeleportCommandHandler::getSyntax(){
	return L"[<x> <y> <z>]";
}

void WhisperCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		std::wstring reciever = params->at(0);
		std::wstring message = params->at(1);
		for (unsigned int k = 2; k < params->size(); k++){
			message.append(L" ");
			message.append(params->at(k));
		}
		Logger::log("WRLD", "COMMAND", "Private message to " + UtfConverter::ToUtf8(reciever));
		Logger::log("WRLD", "COMMAND", UtfConverter::ToUtf8(message));
	}
}

std::vector<std::wstring> WhisperCommandHandler::getCommandNames(){
	return{ L"whisper", L"w", L"tell" };
}

std::wstring WhisperCommandHandler::getDescription(){
	return L"Sends another player a private chat message";
}
std::wstring WhisperCommandHandler::getShortDescription(){
	return L"Private Chat";
}
std::wstring WhisperCommandHandler::getSyntax(){
	return L"<reciever> <message>";
}

void TestmapCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 0){
		unsigned short argumentValue = std::stoi(params->at(0));
		ZoneId zone = static_cast<ZoneId>(argumentValue);
		Logger::log("WRLD", "CHAT", "Requesting teleport to " + std::to_string(argumentValue));
		bool f = false;
		if (getWorldTarget(zone).size() > 0){
			if (zone != ZoneId::NO_ZONE && zone != ZoneId::KEELHAUL_CANYON){
				COMPONENT1_POSITION pos = getZoneSpawnPoint(zone, static_cast<ZoneId>(s->zone));
				f = Worlds::loadWorld(s->addr, zone, pos, 0, 0);
				if (f){
					Session::leave(s->activeCharId);

					WorldPlace place;
					place.zoneID = zone;
					place.mapClone = 0;
					place.mapInstance = 0;
					place.x = pos.x;
					place.y = pos.y;
					place.z = pos.z;
					CharactersTable::setCharactersPlace(s->activeCharId, place);
					ObjectsManager::clientLeaveWorld(s->activeCharId, s->addr);
				}
				else{
					Chat::sendChatMessage(s->addr, L"Cannot teleport to this zone");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Cannot teleport to this zone");
			}
		}
		if (!f){
			Chat::sendChatMessage(s->addr, L"Cannot teleport to WorldID " + params->at(0));
		}
	}
}

std::vector<std::wstring> TestmapCommandHandler::getCommandNames(){
	return{ L"testmap", L"tp" };
}

std::wstring TestmapCommandHandler::getDescription(){
	return L"Sends the player to another world";
}
std::wstring TestmapCommandHandler::getShortDescription(){
	return L"World Travel";
}
std::wstring TestmapCommandHandler::getSyntax(){
	return L"<ZoneId>";
}

void SwitchCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		std::string ip = UtfConverter::ToUtf8(params->at(0));
		short port = (short)std::stoi(params->at(1));
		RakNet::BitStream * redirect = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_REDIRECT);
		PacketTools::WriteToPacket(redirect, ip, 33);
		redirect->Write(port);
		redirect->Write((unsigned char)1);
		WorldServer::sendPacket(redirect, s->addr);
	}
}

std::vector<std::wstring> SwitchCommandHandler::getCommandNames(){
	return{ L"switch", L"tpx" };
}

std::wstring SwitchCommandHandler::getDescription(){
	return L"Sends the player to another server";
}
std::wstring SwitchCommandHandler::getShortDescription(){
	return L"Change Servers";
}
std::wstring SwitchCommandHandler::getSyntax(){
	return L"<IP> <port>";
}

void ItemsCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		if (params->at(0) == L"create"){
			long lot = std::stol(params->at(1));
			long long objid = ObjectsTable::createObject(lot);
			Chat::sendChatMessage(s->addr, L"Object created with id: " + std::to_wstring(objid - 1152921510794154770));
		}

		if (params->at(0) == L"equip"){
			long long objid = 1152921510794154770 + std::stoll(params->at(1));
			PlayerObject *player = (PlayerObject *) ObjectsManager::getObjectByID(s->activeCharId);
			if (player != NULL){
				if (params->size() > 2){
					ushort slot = std::stoi(params->at(2));
					player->getComponent17()->equipItem(objid, slot);
				}
				else{
					player->getComponent17()->equipItem(objid);
				}
				ObjectsManager::serialize(player);  //player->serialize();
			}
		}
		if (params->at(0) == L"unequip"){
			long long objid = 1152921510794154770 + std::stoll(params->at(1));
			PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
			if (player != NULL){
				bool un = player->getComponent17()->unequipItem(objid);
				if (!un){
					Chat::sendChatMessage(s->addr, L"Object not found");
				}
				else{
					ObjectsManager::serialize(player); // player->serialize();
				}
			}
		}
	}
}

std::vector<std::wstring> ItemsCommandHandler::getCommandNames(){
	return{ L"inventory", L"item"};
}

std::wstring ItemsCommandHandler::getDescription(){
	return L"Manages Items for a player";
}

std::wstring ItemsCommandHandler::getShortDescription(){
	return L"Manage Items";
}

std::wstring ItemsCommandHandler::getSyntax(){
	return L"(create <LOT>|equip <num>|unequip <num>)";
}

void AddItemCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() == 2){
		long lot = std::stoi(params->at(0));
		unsigned long amount = std::stoi(params->at(1));

		unsigned long slot = -1;
		for (int i = 0; (slot == -1) && (i != 24); i++){
			if (InventoryTable::getItemFromSlot(s->activeCharId, i) == -1)
				slot = i;
		}

		if (slot == -1){
			Chat::sendChatMessage(s->addr, L"Can't add requested item to your inventory. There aren't any free slots!");
		}
		else{
			long long objid = ObjectsTable::createObject(lot);
			InventoryTable::insertItem(s->activeCharId, objid, amount, slot, false);
			Chat::sendChatMessage(s->addr, L"Successfully added the requested item to your inventory! Please travel to another world or relog to reload your inventory.");
		}
	}
	else if (params->size() == 3){
		long lot = std::stoi(params->at(0));
		unsigned long amount = std::stoi(params->at(1));
		std::string name = UtfConverter::ToUtf8(params->at(2));

		bool found = false;
		long long charid = CharactersTable::getObjidFromCharacter(name);
		if (charid > 0){
			SystemAddress addr = SessionsTable::findCharacter(charid);
			if (addr != UNASSIGNED_SYSTEM_ADDRESS){
				found = true;

				unsigned long slot = -1;
				for (int i = 0; (slot == -1) && (i != 24); i++){
					if (InventoryTable::getItemFromSlot(charid, i) == -1)
						slot = i;
				}

				if (slot == -1){
					std::stringstream ss;
					ss << "Can't add requested item to ";
					ss << name;
					ss << "'s inventory. There aren't any free slots!";

					Chat::sendChatMessage(s->addr, UtfConverter::FromUtf8(ss.str()));
				}
				else{
					long long objid = ObjectsTable::createObject(lot);
					InventoryTable::insertItem(charid, objid, amount, slot, false);

					std::stringstream ss;
					ss << "Successfully added the requested item to ";
					ss << name;
					ss << "'s inventory!";
					Chat::sendChatMessage(s->addr, UtfConverter::FromUtf8(ss.str()));

					std::string source = "UNKNOWN";
					
					ListCharacterInfo c = CharactersTable::getCharacterInfo(s->activeCharId);
					if (c.info.objid > 0){
						source = c.info.name;
					}
					std::stringstream ss2;
					ss2 << source;
					ss2 << " added an item with LOT ";
					ss2 << std::to_string(lot);
					ss2 << " to your inventory. Please travel to another world or relog to reload your inventory.";
					Chat::sendChatMessage(addr, UtfConverter::FromUtf8(ss2.str()));
				}
			}
		}

		if (!found){
			std::stringstream ss;
			ss << "Can't add requested item to ";
			ss << name;
			ss << "'s inventory. Player not found/online!";
			Chat::sendChatMessage(s->addr, UtfConverter::FromUtf8(ss.str()));
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"Syntac: /gmadditem " + this->getSyntax());
	}
}

std::vector<std::wstring> AddItemCommandHandler::getCommandNames(){
	return{ L"gmadditem" };
}

std::wstring AddItemCommandHandler::getDescription(){
	return L"Adds an Item to a Users Inventory";
}

std::wstring AddItemCommandHandler::getShortDescription(){
	return L"Adds an Item";
}

std::wstring AddItemCommandHandler::getSyntax(){
	return L"<LOT> <amount> [<name>]";
}

void PositionCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
	std::wstringstream wstr;
	if (player != NULL){
		COMPONENT1_POSITION pos = player->getComponent1()->getPosition();
		wstr << L"Position: (" << pos.x << "|" << pos.y << "|" << pos.z << ")";
	}
	Chat::sendChatMessage(s->addr, wstr.str());
}

std::vector<std::wstring> PositionCommandHandler::getCommandNames(){
	return{ L"position", L"pos"};
}

std::wstring PositionCommandHandler::getDescription(){
	return L"Displays the position";
}

std::wstring PositionCommandHandler::getShortDescription(){
	return L"Display Position";
}

std::wstring PositionCommandHandler::getSyntax(){
	return L"";
}

void ClientCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){

}

std::vector<std::wstring> ClientCommandHandler::getCommandNames(){
	return{ L"loc"};
}

std::wstring ClientCommandHandler::getDescription(){
	return L"";
}

std::wstring ClientCommandHandler::getShortDescription(){
	return L"";
}

std::wstring ClientCommandHandler::getSyntax(){
	return L"";
}

void AttributeCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		std::wstring attr = params->at(0);
		unsigned long value = std::stoul(params->at(1));
		bool max = false;
		float maxV = 0;
		if (params->size() > 2){
			max = true;
			maxV = std::stof(params->at(2));
		}
		PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
		if (player != NULL){
			Component7 * c7 = player->getComponent7();
			COMPONENT7_DATA4 d4 = c7->getData4();
			if (attr == L"health"){
				d4.health = value;
				if (max) d4.maxHealth = maxV;
			}
			else if (attr == L"armor"){
				d4.armor = value;
				if (max) d4.maxArmor = maxV;
			}
			else if (attr == L"imagi"){
				d4.imagination = value;
				if (max) d4.maxImagination = maxV;
			}
			c7->setData4(d4);
			ObjectsManager::serialize(player);
		}
	}
}

std::vector<std::wstring> AttributeCommandHandler::getCommandNames(){
	return{ L"setattr" };
}

std::wstring AttributeCommandHandler::getDescription(){
	return L"Sets attributes for the player";
}

std::wstring AttributeCommandHandler::getShortDescription(){
	return L"Set Attributes";
}

std::wstring AttributeCommandHandler::getSyntax(){
	return L"(health|armor|imagi) <value> [<max>]";
}

void PacketCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	std::wstring msg = L"Usage: /packet <path>";
	if (params->size() > 0){
		std::vector<unsigned char> v = OpenPacket(UtfConverter::ToUtf8(params->at(0)));
		if (v.size() > 0){
			WorldServer::sendPacket(v, s->addr);
			msg = L"Success sending packet";
		}
		else{
			msg = L"Error sending packet";
		}
	}
	Chat::sendChatMessage(s->addr, msg, L"System");
}

std::vector<std::wstring> PacketCommandHandler::getCommandNames(){
	return{ L"packet" };
}

std::wstring PacketCommandHandler::getDescription(){
	return L"Sends a packet from disk";
}

std::wstring PacketCommandHandler::getShortDescription(){
	return L"Send Packet";
}

std::wstring PacketCommandHandler::getSyntax(){
	return L"<file>";
}

void AnnouncementCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		long long charid;
		std::wstring rec = params->at(0);
		bool flagOne = false;
		if (rec == L"#"){
			charid = s->activeCharId;
			flagOne = true;
		}
		else if (rec == L"*"){
			std::string msg = UtfConverter::ToUtf8(params->at(1));
			std::string title = "Information";
			if (params->size() > 2){
				title = UtfConverter::ToUtf8(params->at(2));
			}
			std::vector<SessionInfo> wsessions = SessionsTable::getClientsInWorld(s->zone);
			for (unsigned int i = 0; i < wsessions.size(); i++){
				Chat::sendMythranInfo(wsessions.at(i).activeCharId, msg, title);
			}
		}
		else{
			charid = CharactersTable::getObjidFromCharacter(UtfConverter::ToUtf8(rec));
			flagOne = true;
		}

		if (flagOne){
			if (charid > 0){
				std::string msg = UtfConverter::ToUtf8(params->at(1));
				std::string title = "Information";
				if (params->size() > 2){
					title = UtfConverter::ToUtf8(params->at(2));
				}
				Chat::sendMythranInfo(charid, msg, title);
			}
			else{
				std::wstring response = L"\"" + rec + L"\" is not a valid Playername";
				Chat::sendChatMessage(s->addr, response);
			}
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"Usage: /popup " + this->getSyntax());
	}
}

std::vector<std::wstring> AnnouncementCommandHandler::getCommandNames(){
	return{ L"announce", L"popup" };
}

std::wstring AnnouncementCommandHandler::getDescription(){
	return L"Sends a localized Mythran announcement";
}

std::wstring AnnouncementCommandHandler::getShortDescription(){
	return L"Send Announcement";
}

std::wstring AnnouncementCommandHandler::getSyntax(){
	return L"(<playername>|*|#) <message> [<title>]";
}