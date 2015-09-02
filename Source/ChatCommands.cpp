#include "ChatCommands.h"
#include "Social.h"
#include "WorldConnection.h"
#include "WorldServer.h"

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
	std::unordered_map<std::wstring, ChatCommandHandler *>::iterator it = ChatCommandManager::ChatCommands.find(command);
	if (it != ChatCommandManager::ChatCommands.end()){
		it->second->handleCommand(s, params);
	}else{
		Chat::sendChatMessage(s->addr, L"Command " + command + L" does not exist!");
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
	return{ L"tele", L"teleport", L"ot"};
}

std::wstring TeleportCommandHandler::getDescription(){
	return L"Teleports a player to the specified location or 0|0|0";
}
std::wstring TeleportCommandHandler::getShortDescription(){
	return L"teleport player";
}
std::wstring TeleportCommandHandler::getSyntax(){
	return L"[<x> <y> <z>]";
}