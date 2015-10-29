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
#include "NPCObject.h"
#include "ReplicaComponents.h"
#include "pugixml.hpp"
#include "WorldObjectsDB.h"

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
			Chat::sendChatMessage(s->addr, L"Switched off fly mode!");
		}
	}
	if (f2){
		RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(s->activeCharId, 561);

		pc->Write((unsigned long)0x70ba);
		pc->Write((unsigned short)0x8);
		pc->Write((unsigned char)0x5);
		pc->Write((unsigned char)0x2);
		pc->Write((unsigned short)0xc);
		pc->Write((unsigned char)0x3);
		pc->Write((unsigned short)0x6c1);
		pc->Write((unsigned char)0x0);
		pc->Write((unsigned char)0x1);
		pc->Write((unsigned char)0x80);
		pc->Write((unsigned char)0x7f);
		pc->Write((unsigned long)0xa7);

		WorldServer::sendPacket(pc, s->addr);

		Chat::sendChatMessage(s->addr, L"Switched on fly mode!");
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
	if (params->size() == 3){
		std::string check = UtfConverter::ToUtf8(params->at(0));
		bool flag = isNumber(check);
		check = UtfConverter::ToUtf8(params->at(1));
		if (flag) flag = isNumber(check);
		check = UtfConverter::ToUtf8(params->at(2));
		if (flag) flag = isNumber(check);
		if (flag){
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

			Chat::sendChatMessage(s->addr, L"Teleported!");
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
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
	if (params->size() == 1){
		std::string check = UtfConverter::ToUtf8(params->at(0));
		bool flag = isNumber(check);
		if (flag){
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
		else{
			Chat::sendChatMessage(s->addr, L"Cannot teleport to WorldID " + params->at(0));
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
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

		SystemAddress target;
		target.SetBinaryAddress(ip.data());
		target.port = port;

		bool flag = Session::sendToInstance(s->addr, target);

		if (flag){
			RakNet::BitStream * redirect = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_REDIRECT);
			PacketTools::WriteToPacket(redirect, ip, 33);
			redirect->Write(port);
			redirect->Write((unsigned char)1);
			WorldServer::sendPacket(redirect, s->addr);
		}
		else{
			Chat::sendChatMessage(s->addr, L"Could not switch instance");
		}
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

void AddItemCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() == 2){
		std::string check = UtfConverter::ToUtf8(params->at(0));
		bool flag = isNumber(check);
		check = UtfConverter::ToUtf8(params->at(1));
		if (flag) flag = isNumber(check);
		if (flag){
			uint lot = std::stoi(params->at(0));

			std::string type = LOTInfo::getType(lot);
			std::string name = LOTInfo::getName(lot);

			if (type != "NOT_FOUND"){
				if (type == "Loot" || AccountsTable::getRank(s->accountid) > 0){
					if (!startsWith(UtfConverter::FromUtf8(name), L"Model Reward") || AccountsTable::getRank(s->accountid) > 0){
						unsigned long amount = std::stoi(params->at(1));

						unsigned long slot = -1;
						for (int i = 0; (slot == -1) && (i != 64); i++){
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
					else{
						Chat::sendChatMessage(s->addr, L"You aren't allowed to have models in your inventory!");
					}
				}
				else{
					Chat::sendChatMessage(s->addr, L"You aren't allowed to own items with ObjectType '" + UtfConverter::FromUtf8(type) + L"'!");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Invalid LOT: '" + std::to_wstring(lot) + L"'");
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else if (params->size() == 3){
		std::string check = UtfConverter::ToUtf8(params->at(0));
		bool flag = isNumber(check);
		check = UtfConverter::ToUtf8(params->at(1));
		if (flag) flag = isNumber(check);
		if (flag){
			uint lot = std::stoi(params->at(0));

			std::string type = LOTInfo::getType(lot);
			std::string name = LOTInfo::getName(lot);

			if (type != "NOT_FOUND"){
				if (type == "Loot" || AccountsTable::getRank(s->accountid) > 0){
					if (!startsWith(UtfConverter::FromUtf8(name), L"Model Reward") || AccountsTable::getRank(s->accountid) > 0){
						unsigned long amount = std::stoi(params->at(1));
						std::string name = UtfConverter::ToUtf8(params->at(2));

						bool found = false;
						long long charid = CharactersTable::getObjidFromCharacter(name);
						if (charid > 0){
							SystemAddress addr = SessionsTable::findCharacter(charid);
							if (addr != UNASSIGNED_SYSTEM_ADDRESS){
								found = true;

								unsigned long slot = -1;
								for (int i = 0; (slot == -1) && (i != 64); i++){
									if (InventoryTable::getItemFromSlot(charid, i) == -1)
										slot = i;
								}

								if (slot == -1){
									Chat::sendChatMessage(s->addr, L"Can't add requested item to " + UtfConverter::FromUtf8(name) + L"'s inventory. There aren't any free slots!");
								}
								else{
									long long objid = ObjectsTable::createObject(lot);
									InventoryTable::insertItem(charid, objid, amount, slot, false);

									Chat::sendChatMessage(s->addr, L"Successfully added the requested item to " + UtfConverter::FromUtf8(name) + L"'s inventory!");

									std::string source = "UNKNOWN";

									ListCharacterInfo c = CharactersTable::getCharacterInfo(s->activeCharId);
									if (c.info.objid > 0){
										source = c.info.name;
									}

									Chat::sendChatMessage(addr, UtfConverter::FromUtf8(source) + L" added an item with LOT " + std::to_wstring(lot) + L" to your inventory. Please travel to another world or relog to reload your inventory.");
								}
							}
						}

						if (!found){
							Chat::sendChatMessage(s->addr, L"Can't add requested item to " + UtfConverter::FromUtf8(name) + L"'s inventory. Player not found/online!");
						}
					}
					else{
						Chat::sendChatMessage(s->addr, L"You aren't allowed to have models in your inventory!");
					}
				}
				else{
					Chat::sendChatMessage(s->addr, L"You aren't allowed to own items with ObjectType '" + UtfConverter::FromUtf8(type) + L"'!");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Invalid LOT: '" + std::to_wstring(lot) + L"'");
			}

		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
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
		wstr << L"Position:  X:" << pos.x << "  Y:" << pos.y << "  Z:" << pos.z;
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

void ClientCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){}

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
	if (AccountsTable::getRank(s->accountid) > 0){
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
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"You don't have permission to use this command!");
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

void AdminCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() == 1){
		ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);

		unsigned short maxLevel = AccountsTable::getRank(s->accountid);
		unsigned short oldLevel = cinfo.info.gmlevel;
		std::string check = UtfConverter::ToUtf8(params->at(0));
		bool flag = isNumber(check);
		if (flag){
			unsigned short newLevel = (unsigned char)std::stoi(params->at(0));
			unsigned char success = 0;
			if (newLevel <= maxLevel){
				success = 1;
			}
			else{
				newLevel = oldLevel;
			}

			if (success == 1) CharactersTable::setGMlevel(s->activeCharId, newLevel);

			RakNet::BitStream * packet = WorldServer::initPacket(RemoteConnection::CLIENT, 16);
			packet->Write(success);
			packet->Write(maxLevel);
			packet->Write(oldLevel);
			packet->Write(newLevel);
			WorldServer::sendPacket(packet, s->addr);
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
	}
}

std::vector<std::wstring> AdminCommandHandler::getCommandNames(){
	return{ L"gmlevel" };
}

std::wstring AdminCommandHandler::getDescription(){
	return L"Sets your GM-Level";
}

std::wstring AdminCommandHandler::getShortDescription(){
	return L"Set GM-Level";
}

std::wstring AdminCommandHandler::getSyntax(){
	return L"<level>";
}

void SpawnObjectCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	if (AccountsTable::getRank(s->accountid) > 0){
		if (params->size() == 1 || params->size() == 2) {
			std::string check = UtfConverter::ToUtf8(params->at(0));
			bool flag = isNumber(check);
			if (flag){
				uint lot = std::stoi(params->at(0));

				std::string type = LOTInfo::getType(lot);
				std::string name = LOTInfo::getName(lot);

				if (type != "NOT_FOUND"){
					std::wstring name = L"";
					if (params->size() > 1)
						name = params->at(1);

					longlong objID = ObjectsTable::createObject(lot);

					PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);

					Component1 *c1 = player->getComponent1();
					COMPONENT1_POSITION position = c1->getPosition();
					COMPONENT1_ROTATION rotation = c1->getRotation();

					NPCObject *npc = new NPCObject(objID, lot, objID);

					npc->name = name;

					World *world = new World(player->world.zone, 0, 0);
					npc->world = *world;

					Component3 *c3 = npc->getComponent3();

					COMPONENT3_POSITION pos = c3->getPosition();
					pos.posX = position.x;
					pos.posY = position.y;
					pos.posZ = position.z;
					c3->setPosition(pos);

					COMPONENT3_ROTATION rot = c3->getRotation();
					rot.rotX = rotation.x;
					rot.rotY = rotation.y;
					rot.rotZ = rotation.z;
					rot.rotW = rotation.w;
					c3->setRotation(rot);

					COMPONENT7_DATA3 data3 = COMPONENT7_DATA3();
					data3.d1 = 0; data3.d2 = 0; data3.d3 = 0; data3.d4 = 0; data3.d5 = 0; data3.d6 = 0; data3.d7 = 0; data3.d8 = 0; data3.d9 = 0;

					Component7 *c7 = npc->getComponent7();
					c7->setData3(data3);

					COMPONENT7_DATA4 d4 = c7->getData4();
					d4.health = 1;
					d4.maxHealthN = 1.0;
					d4.maxHealth = 1.0;

					c7->getData4_1Ref()->push_back(-1);

					c7->setData4(d4);
					c7->setData4_4_1(true);
					c7->setData5(false);

					ulonglong id = NPCTable::registerNPC(npc);
					ObjectsManager::registerObject(npc);
					ObjectsManager::create(npc);

					Chat::sendChatMessage(s->addr, L"Successfully spawned object with LOT '" + std::to_wstring(npc->getLOT()) + L"' in your world and saved it with ID '" + std::to_wstring(id) + L"'!");
				}
				else{
					Chat::sendChatMessage(s->addr, L"Invalid LOT: '" + std::to_wstring(lot) + L"'");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else {
		Chat::sendChatMessage(s->addr, L"You don't have permission to use this command!");
	}
}

std::vector<std::wstring> SpawnObjectCommandHandler::getCommandNames() {
	return{ L"spawnobject" };
}

std::wstring SpawnObjectCommandHandler::getDescription() {
	return L"Spawns an object by LOT";
}

std::wstring SpawnObjectCommandHandler::getShortDescription() {
	return L"Spawn Object";
}

std::wstring SpawnObjectCommandHandler::getSyntax() {
	return L"<LOT>";
}

void DeleteObjectCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	if (AccountsTable::getRank(s->accountid) > 0){
		if (params->size() == 1) {
			std::string check = UtfConverter::ToUtf8(params->at(0));
			bool flag = isNumber(check);
			if (flag){
				ulonglong id = std::stoull(params->at(0));

				long long objID = NPCTable::getObjID(id);
				if (objID != -1){
					ReplicaObject *object = ObjectsManager::getObjectByID(objID);

					Component17 *c17 = (Component17*)object->getComponent(17);

					std::vector<long long> equipment = EquipmentTable::getItems(objID);
					if (equipment.size() > 0){
						for (uint i = 0; i < equipment.size(); i++){
							c17->unequipItem(equipment.at(i));
							EquipmentTable::unequipItem(objID, equipment.at(i));
							ObjectsTable::deleteObject(equipment.at(i));
						}
					}

					NPCTable::deleteNPC(id);
					ObjectsManager::unregisterObject(object);
					ObjectsManager::destruct(object);

					Chat::sendChatMessage(s->addr, L"Successfully deleted object with ID '" + std::to_wstring(id) + L"'!");
				}
				else{
					Chat::sendChatMessage(s->addr, L"There isn't an object with ID '" + std::to_wstring(id) + L"'!");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else {
		Chat::sendChatMessage(s->addr, L"You don't have permission to use this command!");
	}
}

std::vector<std::wstring> DeleteObjectCommandHandler::getCommandNames() {
	return{ L"deleteobject" };
}

std::wstring DeleteObjectCommandHandler::getDescription() {
	return L"Deletes an object by ID";
}

std::wstring DeleteObjectCommandHandler::getShortDescription() {
	return L"Delete Object";
}

std::wstring DeleteObjectCommandHandler::getSyntax() {
	return L"<ID>";
}

void EquipNPCCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	if (AccountsTable::getRank(s->accountid) > 0){
		if (params->size() == 2) {
			std::string check = UtfConverter::ToUtf8(params->at(0));
			bool flag = isNumber(check);
			check = UtfConverter::ToUtf8(params->at(1));
			if (flag) flag = isNumber(check);
			if (flag){
				ulonglong id = std::stoull(params->at(0));
				long lot = std::stoi(params->at(1));

				longlong objID = NPCTable::getObjID(id);
				if (objID != -1){
					ReplicaObject *object = ObjectsManager::getObjectByID(objID);

					Component17 *c17 = (Component17*)object->getComponent(17);

					longlong itemID = ObjectsTable::createObject(lot);
					c17->equipItem(itemID, lot, 0, 0, true);
					EquipmentTable::equipItem(objID, itemID);

					ObjectsManager::serialize(object);

					Chat::sendChatMessage(s->addr, L"Successfully added item with LOT '" + std::to_wstring(lot) + L"' to the equipment of NPC with ID '" + std::to_wstring(id) + L"'!");
				}
				else{
					Chat::sendChatMessage(s->addr, L"There isn't an object with ID '" + std::to_wstring(id) + L"'!");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else {
		Chat::sendChatMessage(s->addr, L"You don't have permission to use this command!");
	}
}

std::vector<std::wstring> EquipNPCCommandHandler::getCommandNames() {
	return{ L"equipnpc" };
}

std::wstring EquipNPCCommandHandler::getDescription() {
	return L"Equips a specific item on an NPC";
}

std::wstring EquipNPCCommandHandler::getShortDescription() {
	return L"Equip NPC";
}

std::wstring EquipNPCCommandHandler::getSyntax() {
	return L"<ID> <LOT>";
}

void UnequipNPCCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	if (AccountsTable::getRank(s->accountid) > 0){
		if (params->size() == 1) {
			std::string check = UtfConverter::ToUtf8(params->at(0));
			bool flag = isNumber(check);
			if (flag){
				ulonglong id = std::stoull(params->at(0));

				longlong objID = NPCTable::getObjID(id);
				if (objID != -1){
					std::vector<longlong> equipment = EquipmentTable::getItems(objID);

					if (equipment.size() > 0){
						ReplicaObject *object = ObjectsManager::getObjectByID(objID);

						Component17 *c17 = (Component17*)object->getComponent(17);

						for (uint i = 0; i < equipment.size(); i++){
							c17->unequipItem(equipment.at(i));
							EquipmentTable::unequipItem(objID, equipment.at(i));
							ObjectsTable::deleteObject(equipment.at(i));
						}

						ObjectsManager::serialize(object);

						Chat::sendChatMessage(s->addr, L"Successfully unequipped items of NPC with ID '" + std::to_wstring(id) + L"'!");
					}
					else{
						Chat::sendChatMessage(s->addr, L"NPC with ID '" + std::to_wstring(id) + L"' doesn't even have any items equipped!");
					}
				}
				else{
					Chat::sendChatMessage(s->addr, L"There isn't an object with ID '" + std::to_wstring(id) + L"'!");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else {
		Chat::sendChatMessage(s->addr, L"You don't have permission to use this command!");
	}
}

std::vector<std::wstring> UnequipNPCCommandHandler::getCommandNames() {
	return{ L"unequipnpc" };
}

std::wstring UnequipNPCCommandHandler::getDescription() {
	return L"Unequips all items of an NPC";
}

std::wstring UnequipNPCCommandHandler::getShortDescription() {
	return L"Unequip NPC";
}

std::wstring UnequipNPCCommandHandler::getSyntax() {
	return L"<ID>";
}

void NearMeCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	if (AccountsTable::getRank(s->accountid) > 0){
		if (params->size() == 1){
			std::string check = UtfConverter::ToUtf8(params->at(0));
			bool flag = isNumber(check);
			if (flag){
				long radius = std::stol(params->at(0));

				std::vector<NPCInfo*> npcs = NPCTable::getNPCs();

				if (npcs.size() > 0){
					std::vector<NPCInfo*> sameWorld;
					for (uint i = 0; i < npcs.size(); i++){
						sameWorld.push_back(npcs.at(i));
					}

					if (sameWorld.size() > 0){
						std::vector<NPCInfo*> nearChar;

						PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);

						Component1 *c1 = player->getComponent1();
						COMPONENT1_POSITION position = c1->getPosition();

						float posX_m_r = position.x - radius;
						float posX_p_r = position.x + radius;

						float posY_m_r = position.y - radius;
						float posY_p_r = position.y + radius;

						float posZ_m_r = position.z - radius;
						float posZ_p_r = position.z + radius;

						for (uint i = 0; i < sameWorld.size(); i++){
							if ((sameWorld.at(i)->posX >= posX_m_r && sameWorld.at(i)->posX <= posX_p_r) && (sameWorld.at(i)->posY >= posY_m_r && sameWorld.at(i)->posY <= posY_p_r) && (sameWorld.at(i)->posZ >= posZ_m_r && sameWorld.at(i)->posZ <= posZ_p_r))
								nearChar.push_back(sameWorld.at(i));
						}

						if (nearChar.size() > 0){
							std::wstringstream wss;
							wss << L"Found the following world objects within a radius of '";
							wss << radius;
							wss << L"':\n";

							for (uint i = 0; i < nearChar.size(); i++){
								std::wstring type = UtfConverter::FromUtf8(LOTInfo::getType(NPCTable::getLOT(nearChar.at(i)->objID)));
								if (type == L"Enemies")
									type = L"Enemy";
								else if (endsWith(type, L"s"))
									type = removeRight(type, 1);

								wss << L"- ";
								wss << type;
								wss << L" with ID '";
								wss << NPCTable::getID(nearChar.at(i)->objID);
								wss << L"' and LOT '";
								wss << NPCTable::getLOT(nearChar.at(i)->objID);
								wss << L"'";

								if (i != nearChar.size() - 1)
									wss << L"\n";
							}

							Chat::sendChatMessage(s->addr, wss.str());
						}
						else{
							Chat::sendChatMessage(s->addr, L"There aren't any world objects near you! (At least not within a radius of '" + std::to_wstring(radius) + L"'!)");
						}
					}
					else{
						Chat::sendChatMessage(s->addr, L"There aren't any world objects in your world!");
					}
				}
				else{
					Chat::sendChatMessage(s->addr, L"There aren't any world objects in the database!");
				}
			}
			else{
				Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /" + this->getCommandNames().at(0) + L" " + this->getSyntax());
		}
	}
	else {
		Chat::sendChatMessage(s->addr, L"You don't have permission to use this command!");
	}
}

std::vector<std::wstring> NearMeCommandHandler::getCommandNames() {
	return{ L"nearme" };
}

std::wstring NearMeCommandHandler::getDescription() {
	return L"Shows a list of world objects near you in a specific radius";
}

std::wstring NearMeCommandHandler::getShortDescription() {
	return L"List objects near you";
}

std::wstring NearMeCommandHandler::getSyntax() {
	return L"<radius>";
}