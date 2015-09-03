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
				Chat::sendChatMessage(s->addr, L"/" + ChatCommandManager::ChatCommandHandlers.at(k)->getCommandNames().at(0) + L"\t\t" + ChatCommandManager::ChatCommandHandlers.at(k)->getShortDescription());
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