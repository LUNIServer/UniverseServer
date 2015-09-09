#pragma once
#include "serverLoop.h"

#include "legoPackets.h"
#include "CharPackets.h"
#include "Database.h"
#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"
#include "RakNet\BitStream.h"
#include "RakNet\ReplicaManager.h"

#include "Packet.h"
#include "Replica.h"
#include "GameMessages.h"
#include "Color.h"
#include "ReplicaComponents.h"
#include "PlayerObject.h"
#include "InventoryDB.h"
#include "CharactersDB.h"
#include "ServerDB.h"

#include "Account.h"
#include "Social.h"
#include "Characters.h"
#include "Worlds.h"
#include "WorldServer.h"
#include "WorldConnection.h"

#include "ChatCommands.h"

#include "zlib.h"

#include "UtfConverter.h"
#include "Logger.h"

#include <cstdlib>
#include <map>
#include <iomanip>
#include <algorithm>
using namespace std;

ReplicaManager replicaManager;
NetworkIDManager networkIdManager;

//std::map<SystemAddress, ZoneId> Player;

void WorldLoop(CONNECT_INFO* cfg) {
	// Initialize the RakPeerInterface used throughout the entire server
	RakPeerInterface* rakServer = RakNetworkFactory::GetRakPeerInterface();

	// Initialize the PacketFileLogger plugin (for the logs)
	PacketFileLogger* msgFileHandler = NULL;
	if (cfg->logFile) {
		msgFileHandler = new PacketFileLogger();
		rakServer->AttachPlugin(msgFileHandler);
	}

	// Initialize security IF user has enabled it in config.ini
	InitSecurity(rakServer, cfg->useEncryption);

	// Initialize the SocketDescriptor
	SocketDescriptor socketDescriptor(cfg->listenPort, 0);

	SystemAddress ServerAddress;
	ServerAddress.SetBinaryAddress(cfg->redirectIp);
	ServerAddress.port = cfg->listenPort;
	
	// If the startup of the server is successful, print it to the console
	// Otherwise, quit the server (as the char server is REQUIRED for the
	// server to function properly)
	if (rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		Logger::log("WRLD", "", "started! Listening on port " + std::to_string(cfg->listenPort));
		Instances::registerInstance(ServerAddress);
		//int serverinstanceid = InstancesTable::registerInstance(ServerAddress);
		//s << "WORLD SERVER STARTED ON " << ServerAddress.ToString() << " AS INSTANCE " << std::to_string(serverinstanceid) << std::endl;
	} else exit(2);

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is not NULL, save logs of char server
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\world");

	// Initialize the Packet class for the packets
	Packet* packet;

	// This will be used in the saving of packets below
	int i = 0;

	//ZoneId zone = NIMBUS_ISLE;

	// -- REPLICA MANAGER -- //
	rakServer->AttachPlugin(&replicaManager);
	rakServer->SetNetworkIDManager(&networkIdManager);

	replicaManager.SetAutoParticipateNewConnections(true);
	//replicaManager.SetAutoConstructToNewParticipants(true);
	replicaManager.SetAutoSerializeInScope(true);

	//Since we are the server, we have the ID Authority
	networkIdManager.SetIsNetworkIDAuthority(true);

	// 
	//	We can implement this, but it seems we don't have to atm.
	//	Actually we should do that at some point to find out if the client tries to create anything like minigames or so
	//
	// Set the function that will get messages from Replica::SendConstruction
	//replicaManager.SetReceiveConstructionCB(&constructionCB);
	// Set the optional callbacks to send and receive download complete notifications
	//replicaManager.SetDownloadCompleteCB(&sendDownloadCompleteCB, &receiveDownloadCompleteCB);

	//Before we start handling packets, we set this RakPeer as the world server of this instance
	WorldServer::publishWorldServer(rakServer, &replicaManager, ServerAddress);

	ChatCommandManager::registerCommands(new FlightCommandHandler());
	ChatCommandManager::registerCommands(new TeleportCommandHandler());
	ChatCommandManager::registerCommands(new WhisperCommandHandler());
	ChatCommandManager::registerCommands(new TestmapCommandHandler());
	ChatCommandManager::registerCommands(new SwitchCommandHandler());
	ChatCommandManager::registerCommands(new AddItemCommandHandler());
	ChatCommandManager::registerCommands(new ItemsCommandHandler());
	ChatCommandManager::registerCommands(new PositionCommandHandler());
	ChatCommandManager::registerCommands(new ClientCommandHandler());
	ChatCommandManager::registerCommands(new AttributeCommandHandler());
	ChatCommandManager::registerCommands(new PacketCommandHandler());
	ChatCommandManager::registerCommands(new AnnouncementCommandHandler());

	//LUNI_WRLD = true;

	// This will be used in the saving of packets below...
	while (!getTerminate()) {
		if (getTerminate()){
			std::cout << "W";
		}
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive(); // Recieve the packets from the server
		if (packet == NULL) continue; // If packet is NULL, just continue without processing anything

		// This will save all packets recieved from the client if running from DEBUG
		#ifdef DEBUG
			stringstream packetName;
			packetName << ".//Saves//World_Packet" << i << ".bin";

			if (packet->data[3] != 22) {
				SavePacket(packetName.str(), (char*)packet->data, packet->length);
				i++;
			}
		#endif

		RakNet::BitStream *pdata = new RakNet::BitStream(packet->data, packet->length, false);
		uchar packetId;
		pdata->Read(packetId);

		// Create and send packets back here according to the one we got
		switch (packetId) {
			case ID_USER_PACKET_ENUM:
			{
				//This is a normal packet, that should be parsed and responded to accordingly.
				SessionInfo session = SessionsTable::getClientSession(packet->systemAddress);
				if (session.phase > SessionPhase::PHASE_NONE){
					parsePacket(rakServer, packet->systemAddress, pdata, (unsigned long)(packet->length - 1));
				}
				else{
					Logger::log("WRLD", "CLIENT", "Recieved packet from unconnected user " + std::string(packet->systemAddress.ToString()));
				}
			}				
				break;
			case ID_NEW_INCOMING_CONNECTION:
			#ifdef DEBUG
				Logger::log("WRLD", "CLIENT", "Receiving a new connection...");
			#endif
				break;

			case ID_DISCONNECTION_NOTIFICATION:
			{
				SessionInfo session = SessionsTable::getClientSession(packet->systemAddress);
				std::string name = AccountsTable::getAccountName(session.accountid);
				//auto usr = OnlineUsers->Find(packet->systemAddress);
				//OnlineUsers->Remove(packet->systemAddress);
				Logger::log("WRLD", "CLIENT", "Disconnected " + name);
				Friends::broadcastFriendLogout(session.activeCharId);
				ObjectsManager::clientLeaveWorld(session.activeCharId, packet->systemAddress);
				//usr->DestructPlayer();
				Session::disconnect(packet->systemAddress, SessionPhase::PHASE_INWORLD);
			}
				break;
			case DefaultMessageIDTypes::ID_CONNECTION_LOST:
			{
				SessionInfo session = SessionsTable::getClientSession(packet->systemAddress);
				Logger::log("WRLD", "", "Lost connection to " + std::string(packet->systemAddress.ToString()), LOG_ERROR);
				if (session.phase >= SessionPhase::PHASE_AUTHENTIFIED){
					//auto usr = OnlineUsers->Find(packet->systemAddress);
					if (session.phase >= SessionPhase::PHASE_PLAYING){
						Friends::broadcastFriendLogout(session.activeCharId);
						ObjectsManager::clientLeaveWorld(session.activeCharId, packet->systemAddress);
						//usr->DestructPlayer();
					}
					//OnlineUsers->Remove(packet->systemAddress);
				}
				Session::disconnect(packet->systemAddress, SessionPhase::PHASE_INWORLD);
			}
				break;
			default:
				stringstream s;
				Logger::log("WRLD", "", "recieved unknown packet [" + std::to_string(packetId) + "]", LOG_DEBUG);
				Logger::log("WRLD", "", RawDataToString(packet->data, packet->length), LOG_DEBUG);
		}
		rakServer->DeallocatePacket(packet);
	}

	//InstancesTable::unregisterInstance(ServerAddress);

	Logger::log("WRLD", "", "Quitting");
	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);

	//LUNI_WRLD = false;
}

//TODO: temporary
ulong counter = 0;

//This function should get the packet WITHOUT the leading 0x53. bytelength should account for that
//Please note that it CAN be caused recursively, so the read offset should never be set to a static length
void parsePacket(RakPeerInterface* rakServer, SystemAddress &systemAddress, RakNet::BitStream *data, ulong bytelength){
	//ulong packetLength = data->GetNumberOfUnreadBits() >> 3;
	unsigned short connectionType;
	data->Read(connectionType);
	unsigned long packetId;
	data->Read(packetId);
	unsigned char padding;
	data->Read(padding);

	bool printData = false;

	switch (connectionType){
	case RemoteConnection::GENERAL:
		//Message for Handshake
		switch (packetId){
		case ServerPacketID::VERSION_CONFIRM:
		{
			DoHandshake(rakServer, systemAddress, data, "WRLD");
		}
			break;
		default:
			Logger::log("WRLD", "PACKETS", "Recieved unknown GENERAL packet (" + std::to_string(packetId) + ")"), LOG_DEBUG;
		}
		printData = true;
		break;
	case RemoteConnection::AUTH:
		//Message during AUTHENTIFICATION
		Logger::log("WRLD", "PACKETS", "Recieved unknown AUTH packet (" + std::to_string(packetId) + ")"), LOG_DEBUG;
		printData = true;
		break;
	case RemoteConnection::CHAT:
		//Chat related messages
		switch (packetId){
		case ChatPacketID::PRIVATE_CHAT_MESSAGE:
		{
			unsigned long long a0;
			unsigned char chatChannel;
			unsigned long msgLength;
			data->Read(a0);
			data->Read(chatChannel);
			data->Read(msgLength);
			std::wstring sender = PacketTools::ReadFromPacket(data, 33);
			long long sobjid;
			data->Read(sobjid);
			unsigned short u;
			data->Read(u);
			unsigned char SiM;
			data->Read(SiM);
			bool senderIsMythran = false;
			if (SiM == 1){
				senderIsMythran = true;
			}
			std::wstring reciever = PacketTools::ReadFromPacket(data, 33);
			unsigned char RiM;
			data->Read(RiM);
			bool recieverIsMythran = false;
			if (RiM == 1){
				recieverIsMythran = true;
			}
			unsigned char returnValue;
			data->Read(returnValue);
			std::wstring msg = PacketTools::ReadFromPacket(data, msgLength);
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] Private Chat Message: status " + std::to_string(returnValue), LOG_DEBUG);
			std::stringstream senders;
			senders << std::to_string(sobjid);
			if (senderIsMythran) senders << " (Mythran)";
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] SENDER:   " + senders.str(), LOG_DEBUG);
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] RECIEVER: " + UtfConverter::ToUtf8(reciever), LOG_DEBUG);
			std::string msgs = UtfConverter::ToUtf8(msg);
			if (msgs.size() > 20) msgs = msgs.substr(0, 17) + "...";
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] MESSAGE:  " + msgs, LOG_DEBUG);
		}
			break;
		case ChatPacketID::ADD_FRIEND_REQUEST:
		{
			unsigned long long unknown;
			data->Read(unknown);
			if (unknown != 0) Logger::log("WRLD", "FRIENDS_REQUEST", "8bytes not 0 please investigate", LOG_DEBUG);
			std::vector<wchar_t> input;
			bool eos = false;
			for (unsigned int k = 0; k < 33; k++){
				wchar_t chr;
				data->Read(chr);
				if (chr == 0) eos = true;
				if (!eos) input.push_back(chr);
			}
			std::wstring name(input.begin(), input.end());
			unsigned char isBestFriendRequest;
			data->Read(isBestFriendRequest);

			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);

			std::string sname = UtfConverter::ToUtf8(name);
			std::string uname = cinfo.info.name;
			Friends::sendFriendRequest(uname, sname, (isBestFriendRequest == 1));
		}
			break;
		case ChatPacketID::ADD_FRIEND_RESPONSE:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);

			long long unknown;
			data->Read(unknown);
			unsigned char status;
			data->Read(status);
			std::wstring name = PacketTools::ReadFromPacket(data, 33);
			Logger::log("WRLD", "SOCIAL", "DECLINED? " + std::to_string(status), LOG_DEBUG);
			Friends::handleFriendRequestResponse(s.activeCharId, name, status);
		}
			break;
		case ChatPacketID::GET_FRIENDS_LIST:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			Logger::log("WRLD", "PARSER", "Requesting Friends-List");
			Friends::handleWorldJoin(s.activeCharId);
			//Friends::sendFriendsList(usr->GetCurrentCharacter()->charobjid);
			//Friends::broadcastFriendLogin(usr->GetCurrentCharacter()->charobjid);
		}
			break;
		case ChatPacketID::GET_IGNORE_LIST:
			Logger::log("WRLD", "PARSER", "Requesting Ignore-List");
			//TODO: probably similar to implementing friends using 53-05-00-1e
			break;
		case ChatPacketID::TEAM_INVITE:
			Logger::log("WRLD", "PARSER", "Send Team invite", LOG_DEBUG);
			break;
		case ChatPacketID::TEAM_GET_STATUS:
			Logger::log("WRLD", "PARSER", "Requesting Team status");
			//TODO
			break;
		case ChatPacketID::REQUEST_MINIMUM_CHAT_MODE_PRIVATE:
		{
			unsigned long long a;
			unsigned char b;
			data->Read(a);
			data->Read(b);
			std::wstring pstr = PacketTools::ReadFromPacket(data, 33);
			Logger::log("WRLD", "CHAT", "Client requests private chat with " + UtfConverter::ToUtf8(pstr) + " (" + std::to_string(a) + "|" + std::to_string(b) + ")");
		}
			break;
		default:
			Logger::log("WRLD", "PACKET", "Recieved unknown CHAT packet (" + std::to_string(packetId) + ")");
			printData = true;
			break;
		}
		printData = true;
		break;
	case RemoteConnection::UNKNOWNCONN:
		//Unknown connection type
		Logger::log("WRLD", "PACKET", "Recieved unknown UNKNOWNCONN packet (" + std::to_string(packetId) + ")");
		printData = true;
		break;
	case RemoteConnection::SERVER:
		//Message to server
		switch (packetId){
		// 1
		case WorldPacketID::CLIENT_VALIDATION:
		{
			//When the world server recieves a validation packet, check it and continue
			bool flag = ClientWorldPackets::HandleValidation(data, systemAddress);
			if (!flag){
				rakServer->CloseConnection(systemAddress, true);
			}

			//When the client is validated, send him to the right world.
			//Actually, this should finally not be done here, but as a response to the char packet that sends the client to this server

			//ONLY USE WORLD FROM NOW ON
			//SessionInfo s = SessionsTable::getClientSession(systemAddress);
			//ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
			//COMPONENT1_POSITION pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
			//ZoneId zone = static_cast<ZoneId>(cinfo.lastPlace.zoneID);
			//Worlds::loadWorld(systemAddress, zone, pos, cinfo.lastPlace.mapInstance, cinfo.lastPlace.mapClone);
		}
			break;
		// 2
		case WorldPacketID::CLIENT_CHARACTER_LIST_REQUEST:
		{
			//Only happens when getting charlist ingame
			Logger::log("WORLD", "CHARS", "CLIENT_CHARACTER_LIST_REQUEST");
			Logger::log("WORLD", "CHARS", "Sending char packet...");
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			if (s.phase > SessionPhase::PHASE_PLAYING){
				ObjectsManager::clientLeaveWorld(s.activeCharId, systemAddress);
				Session::quit(s.activeCharId);
			}
			SendCharPacket(rakServer, systemAddress, s.accountid);
		}
			break;
		// 3
		case WorldPacketID::CLIENT_CHARACTER_CREATE_REQUEST:
		{
			// Find online user by systemAddress
			//auto usr = OnlineUsers->Find(packet->systemAddress);
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			bool success = false;
			if (s.phase > SessionPhase::PHASE_CONNECTED) {
				success = Characters::CreateCharacter(data, systemAddress, s.accountid);
			}
			else {
				Logger::logError("CHAR", "", "saving user", "not authentified");
			}

			// If the username is in use, do NOT send the char packet. Otherwise, send it
			if (success) {
				SendCharPacket(rakServer, systemAddress, s.accountid);
			}
		}
			break;
		// 4
		case WorldPacketID::CLIENT_LOGIN_REQUEST:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			long long objid;
			data->Read(objid);
			if (s.phase >= SessionPhase::PHASE_AUTHENTIFIED) {
				//usr->DestructPlayer();
				//usr->SetCharacter(objid);
				Session::play(s.accountid, objid);
				s = SessionsTable::getClientSession(systemAddress);
				
				ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
				COMPONENT1_POSITION pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
				ZoneId zone = static_cast<ZoneId>(cinfo.lastPlace.zoneID);
				Worlds::loadWorld(systemAddress, zone, pos, cinfo.lastPlace.mapInstance, cinfo.lastPlace.mapClone);
				
				//RakNet::BitStream worldLoad;
				//usr->LoadWorld(usr->getWorld(), &worldLoad);
				//rakServer->Send(&worldLoad, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
			}
		}
			break;
		// 5
		case WorldPacketID::CLIENT_GAME_MSG:
		{
			long long objid;
			data->Read(objid);
			
			std::stringstream ss;
			data->SetReadOffset(data->GetReadOffset() - 32);
			for (unsigned int k = 0; k < 32; k++){
				bool flag;
				data->Read(flag);
				if (flag){
					ss << " [" << (k) << "]";
				}
			}
			std::string flags = ss.str();

			unsigned short msgid;
			data->Read(msgid);

			//We ignore what was here, because bitstream seems way better to use in this case.
			//The code gave a hint that the "objid" could be the character using the object
			//Actually it is mostly the charcter, but in some cases it is not
			Logger::log("WRLD", "PARSER", "Game Message, ID: " + std::to_string(msgid), LOG_DEBUG);
			Logger::log("WRLD", "PARSER", "OBJECT-ID: " + std::to_string(objid), LOG_DEBUG);

			switch (msgid){
			case 41:
				unsigned short speedchatid;
				data->Read(speedchatid);
				Logger::log("WRLD", "SPEEDCHAT", getSpeedchatMessage(speedchatid), LOG_DEBUG);
				unsigned long long rest;
				data->Read(rest);
				if (rest > 0) data->SetReadOffset(data->GetReadOffset() - 64); // Display Data when present
				//Speedchat has more Information than I thought.
				//When an object is nearby, it sends its objid with the speedchat packet
				//This may be related to something like the quest in Avant Gardens
				//where you have to salute to the Commander
				unsigned short something;
				data->Read(something);
				if (data->GetNumberOfUnreadBits() > 0){
					unsigned long long object;
					data->Read(object);
					ObjectInformation o = getObjectInformation(object);
					Logger::log("WRLD", "SPEEDCHAT", "Object: " + std::to_string(object) + ", " + getObjectDescription(o, object));
				}
				printData = true;
				break;
			case 124:
				//Happens on world join
				break;
			case 224:
			{
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				//Move item in Inventory
				//When you move on item on top of another item, then the two change their place
				bool flag;
				data->Read(flag);
				long long objid;
				data->Read(objid);
				long long unknown;
				data->Read(unknown);
				unsigned long slot;
				data->Read(slot);

				Logger::log("WRLD", "INVENTORY", "Move obj[" + std::to_string(objid) + "] to slot '" + std::to_string(slot) + "' (" + std::to_string(unknown) + "|" + std::to_string(flag), LOG_DEBUG);
				InventoryTable::moveItemToSlot(objid, s.activeCharId, slot);

				bool end;
				for (int k = 0; k < 7; k++){
					data->Read(end);
				}
			}
				break;
			case 230:
			{
				//Delete Item from Inventory
				bool mode;
				data->Read(mode);
				unsigned long uk1;
				data->Read(uk1);
				unsigned long long uk2;
				data->Read(uk2);
				bool a1, a2, a3, a4, a5, a6, a7;
				data->Read(a1);
				data->Read(a2);
				data->Read(a3);
				data->Read(a4);
				data->Read(a5);
				data->Read(a6);
				data->Read(a7);
				long long itemid;
				data->Read(itemid);
				unsigned char uk4;
				data->Read(uk4);
				unsigned long uk5;
				data->Read(uk5);
				if (mode == true){
					Logger::log("WLRD", "INVENTORY", "Delete item " + std::to_string(objid) + " from character " + std::to_string(objid));
					InventoryTable::deleteItem(objid, itemid);
					ObjectsTable::deleteObject(itemid);
				}
			}
				break;
			case 231:
			{
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				//Equip an item
				bool flag;
				data->Read(flag);
				bool flag2;
				data->Read(flag2);
				long long objid;
				data->Read(objid);
				Logger::log("WORLD", "EQUIP", "Equip obj [" + std::to_string(objid) + "]", LOG_ALL);
				bool end;
				for (int k = 0; k < 7; k++){
					data->Read(end);
				}

				PlayerObject *player = (PlayerObject *) ObjectsManager::getObjectByID(s.activeCharId);
				if (player != NULL){
					long lot = player->getComponent17()->equipItem(objid);
					EquipmentTable::equipItem(s.activeCharId, objid);
					ObjectsManager::serialize(player);
					if (lot == LOT::LOT_JETPACK){
						RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s.activeCharId, 561);
						ef->Write((unsigned long)0x70ba);
						ef->Write((unsigned short)0x8);
						ef->Write((unsigned char)0x5);
						ef->Write((unsigned char)0x2);
						ef->Write((unsigned short)0xc);
						ef->Write((unsigned char)0x3);
						ef->Write((unsigned short)0x6c1);
						ef->Write((unsigned char)0x0);
						ef->Write((unsigned char)0x1);
						ef->Write((unsigned char)0x80);
						ef->Write((unsigned char)0x7f);
						ef->Write((unsigned long)0xa7);
						WorldServer::sendPacket(ef, systemAddress);
					}
				}
			}
				break;
			case 233:
			{
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				bool flag;
				data->Read(flag);
				bool flag2;
				data->Read(flag2);
				bool flag3;
				data->Read(flag3);
				long long objid;
				data->Read(objid);
				Logger::log("WRLD", "EQUIP", "Unequip object [" + std::to_string(objid) + "]", LOG_ALL);
				bool end;
				for (int k = 0; k < 5; k++){
					data->Read(end);
				}
				PlayerObject *player = (PlayerObject *) ObjectsManager::getObjectByID(s.activeCharId);
				if (player != NULL){
					bool un = player->getComponent17()->unequipItem(objid);
					if (!un){
						Logger::log("WRLD", "EQUIP", "ERROR: item not found", LOG_ERROR);
					}
					else{
						EquipmentTable::unequipItem(s.activeCharId, objid);
						ObjectsManager::serialize(player);
						long lot = ObjectsTable::getTemplateOfItem(objid);
						if (lot == LOT::LOT_JETPACK){
							RakNet::BitStream ef;
							WorldServerPackets::SendGameMessage(systemAddress, s.activeCharId, 561);
						}
					}
				}
			}
				break;
			case 364:
			{
				//Interaction
				//There has to be a flag somewhere in here, with the speedchat messages I found out that it is offset by one bit.
				//Normally all bits until there are 0, i'll check both sides to see if anything happens -> where the flag is
				//TODO: Investigate
				bool flag1;
				data->Read(flag1);
				data->SetReadOffset(data->GetReadOffset() - 1);
				unsigned long long something3;
				data->Read(something3);
				bool flag;
				data->Read(flag);
				unsigned long long object;
				data->Read(object);

				//std::stringstream oss;
				//data->SetReadOffset(data->GetReadOffset() - 32);
				//for (uint k = 0; k < 32; k++){
					//bool flag;
					//data->Read(flag);
					//if (flag){
					//	oss << " [" << (k) << "]";
					//}
				//}
				//std::string objectTypes = oss.str();

				//uchar something4; -- With the flag up there, this has to be just padding
				//data->Read(something4);
				
				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "INTERACT", getObjectDescription(o, object));

				bool b;
				for (unsigned char a = 0; a < 7; a++) data->Read(b);

				handleObject(o, rakServer, systemAddress);
			}
				break;
			case 505:
			{
				long long object;
				data->Read(object);
				//cout << "Object: " << object << std::endl;
				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "LOAD?", getObjectDescription(o, object));
				//Some sort of loading, L8: objid
			}
				break;
			case 530:
			{
				//Some sort of script
				//e.g. Closing a plaque
				unsigned long something;
				data->Read(something);
				unsigned long len;
				data->Read(len);
				std::vector<wchar_t> mV;
				mV.reserve(len);
				for (unsigned long k = 0; k < len; k++){
					wchar_t mC;
					data->Read(mC);
					mV.push_back(mC);
				}
				std::wstring script(mV.begin(), mV.end());
				unsigned long something2;
				data->Read(something2);
				Logger::log("WRLD", "SCRIPT", "530: " + UtfConverter::ToUtf8(script));
				//if (something > 0) cout << " [1:" << something << "]";
				//if (something2 > 0) cout << " [2:" << something2 << "]";
				//cout << endl;
			}
				break;
			case 603:
			{
				//Open Bag
				long long aobjid;
				data->Read(aobjid);
				Logger::log("WRLD", "GMSG", "Open object " + std::to_string(aobjid));
			}
				break;
			case 717:
				//This happens independant of the Zone you are in, and these are NOT the object IDs of the characters equipment.
				//It is still probably some sort of registering of Objects
				Logger::log("WRLD", "ZONELOAD", "Registering Object???", LOG_ALL);
				break;
			case 770:
			{
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				
				//Some sort of script
				//e.g. Closing a postbox
				//For this message, the objid is the ID of the postbox in question

				//Ok actually, this is called from within the LUA script for the post box
				//in L_MAIL_BOX_INTERACT.lua, with the postbox as game message object and the user later on
				//this means, that this here is the server endpoint for the lua function:
				//object:fireEventServerSide('text', object)
				//Which is exactly what we recieve here.

				unsigned long len;
				data->Read(len);
				std::vector<wchar_t> mV;
				mV.reserve(len);
				for (unsigned long k = 0; k < len; k++){
					wchar_t mC;
					data->Read(mC);
					mV.push_back(mC);
				}
				std::wstring script(mV.begin(), mV.end());
				Logger::log("WRLD", "SCRIPT", "770: " + UtfConverter::ToUtf8(script), LOG_DEBUG);
				bool f;
				for (unsigned char k = 0; k < 3; k++){
					data->Read(f);
				}

				unsigned long long object;
				data->Read(object);
				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "SCRIPT", "Object: " + getObjectDescription(o, object), LOG_DEBUG);

				bool dat;
				for (unsigned char i = 0; i < 5; i++){
					data->Read(dat);
				}

				if (script == L"toggleMail"){
					Mail::closeMailbox(s.activeCharId);
				}
			}
				break;
			case 767:
				bool isStart;
				data->Read(isStart);
				bool dat;
				for (unsigned char i = 0; i < 7; i++){
					data->Read(dat);
				}
				if (isStart) Logger::log("WRLD", "PARSER", "Camerapath started"); else Logger::log("WRLD", "PARSER", "Camerapath finished");
				break;
			case 768:
			{
				//Camera moving
				REPLICA_OBJECT_POSITION pos;
				data->Read(pos);
				std::stringstream str;
				str << "Camera-Position: (X:" << pos.posX << "|Y:" << pos.posY << "|Z:" << pos.posZ << ")";
				Logger::log("WRLD", "PARSER", str.str());
			}
				break;
			case 850: //Chat messages and commands, starting with '/'
			{
				unsigned long unknown;
				data->Read(unknown);
				unsigned long len;
				data->Read(len);
				Logger::log("WRLD", "CHAT", "Recieved Packet of Length " + std::to_string(len), LOG_ALL);
				vector<wchar_t> msgVector;
				msgVector.reserve(len);
				for (unsigned long k = 0; k < len; k++){
					wchar_t mchr;
					data->Read(mchr);
					msgVector.push_back(mchr);
				}

				std::wstring message(msgVector.begin(), msgVector.end());
				Logger::log("WRLD", "CHAT", "Recieved message: \"" + UtfConverter::ToUtf8(message) + "\"");

				if (message.substr(0, 1) == L"/"){
					std::wstring command;
					std::vector<std::wstring> params;
					unsigned char state = 0;

					for (unsigned int k = 0; k < message.length(); k++){
						wchar_t chr = message.at(k);
						if (state == 0){
							//Nothing is parsed yet
							if (chr == L'/'){ //Needs to start with '/'
								state = 1; //Set state to STATE_COMMAND
							}else{
								break;
							}
						}
						else if (state == 1){
							//We are parsing the command name
							if (chr == L' '){
								state = 2;
							}else{
								command.push_back(chr);
							}
						}
						else if (state == 2){
							//expecting commands
							if (chr == L' '){
								//We could have two modes: add an empty param or ignore more spaces
								//Aditionally we can test if the last one was already an empty one
								//To be safe for now, just ignore it
							}
							else if (chr == L'"'){ //Start of a string command
								params.push_back(L"");
								state = 4;
							}
							else{
								//Start of a normal command
								params.push_back(std::wstring(1, chr));
								state = 3;
							}
						}
						else if (state == 3){
							//Normal param
							if (chr == L' '){
								state = 2;
							}
							else{
								params.at(params.size() - 1).append(1, chr);
							}
						}
						else if (state == 4){
							//String param
							if (chr == L'"'){
								state = 2;
								//what do we do with that? It should be a parsing error when not followed by a space
							}
							else if (chr == L'\\'){
								state = 5;
							}
							else{
								params.at(params.size() - 1).append(1, chr);
							}
						}
						else if (state == 5){
							//Escape sequences
							if (chr == L'n'){
								params.at(params.size() - 1).append(L"\n");
							}
							else if (chr == L't'){
								params.at(params.size() - 1).append(L"\t");
							}
							else{
								params.at(params.size() - 1).append(1, chr);
							}
						}
					}
					Logger::log("WRLD", "CHAT", "Command: " + UtfConverter::ToUtf8(command));

					//Packet 1: 2365 (ZoneControl Object)
					//Packet 2: objid: 288300744895889662 LOT: 6635 SpawnerId: 72567767517768 (Mardolf the Orange)
					//Packet 3: objid: 288300744895889664 LOT: 6636 SpawnerId: 72567767517769 (Rad Eccles)
					//Packet 4: objid: 288300744895889669 LOT: 3495 SpawnerId: 72567767517772
					//Packet 5: objid: 288300744895899827 LOT: 6726 SpawnerId: 72567767517771
					//Packet 6: objid: 288300744895909034 LOT: 5637 SpawnerId: 3695535 NodeID: 1, Starts with Index 20 (position), x:-309.645782, y:288.356626, z: 70.64473 (Robot Dog Pet)
					//Packet 7: objid: 288300744895889606 LOT: 3646 SpawnerId: 3695538 NodeId: 3, 
					//Packet 8: objid: 288300744895930870 LOT: 9717 SpawnerId: 72567767517763,
					//Packet 9: PLAYER

					SessionInfo s = SessionsTable::getClientSession(systemAddress);
					ChatCommandManager::handleCommand(command, &s, &params);
				}
			}
				break;
			case 888:
			{
				long long object;
				data->Read(object);
				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "OBJECT", getObjectDescription(o, object));
				//Some sort of loading, L8: objid
			}
				break;
			case 1145:
			{
				bool flag;
				data->Read(flag);
				unsigned long d1;
				data->Read(d1);
				Logger::log("WRLD", "COMBAT", std::to_string(flag) + "|" + std::to_string(d1));
				PacketTools::printBytes(data, d1);
				unsigned long num1;
				data->Read(num1);
				unsigned long num2;
				data->Read(num2);
				Logger::log("WRLD", "COMBAT", std::to_string(num1) + "|" + std::to_string(num2));
			}
				break;
			case 1202:
			{
				Logger::log("WRLD", "HELP", "Smash me!", LOG_DEBUG);
			}
				break;
			case 1308:
			{
				//This handles the requirements of a minigame for the server side.
				//Starting with 8 bytes of objectId?, then 4 bytes for the data length
				//Then that with a null byte terminator?
				//Then two 4 byte values
				unsigned long long object;
				data->Read(object);

				Logger::log("WRLD", "MINIGAME", "Cost?: objid: " + std::to_string(objid), LOG_DEBUG);

				unsigned long len;
				data->Read(len);

				if (len > 0){
					std::vector<wchar_t> mV;
					mV.reserve(len);
					for (unsigned long k = 0; k < len; k++){
						wchar_t mC;
						data->Read(mC);
						mV.push_back(mC);
					}
					std::wstring script(mV.begin(), mV.end());
					//I guess this is a null terminator as it only appears with text content, but not included in length
					unsigned short nullT;
					data->Read(nullT);
					Logger::log("WRLD", "MINIGAME", "Data: " + UtfConverter::ToUtf8(script));
				}
				//These 8 bytes should be two values, since the only thing I found so far has content only in the 5th
				unsigned long dataA;
				unsigned long dataB;
				data->Read(dataA);
				data->Read(dataB);
				Logger::log("WRLD", "MINIGAME", "A: " + std::to_string(dataA) + ", B: " + std::to_string(dataB), LOG_DEBUG);
			}
				break;
			case 1419:
			{
				unsigned long long object;
				data->Read(object);
				Logger::log("WLRD", "UNKNOWN", "Objectid: " + std::to_string(object));
				break;
			}
			case 1734:
				//Level UP

				break;
			default:
				Logger::log("WRLD", "GAMEMESSAGE", "Unknown Game Message: " + std::to_string(msgid));
				break;
			}
			printData = true;
		}
			break;
		// 6
		case WorldPacketID::CLIENT_CHARACTER_DELETE_REQUEST:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			long long charid;
			data->Read(charid);
			Characters::DeleteCharacter(s.accountid, charid);
			RakNet::BitStream * bitStream = WorldServer::initPacket(RemoteConnection::CLIENT, 12);
			bitStream->Write((uchar)1); // Success?
			WorldServer::sendPacket(bitStream, systemAddress);
		}
			break;
		// 14
		case WorldPacketID::CLIENT_GENERAL_CHAT_MESSAGE:
		{
			data->SetReadOffset(data->GetReadOffset() + 24); //3 bytes channel?
			unsigned long length;
			data->Read(length);
			std::vector<wchar_t> messageVector;
			messageVector.reserve(length);
			for (unsigned long k = 0; k < length; k++){
				wchar_t mc;
				data->Read(mc);
				if (mc == 0) break; else messageVector.push_back(mc);
			}

			std::wstring message(messageVector.begin(), messageVector.end());
			
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
			
			Logger::log("WRLD", "CHAT", cinfo.info.name + ": " + UtfConverter::ToUtf8(message));
			Chat::broadcastChatMessage(s.zone, message, UtfConverter::FromUtf8(cinfo.info.name));
		}
			break;
		// 19
		case WorldPacketID::CLIENT_LEVEL_LOAD_COMPLETE:
		{
			ZoneAddress za = ClientWorldPackets::HandleLevelLoadComplete(data, systemAddress);

			Logger::log("WRLD", "LEVEL", "Client loading complete");
			Logger::log("WRLD", "LEVEL", "Replying to Character");

			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			if (s.activeCharId > 0){
				long long objid = s.activeCharId;
				ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(objid);

				COMPONENT1_POSITION pos;
				ZoneId zid = static_cast<ZoneId>(za.zoneid);
				if (za.zoneid == cinfo.lastPlace.zoneID){
					pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
					if (pos.isNull()) pos = getZoneSpawnPoint(zid);
				}
				else{
					pos = getZoneSpawnPoint(zid);
				}

				WorldServerPackets::CreateCharacter(systemAddress, objid);

				PlayerObject * player = new PlayerObject(objid, UtfConverter::FromUtf8(cinfo.info.name));

				//Temporarily ?
				player->gmlevel = AccountsTable::getRank(s.accountid);
				player->world.zone = zid;

				Component1 * c1 = player->getComponent1();
				c1->setPosition(pos);

				Component4 * c4 = player->getComponent4();
				c4->setLevel(6);
				PLAYER_INFO pi;
				pi.accountID = s.accountid;
				pi.isFreeToPlay = cinfo.info.isFreeToPlay;
				pi.legoScore = 600;
				c4->setInfo(pi);
				PLAYER_STYLE ps;
				ps.eyebrowsStyle = cinfo.style.eyebrows;
				ps.eyesStyle = cinfo.style.eyes;
				ps.hairColor = cinfo.style.hairColor;
				ps.hairStyle = cinfo.style.hairStyle;
				ps.mouthStyle = cinfo.style.mouth;
				ps.pantsColor = cinfo.style.pantsColor;
				ps.shirtColor = cinfo.style.shirtColor;
				c4->setStyle(ps);

				Component7 * c7 = player->getComponent7();
				COMPONENT7_DATA4 d4 = c7->getData4();
				d4.health = 5;
				d4.maxHealthN = 5.0F;
				d4.maxHealth = 5.0F;
				c7->setData4(d4);

				Component17 * c17 = player->getComponent17();
				std::vector<long long> equip = EquipmentTable::getItems(objid);
				for (unsigned int k = 0; k < equip.size(); k++){
					c17->equipItem(equip.at(k));
				}

				//usr->SetPlayer(player);

				ObjectsManager::clientJoinWorld(player, systemAddress);
				//player->doCreation(packet->systemAddress, zid);

				WorldServerPackets::SendGameMessage(systemAddress, objid, 1642);
				WorldServerPackets::SendGameMessage(systemAddress, objid, 509);

				RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(objid, 472);
				pc->Write((unsigned long)185);
				pc->Write((unsigned char)0);
				WorldServer::sendPacket(pc, systemAddress);

				Session::enter(s.activeCharId, zid);
				Logger::log("WRLD", "PARSER", "Client: Level loading complete " + zid);
			}			
		}
			break;
		// 21
		case WorldPacketID::CLIENT_ROUTE_PACKET:
		{
			unsigned long subPacketLength;
			data->Read(subPacketLength);
			Logger::log("WRLD", "PARSER", "Recieved routing packet; Length: " + std::to_string(subPacketLength));
			parsePacket(rakServer, systemAddress, data, subPacketLength);
		}
			break;
		// 22
		case WorldPacketID::CLIENT_POSITION_UPDATE: //user moving / update request?
		{
			SessionInfo i = SessionsTable::getClientSession(systemAddress);
			if (i.phase > SessionPhase::PHASE_PLAYING){
				PlayerObject * player = (PlayerObject *)ObjectsManager::getObjectByID(i.activeCharId);

				if (player != NULL){
					Component1 *c1 = player->getComponent1();

					float x, y, z;
					data->Read(x);
					data->Read(y);
					data->Read(z);
					c1->setPosition(COMPONENT1_POSITION(x, y, z));

					float rx, ry, rz, rw;
					data->Read(rx);
					data->Read(ry);
					data->Read(rz);
					data->Read(rw);
					c1->setRotation(COMPONENT1_ROTATION(rx, ry, rz, rw));

					bool onGround;
					data->Read(onGround);
					c1->setOnGround(onGround);


					bool unknownBit;
					data->Read(unknownBit);
					c1->setData6_d4(unknownBit);
					bool hasVelocityInfo;
					data->Read(hasVelocityInfo);

					if (hasVelocityInfo){
						float vx, vy, vz;
						data->Read(vx);
						data->Read(vy);
						data->Read(vz);
						c1->setVelocity(COMPONENT1_VELOCITY(vx, vy, vz));
					}
					else{
						c1->setVelocity(COMPONENT1_VELOCITY(0, 0, 0));
					}

					bool hasAngularVelocityInfo;
					data->Read(hasAngularVelocityInfo);

					if (hasAngularVelocityInfo){
						float avx, avy, avz;
						data->Read(avx);
						data->Read(avy);
						data->Read(avz);
						c1->setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR(avx, avy, avz));
					}
					else{
						c1->setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR(0, 0, 0));
					}
					ObjectsManager::serialize(player); //player->serialize();
				}
				else{
					//Player is null????
				}
			}
		}
			break;
		// 23
		case WorldPacketID::CLIENT_MAIL:
		{
			long mailid;
			data->Read(mailid);
			switch (mailid){
			case 0:
			{
				std::wstring subject = PacketTools::ReadFromPacket(data, 50);
				std::wstring text = PacketTools::ReadFromPacket(data, 400);
				std::wstring recipient = PacketTools::ReadFromPacket(data, 32);
				long long d1;
				data->Read(d1);
				long long attach;
				data->Read(attach);
				unsigned short attcount;
				data->Read(attcount);
				unsigned short lang;
				data->Read(lang);
				unsigned long d3;
				data->Read(d3);

				Logger::log("WLRD", "MAIL", "Send Mail Request", LOG_DEBUG);
				Logger::log("WLRD", "MAIL", "TO:           " + UtfConverter::ToUtf8(recipient), LOG_ALL);
				Logger::log("WLRD", "MAIL", "SUBJECT:      " + UtfConverter::ToUtf8(subject), LOG_ALL);
				Logger::log("WLRD", "MAIL", "TEXT:         " + UtfConverter::ToUtf8(text), LOG_ALL);
				Logger::log("WLRD", "MAIL", "LANGUAGE:     " + std::to_string(lang), LOG_ALL);
				Logger::log("WLRD", "MAIL", "ATTACHMENT:   " + std::to_string(attach) + "[" + std::to_string(attcount) + "]", LOG_ALL);
				Logger::log("WLRD", "MAIL", "D1/D3:        " + std::to_string(d1) + "/" + std::to_string(d3), LOG_ALL);

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				long long charid = s.activeCharId;
				Mail::requestMailSending(charid, subject, text, recipient, attach, attcount);
			}
				break;
			case 3:
			{
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				Mail::loadMailboxData(s.activeCharId);
			}
				break;
			case 5:
			{
				unsigned long ua;
				data->Read(ua);

				long long mid;
				data->Read(mid);
				long long objid;
				data->Read(objid);
				Logger::log("WRLD", "MAIL", "Requesting remove attachment for mail id " + std::to_string(mid), LOG_DEBUG);
				Logger::log("WRLD", "MAIL", std::to_string(ua), LOG_DEBUG);
				Mail::removeAttachment(mid, objid);
			}
				break;
			case 7:
			{
				long d4;
				data->Read(d4);
				long long mid;
				data->Read(mid);
				long long charid;
				data->Read(charid);
				Mail::deleteMail(mid, charid);
			}
				break;
			case 9:
			{
				long d2;
				data->Read(d2);
				long long mid;
				data->Read(mid);
				MailsTable::setIsRead(mid);
				Logger::log("WRLD", "MAIL", "Mail Read: " + std::to_string(mid), LOG_DEBUG);
			}
				break;
			default:
				Logger::log("WLRD", "MAIL", "Mail ID: " + std::to_string(mailid), LOG_DEBUG);
				printData = true;
			}
		}
			break;
		// 25
		case WorldPacketID::CLIENT_STRING_CHECK:
		{
			unsigned char superChatLevel;
			data->Read(superChatLevel);
			unsigned char requestID;
			data->Read(requestID);
			std::wstring reciever = PacketTools::ReadFromPacket(data, 42);
			unsigned short length;
			data->Read(length);
			std::wstring text = PacketTools::ReadFromPacket(data, length);

			std::stringstream str;
			str << "Moderation Request #" << std::to_string(requestID);
			if (reciever != L""){
				str << " for " << UtfConverter::ToUtf8(reciever);
			}
			str << ": '";
			if (text.size() > 20){
				str << UtfConverter::ToUtf8(text.substr(0, 17)) << "...";
			}
			else{
				str << UtfConverter::ToUtf8(text);
			}
			str << "'";
			Logger::log("WRLD", "MODR", str.str(), LOG_DEBUG);

			RakNet::BitStream  * bs = WorldServer::initPacket(RemoteConnection::CLIENT, 59);
			bs->Write((uchar)1);
			bs->Write((ushort)0);
			bs->Write(requestID);
			WorldServer::sendPacket(bs, systemAddress);
		}
			break;
		// 120
		case WorldPacketID::UGC_DOWNLOAD_FAILED:
		{
			long d1;
			data->Read(d1);
			long long objid;
			data->Read(objid);
			long d2;
			data->Read(d2);
			long long charid;
			data->Read(charid);
			Logger::log("WLRD", "UGC", "Download failed (" + std::to_string(d1) + "|" + std::to_string(d2) + ")", LOG_WARNING);
			Logger::log("WLRD", "UGC", "Object: " + std::to_string(objid), LOG_WARNING);
			Logger::log("WLRD", "UGC", "Player: " + std::to_string(charid), LOG_WARNING);
		}
			break;
		default:
			Logger::log("WLRD", "PACKETS", "Recieved unknown SERVER packet (" + std::to_string(packetId) + ")", LOG_DEBUG);
			printData = true;
			break;
		}
		break;
	case RemoteConnection::CLIENT:
		//Message to client
		//This connection should never occur as it is a message to the client which this program never functions as
		Logger::log("WLRD", "PACKETS", "Recieved unknown SERVER packet (" + std::to_string(packetId) + ")", LOG_DEBUG);
		printData = true;
		break;
	}

	if (printData){
		if (data->GetNumberOfUnreadBits() > 0){
			PacketTools::printRest(data);
			//cout << RawDataToString(data->GetData(), data->GetNumberOfBytesUsed(), false, (data->GetReadOffset() >> 3)) << endl;
		}else{
			Logger::log("DATA", "", "No more data");
		}
		
	}
}

bool handleObject(ObjectInformation obj, RakPeerInterface* rakServer, SystemAddress &systemAddress){
	if (obj.type != ObjectType::NONE){
		switch (obj.type)
		{
		case ObjectType::LaunchPad:
		{
			ZoneId zone = getLaunchPadTarget(obj);
			if (zone != ZoneId::NO_ZONE && zone != ZoneId::KEELHAUL_CANYON){
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				COMPONENT1_POSITION pos = getZoneSpawnPoint(zone, static_cast<ZoneId>(s.zone));
				bool flag = Worlds::loadWorld(systemAddress, zone, pos, 0, 0);
				if (flag){
					Session::leave(s.activeCharId);

					WorldPlace place;
					place.zoneID = zone;
					place.mapClone = 0;
					place.mapInstance = 0;
					place.x = pos.x;
					place.y = pos.y;
					place.z = pos.z;
					CharactersTable::setCharactersPlace(s.activeCharId, place);

					ObjectsManager::clientLeaveWorld(s.activeCharId, systemAddress);
					//usr->DestructPlayer();
				}
			}
		}
			break;
		case ObjectType::PostBox:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			//Chat::sendMythranInfo(usr->GetCurrentCharacter()->charobjid, "We are sorry to inform you the Mail system isn't working yet!", "Our deepest apologies");
			Mail::openMailbox(s.activeCharId);
		}
			break;
		default:
			return false;
			break;
		}
	}
	return true;
}
