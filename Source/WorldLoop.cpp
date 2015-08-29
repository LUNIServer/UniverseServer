#pragma once
#include "serverLoop.h"

#include "legoPackets.h"
#include "CharPackets.h"
#include "Character.h"
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
#include "InventoryDB.h"
#include "CharactersDB.h"
#include "ServerDB.h"

#include "Account.h"
#include "Social.h"
#include "Worlds.h"
#include "WorldServer.h"
#include "WorldConnection.h"

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
Ref< UsersPool > WorldOnlineUsers;

void WorldLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< string > > OutputQueue) {
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

	std::stringstream straddr;
	straddr << cfg->redirectIp << ":" << cfg->listenPort;
	SystemAddress ServerAddress;
	ServerAddress.SetBinaryAddress(straddr.str().data());
	
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

	WorldOnlineUsers = OnlineUsers;

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

	ReplicaComponent *cp = new Component1();
	cp->writeToPacket(new RakNet::BitStream(), REPLICA_PACKET_TYPE::REPLICA_CONSTRUCTION_PACKET);

	//Before we start handling packets, we set this RakPeer as the world server of this instance
	WorldServer::publishWorldServer(rakServer);

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
				//This is a normal packet, that should be parsed and responded to accordingly.
				
				//This flag is temporary and should be used to while transfering all logic from here to the parser function
				bool flagUseParser;
				flagUseParser = false;
				//-------------------------
				
				ushort networkType;
				pdata->Read(networkType);
				ulong packetType;
				pdata->Read(packetType);
				uchar pad;
				pdata->Read(pad);

				switch (networkType) {
					case SERVER:
						switch (packetType) {
							case CLIENT_VALIDATION:
							{
								//When the world server recieves a validation packet, check it and continue
								bool flag = ClientWorldPackets::HandleValidation(pdata, packet->systemAddress);
								if (flag){
									//When the client is validated, send him to the right world.
									//Actually, this should finally not be done here, but as a response to the char packet that sends the client to this server
									SessionInfo s = SessionsTable::getClientSession(packet->systemAddress);
									ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
									COMPONENT1_POSITION pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
									ZoneId zone = static_cast<ZoneId>(cinfo.lastPlace.zoneID);
									Worlds::loadWorld(packet->systemAddress, zone, pos, cinfo.lastPlace.mapInstance, cinfo.lastPlace.mapClone);
								}else{
									rakServer->CloseConnection(packet->systemAddress, true);
								}
							}
								break;
							case CLIENT_LEVEL_LOAD_COMPLETE:
							{
								ZoneAddress za = ClientWorldPackets::HandleLevelLoadComplete(pdata, packet->systemAddress);
								
								//------
								auto usr = OnlineUsers->Find(packet->systemAddress);
								//std::pair<std::map<SystemAddress, ZoneId>::iterator, bool> ret;
								//ret = Player.insert(std::pair<SystemAddress, ZoneId>(packet->systemAddress, usr->getWorld()));
								//if (ret.second == false) {
								//	Player.at(packet->systemAddress) = usr->getWorld();
								//}
								//------

								Logger::log("WRLD", "LEVEL", "Client loading complete");
								Logger::log("WRLD", "LEVEL", "Replying to Character");

								SessionInfo s = SessionsTable::getClientSession(packet->systemAddress);
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

									WorldServerPackets::CreateCharacter(packet->systemAddress, objid);

									PlayerObject * player = new PlayerObject(objid, UtfConverter::FromUtf8(cinfo.info.name));
									
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

									usr->SetPlayer(player);
									player->doCreation(packet->systemAddress, zid);

									WorldServerPackets::SendGameMessage(packet->systemAddress, objid, 1642);
									WorldServerPackets::SendGameMessage(packet->systemAddress, objid, 509);

									RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(objid, 472);
									pc->Write((unsigned long)185);
									pc->Write((unsigned char)0);
									WorldServer::sendPacket(pc, packet->systemAddress);
								}
								flagUseParser = true;
							}
								break;

							case CLIENT_STRING_CHECK:
							{
								unsigned char superChatLevel;
								pdata->Read(superChatLevel);
								unsigned char requestID;
								pdata->Read(requestID);
								std::wstring reciever = PacketTools::ReadFromPacket(pdata, 42);
								unsigned short length;
								pdata->Read(length);
								std::wstring text = PacketTools::ReadFromPacket(pdata, length);
								
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
								WorldServer::sendPacket(bs, packet->systemAddress);
							}
								break;

							case CLIENT_POSITION_UPDATE: //user moving / update request?
							{
								SessionInfo i = SessionsTable::getClientSession(packet->systemAddress);
								if (i.phase > SessionPhase::PHASE_NONE){
									auto usr = OnlineUsers->Find(packet->systemAddress);
									PlayerObject *player = usr->GetPlayer();
									if (player != NULL){
										Component1 *c1 = player->getComponent1();
										RakNet::BitStream *data = new RakNet::BitStream(packet->data, packet->length, false);
										data->SetReadOffset(64);

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
										player->serialize();
									}

								}
							}
								break;
							default:
								flagUseParser = true;
						}

						break;

					default:
						flagUseParser = true;
						
						/*stringstream s;
						s << "\n[WRLD] received unknown packet: " << RawDataToString(packet->data, packet->length) << endl;
						OutputQueue->Insert(s.str());*/
				}
				
				//End of the normal packet parsing.
				if (flagUseParser){
					RakNet::BitStream *data = new RakNet::BitStream(packet->data, packet->length, false);
					uchar packetId;
					data->Read(packetId);
					if (packetId == ID_USER_PACKET_ENUM){
						auto usr = OnlineUsers->Find(packet->systemAddress);
						if (usr != NULL){
							parsePacket(rakServer, packet->systemAddress, data, (ulong)(packet->length - 1), usr);
						}
						else{
							Logger::log("WRLD", "CLIENT", "Recieved packet from unconnected user " + std::string(packet->systemAddress.ToString()));
						}
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
				auto usr = OnlineUsers->Find(packet->systemAddress);
				if (OnlineUsers->Remove(packet->systemAddress))
					Logger::log("WRLD", "CLIENT", "Disconnected " + usr->GetUsername());
				Friends::broadcastFriendLogout(usr->GetCurrentCharacter()->charobjid);
				usr->DestructPlayer();
				//Player.erase(packet->systemAddress);
				Session::disconnect(packet->systemAddress, SessionPhase::PHASE_INWORLD);
			}
				break;
			case DefaultMessageIDTypes::ID_CONNECTION_LOST:
			{
				SessionInfo session = SessionsTable::getClientSession(packet->systemAddress);
				Logger::log("WRLD", "", "Lost connection to " + std::string(packet->systemAddress.ToString()), LOG_ERROR);
				if (session.phase >= SessionPhase::PHASE_AUTHENTIFIED){
					auto usr = OnlineUsers->Find(packet->systemAddress);
					if (session.phase >= SessionPhase::PHASE_PLAYING){
						Friends::broadcastFriendLogout(session.activeCharId);
						usr->DestructPlayer();
						//Player.erase(packet->systemAddress);
					}
					if (OnlineUsers->Remove(packet->systemAddress)) {}
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
void parsePacket(RakPeerInterface* rakServer, SystemAddress &systemAddress, RakNet::BitStream *data, ulong bytelength, Ref<User> usr){
	//ulong packetLength = data->GetNumberOfUnreadBits() >> 3;
	ushort connectionType;
	data->Read(connectionType);
	ulong packetId;
	data->Read(packetId);
	uchar padding;
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
			cout << "[WRLD] Recieved unknown GENERAL packet (" << packetId << ")" << endl;
		}
		printData = true;
		break;
	case RemoteConnection::AUTH:
		//Message during AUTHENTIFICATION
		cout << "[WRLD] Recieved unknown AUTH packet (" << packetId << ")" << endl;
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
			std::cout << "[WRLD] [CHAT] [" << std::to_string(chatChannel) << "] Private Chat Message: status " << std::to_string(returnValue) << std::endl;
			std::cout << "[WRLD] [CHAT] [" << std::to_string(chatChannel) << "] SENDER: ";
			std::wcout << sender;
			std::cout << " [" << std::to_string(sobjid) << "]";
			if (senderIsMythran) std::cout << " Mythran";
			std::cout << std::endl;
			std::cout << "[WRLD] [CHAT] [" << std::to_string(chatChannel) << "] RECIEVER: ";
			std::wcout << reciever;
			if (senderIsMythran) std::cout << " Mythran";
			std::cout << std::endl;
			std::cout << "[WRLD] [CHAT] [" << std::to_string(chatChannel) << "] MESSAGE: ";
			std::wcout << msg;
			std::cout << std::endl;
		}
			break;
		case ChatPacketID::ADD_FRIEND_REQUEST:
		{
			ulonglong unknown;
			data->Read(unknown);
			if (unknown != 0) cout << "8bytes not 0 please investigate";
			//ushort lengthU;
			//data->Read(lengthU);
			vector<wchar_t> input;
			bool eos = false;
			for (uint k = 0; k < 33; k++){
				wchar_t chr;
				data->Read(chr);
				if (chr == 0) eos = true;
				if (!eos) input.push_back(chr);
			}
			std::wstring name(input.begin(), input.end());
			uchar isBestFriendRequest;
			data->Read(isBestFriendRequest);

			std::string sname = UtfConverter::ToUtf8(name);
			std::string uname = usr->GetCurrentCharacter()->GetName();
			Friends::sendFriendRequest(uname, sname, (isBestFriendRequest == 1));
		}
			break;
		case ChatPacketID::ADD_FRIEND_RESPONSE:
		{
			long long unknown;
			data->Read(unknown);
			unsigned char status;
			data->Read(status);
			std::wstring name = PacketTools::ReadFromPacket(data, 33);
			std::cout << "[WRLD] [SOCIAL] DECLINED? " << std::to_string(status) << std::endl;
			Friends::handleFriendRequestResponse(usr->GetCurrentCharacter()->charobjid, name, status);
		}
			break;
		case ChatPacketID::GET_FRIENDS_LIST:
			Logger::log("WRLD", "PARSER", "Requesting Friends-List");
			//TODO: Please implement using 53-05-00-1e
			Friends::handleWorldJoin(usr->GetCurrentCharacter()->charobjid);
			//Friends::sendFriendsList(usr->GetCurrentCharacter()->charobjid);
			//Friends::broadcastFriendLogin(usr->GetCurrentCharacter()->charobjid);
			break;
		case ChatPacketID::GET_IGNORE_LIST:
			Logger::log("WRLD", "PARSER", "Requesting Ignore-List");
			//TODO: probably similar to implementing friends using 53-05-00-1e
			break;
		case ChatPacketID::TEAM_INVITE:
			cout << "[WRLD] Send Team Invite" << endl;

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
			std::cout << "[WRLD] [CHAT] Client requests private chat with ";
			std::wstring pstr = PacketTools::ReadFromPacket(data, 33);
			std::wcout << pstr;
			std::cout << " (" << std::to_string(a) << "|" << std::to_string(b) << ")";
			std::cout << std::endl;
		}
			break;
		default:
			cout << "[WRLD] Recieved unknown CHAT packet (" << packetId << ")" << endl;
			printData = true;
			break;
		}
		printData = true;
		break;
	case RemoteConnection::UNKNOWNCONN:
		//Unknown connection type
		cout << "[WRLD] Recieved unknown UNKNOWNCONN packet (" << packetId << ")" << endl;
		printData = true;
		break;
	case RemoteConnection::SERVER:
		//Message to server
		switch (packetId){
		case WorldPacketID::CLIENT_CHARACTER_LIST_REQUEST:
		{
			//Only happens when getting charlist ingame
			std::cout << "[CHAR] CLIENT_CHARACTER_LIST_REQUEST" << std::endl;
			std::cout << "[CHAR] Sending char packet..." << std::endl;
			SendCharPacket(rakServer, systemAddress, usr->GetID());
			break;
		}
		case WorldPacketID::CLIENT_LOGIN_REQUEST:
			long long objid;
			data->Read(objid);
			if (usr != NULL) {
				usr->DestructPlayer();
				usr->SetCharacter(objid);
				Session::play(usr->GetID(), objid);
				RakNet::BitStream worldLoad;
				usr->LoadWorld(usr->getWorld(), &worldLoad);
				rakServer->Send(&worldLoad, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
			}
			break;
		case WorldPacketID::CLIENT_ROUTE_PACKET:
			ulong subPacketLength;
			data->Read(subPacketLength);
			Logger::log("WRLD", "PARSER", "Recieved routing packet; Length: " + std::to_string(subPacketLength));
			parsePacket(rakServer, systemAddress, data, subPacketLength, usr);
			break;
		case WorldPacketID::CLIENT_GAME_MSG:
		{
			longlong objid;
			data->Read(objid);
			
			std::stringstream ss;
			data->SetReadOffset(data->GetReadOffset() - 32);
			for (uint k = 0; k < 32; k++){
				bool flag;
				data->Read(flag);
				if (flag){
					ss << " [" << (k) << "]";
				}
			}
			std::string flags = ss.str();

			ushort msgid;
			data->Read(msgid);
#ifdef DEBUG
			//We ignore what was here, because bitstream seems way better to use in this case.
			//The code gave a hint that the "objid" could be the character using the object
			//Actually it is mostly the charcter, but in some cases it is not
			Logger::log("WRLD", "PARSER", "Game Message, ID: " + std::to_string(msgid));
			Logger::log("WRLD", "PARSER", "OBJECT-ID: " + std::to_string(objid));
#endif
			switch (msgid){
			case 41:
				ushort speedchatid;
				data->Read(speedchatid);
				cout << "[WRLD] Speedchat: " << getSpeedchatMessage(speedchatid) << endl;
				ulonglong rest;
				data->Read(rest);
				if (rest > 0) data->SetReadOffset(data->GetReadOffset() - 64); // Display Data when present
				//Speedchat has more Information than I thought.
				//When an object is nearby, it sends its objid with the speedchat packet
				//This may be related to something like the quest in Avant Gardens
				//where you have to salute to the Commander
				ushort something;
				data->Read(something);
				if (data->GetNumberOfUnreadBits() > 0){
					ulonglong object;
					data->Read(object);
					cout << "Object: " << object << std::endl;
					ObjectInformation o = getObjectInformation(object);
					cout << getObjectDescription(o, object) << endl;
				}

				printData = true;
				break;
			case 124:
				//Happens on world join
				break;
			case 224:
			{
				//Move item in Inventory
				//When you move on item on top of another item, then the two change their place
				bool flag;
				data->Read(flag);
				long long objid;
				data->Read(objid);
				long long unknown;
				data->Read(unknown);
				ulong slot;
				data->Read(slot);

				std::cout << "[WRLD] Move obj [" << objid << "] to slot '" << slot << "' (" << unknown << "|" << flag << ") ";
				InventoryTable::moveItemToSlot(objid, usr->GetCurrentCharacter()->charobjid, slot);

				bool end;
				for (int k = 0; k < 7; k++){
					data->Read(end);
					if (end) cout << k;
				}
				cout << std::endl;
			}
				break;
			case 231:
			{
				//Equip an item
				bool flag;
				data->Read(flag);
				bool flag2;
				data->Read(flag2);
				long long objid;
				data->Read(objid);
				std::cout << "[WRLD] [CS] Equip obj [" << objid << "](" << flag << "|" << flag2 << ") ";
				bool end;
				for (int k = 0; k < 7; k++){
					data->Read(end);
					if (end) cout << k;
				}
				std::cout << std::endl;

				PlayerObject *player = usr->GetPlayer();
				if (player != NULL){
					long lot = player->getComponent17()->equipItem(objid);
					EquipmentTable::equipItem(usr->GetCurrentCharacter()->charobjid, objid);
					player->serialize();

					if (lot == LOT::LOT_JETPACK){
						RakNet::BitStream ef;
						sendGameMessage(usr->GetCurrentCharacter()->charobjid, &ef, 561);
						ef.Write((ulong)0x70ba);
						ef.Write((ushort)0x8);
						ef.Write((uchar)0x5);
						ef.Write((uchar)0x2);
						ef.Write((ushort)0xc);
						ef.Write((uchar)0x3);
						ef.Write((ushort)0x6c1);
						ef.Write((uchar)0x0);
						ef.Write((uchar)0x1);
						ef.Write((uchar)0x80);
						ef.Write((uchar)0x7f);
						ef.Write((ulong)0xa7);
						rakServer->Send(&ef, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
					}
				}
			}
				break;
			case 233:
			{
				bool flag;
				data->Read(flag);
				bool flag2;
				data->Read(flag2);
				bool flag3;
				data->Read(flag3);
				long long objid;
				data->Read(objid);
				std::cout << "[WRLD] [CS] Unquip obj [" << objid << "] (" << flag << "|" << flag2 << "|" << flag3 << ") ";
				bool end;
				for (int k = 0; k < 5; k++){
					data->Read(end);
					if (end) cout << k;
				}
				std::cout << std::endl;
				PlayerObject *player = usr->GetPlayer();
				if (player != NULL){
					bool un = player->getComponent17()->unequipItem(objid);
					if (!un){
						std::cout << "[WRLD] [CS] ERROR: Item not found!" << std::endl;
					}
					else{
						EquipmentTable::unequipItem(usr->GetCurrentCharacter()->charobjid, objid);
						player->serialize();
						long lot = ObjectsTable::getTemplateOfItem(objid);
						if (lot == LOT::LOT_JETPACK){
							RakNet::BitStream ef;
							sendGameMessage(usr->GetCurrentCharacter()->charobjid, &ef, 561);
							rakServer->Send(&ef, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
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
				ulonglong something3;
				data->Read(something3);
				bool flag;
				data->Read(flag);
				ulonglong object;
				data->Read(object);

				std::stringstream oss;
				data->SetReadOffset(data->GetReadOffset() - 32);
				for (uint k = 0; k < 32; k++){
					bool flag;
					data->Read(flag);
					if (flag){
						oss << " [" << (k) << "]";
					}
				}
				std::string objectTypes = oss.str();

				//uchar something4; -- With the flag up there, this has to be just padding
				//data->Read(something4);
				cout << "[WRLD] Interaction: "; //objid: " << object << objectTypes;
				ObjectInformation o = getObjectInformation(object);
				cout << getObjectDescription(o, object) << endl;
				if (something3 > 0) cout << " [1:" << something3 << "]";
				if (flag1) cout << " [FLAG:A]";
				if (flag) cout << " [FLAG:B]";
				if ((something3 > 0) || flag1 || flag) cout << std::endl;

				bool b;
				for (uchar a = 0; a < 7; a++) data->Read(b);

				handleObject(o, rakServer, systemAddress, usr);
			}
				break;
			case 505:
			{
				long long object;
				data->Read(object);
				//cout << "Object: " << object << std::endl;
				ObjectInformation o = getObjectInformation(object);
				cout << getObjectDescription(o, object) << endl;
				//Some sort of loading, L8: objid
			}
				break;
			case 530:
			{
				//Some sort of script
				//e.g. Closing a plaque
				ulong something;
				data->Read(something);
				ulong len;
				data->Read(len);
				vector<wchar_t> mV;
				mV.reserve(len);
				for (ulong k = 0; k < len; k++){
					wchar_t mC;
					data->Read(mC);
					mV.push_back(mC);
				}
				std::wstring script(mV.begin(), mV.end());
				ulong something2;
				data->Read(something2);
				cout << "[WRLD] Script Command (530): '";
				wcout << script;
				cout << "'";
				if (something > 0) cout << " [1:" << something << "]";
				if (something2 > 0) cout << " [2:" << something2 << "]";
				cout << endl;
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
				cout << "Registering object?!" << endl;
				break;
			case 770:
			{
				//Some sort of script
				//e.g. Closing a postbox
				//For this message, the objid is the ID of the postbox in question

				//Ok actually, this is called from within the LUA script for the post box
				//in L_MAIL_BOX_INTERACT.lua, with the postbox as game message object and the user later on
				//this means, that this here is the server endpoint for the lua function:
				//object:fireEventServerSide('text', object)
				//Which is exactly what we recieve here.

				ulong len;
				data->Read(len);
				vector<wchar_t> mV;
				mV.reserve(len);
				for (ulong k = 0; k < len; k++){
					wchar_t mC;
					data->Read(mC);
					mV.push_back(mC);
				}
				std::wstring script(mV.begin(), mV.end());
				cout << "[WRLD] Script Command (770): '";
				wcout << script;
				cout << "'" << std::endl;

				std::stringstream ss;
				ss << "Flags:";
				for (uchar k = 0; k < 3; k++){
					bool f;
					data->Read(f);
					if (f) ss << " [" << k << "]";
				}
				std::string flags = ss.str();
				if (flags.length() > 6){
					cout << flags << endl;
				}

				ulonglong object;
				data->Read(object);
				//cout << "objid:" << object << endl;
				ObjectInformation o = getObjectInformation(object);
				cout << getObjectDescription(o, object) << endl;

				bool dat;
				for (uchar i = 0; i < 5; i++){
					data->Read(dat);
				}

				if (script == L"toggleMail"){
					Mail::closeMailbox(usr->GetCurrentCharacter()->charobjid);
				}

			}
				break;
			case 767:
				bool isStart;
				data->Read(isStart);
				bool dat;
				for (uchar i = 0; i < 7; i++){
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
				ulong unknown;
				data->Read(unknown);
				ulong len;
				data->Read(len);
				cout << "[WRLD] Recieved Packet of Length " << len << endl;
				vector<wchar_t> msgVector;
				msgVector.reserve(len);
				for (ulong k = 0; k < len; k++){
					wchar_t mchr;
					data->Read(mchr);
					msgVector.push_back(mchr);
				}

				std::wstring message(msgVector.begin(), msgVector.end());
				cout << "[WRLD] Recieved Message: \"";
				wcout << message;
				cout << "\"" << endl;

				if (message.substr(0, 1) == L"/"){
					long commandEnd = message.find_first_of(L" ");
					wstring command;
					vector<wstring> params;
					if (commandEnd == -1){
						//The command has no spaces, it is just the command name
						command = message.substr(1, message.size() - 1);
					}
					else{
						//command name isfrom char 1 with length commandEnd - 1
						command = message.substr(1, commandEnd - 1);
						wstring param_str = message.substr(commandEnd + 1);
						long lpos = param_str.find_first_of(L" ");
						bool append = false;
						while ((param_str.size() > 0) && (lpos > -1)){
							std::wstring next = param_str.substr(0, lpos);
							if (append){
								if (endsWith(next, L"\"")){ //Ends with '"'
									params.at(params.size() - 1).append(L" ").append(removeRight(next, 1));
									append = false;
								}
								else{
									params.at(params.size() - 1).append(L" ").append(next);
								}
							}
							else{
								if (startsWith(next, L"\"")){
									bool flag = (next.size() > 1) && endsWith(next, L"\"");
									if (!flag){
										append = true;
										std::cout << "APP";
										params.push_back(param_str.substr(1, lpos - 1));
									}
									else{
										std::cout << "BLOCK";
										params.push_back(param_str.substr(1, lpos - 2));
									}
								}
								else{
									params.push_back(param_str.substr(0, lpos));
								}
							}
							param_str = param_str.substr(lpos + 1);
							lpos = param_str.find_first_of(L" ");
						}
						if (param_str.size() > 0){
							if (append){
								if (param_str.substr(param_str.size() - 1, 1).compare(L"\"") == 0){ //Ends with '"'
									params.at(params.size() - 1).append(L" ").append(param_str.substr(0, param_str.size() - 1));
								}else{
									params.at(params.size() - 1).append(L" ").append(param_str);
								}
								append = false;
							}
							else{
								if (startsWith(param_str, L"\"") && endsWith(param_str, L"\"")){
									params.push_back(remove(param_str, 1,1));
								}
								else{
									params.push_back(param_str);
								}
							}
						}
					}
					cout << "[WRLD] Command: '";
					wcout << command;
					cout << "'" << endl;

					cout << "[WRLD] Params: ";
					for (ushort k = 0; k < params.size(); k++){
						if (k>0) cout << ", ";
						cout << "'";
						wcout << params.at(k);
						cout << "'";
					}
					cout << endl;

					bool flag = false;

					// -- Client defined commands --
					if (command == L"help"){
						if (params.size() > 0){
							if (params.at(0) == L"/w"){
								RakNet::BitStream *aw = usr->sendMessage(L"/w <recipient> <message>", L"Command Usage");
								WorldServer::sendPacket(aw, systemAddress);
							}
							if (params.at(0) == L"/help"){
								RakNet::BitStream *aw = usr->sendMessage(L"/help [/<command>]", L"Command Usage");
								WorldServer::sendPacket(aw, systemAddress);
							}
						}
						else{
							std::wstring help(L"");
							help.append(L"/help			This command"); //These tabs are like that to align the text in game
							help.append(L"\n/pos		Show current position");
							help.append(L"\n/tp			Teleport to another world");

							RakNet::BitStream *aw = usr->sendMessage(help, L"Command Help");
							rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
						}
						flag = true;
					}

					if (command == L"w"){
						//Private Chat command
						if (params.size() > 1){
							std::wstring reciever = params.at(0);
							std::wstring message = params.at(1);
							for (unsigned int k = 2; k < params.size(); k++){
								message.append(L" ");
								message.append(params.at(k));
							}
							std::cout << "[WRLD] [COMMAND] Private Message to ";
							std::wcout << reciever;
							std::cout << std::endl;
							std::cout << "[WRLD] [COMMAND] ";
							std::wcout << message;
							std::cout << std::endl;
						}
						flag = true;
					}

					// -- Custom commands --

					if (command == L"tp"){
						if (params.size() > 0){
							ushort argumentValue = stoi(params.at(0));
							ZoneId zone = static_cast<ZoneId>(argumentValue);
							cout << "[WRLD] Requesting Teleport to " << zone << endl;
							bool f = false;
							if (getWorldTarget(zone).size() > 0){
								f = usr->ChangeWorld(zone, rakServer, systemAddress);
							}
							if (!f){
								std::wstringstream strs;
								strs << L"Cannot teleport to WorldID " << params.at(0);
								std::wstring rmsg = strs.str();
								std::wcout << rmsg;
								RakNet::BitStream *aw = usr->sendMessage(rmsg);
								rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
							}
						}
						else{
							RakNet::BitStream *aw = usr->sendMessage(L"Usage: /tp <ZoneId>");
							rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
						}
						flag = true;
					}

					if (command == L"tpx"){
						if (params.size() > 1){
							std::string ip = UtfConverter::ToUtf8(params.at(0));
							short port = (short) std::stoi(params.at(1));
							RakNet::BitStream * redirect = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_REDIRECT);
							PacketTools::WriteToPacket(redirect, ip, 33);
							redirect->Write(port);
							redirect->Write((unsigned char) 1);
							WorldServer::sendPacket(redirect, systemAddress);
						}
						flag = true;
					}

					if (command == L"move"){
						RakNet::BitStream gmmsg2;
						sendGameMessage(objid, &gmmsg2, 509); //Enable Movement
						rakServer->Send(&gmmsg2, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
						flag = true;
					}

					if (command == L"ot"){
						RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
						bs->Write(usr->GetCurrentCharacter()->charobjid);
						bs->Write((unsigned short)19);
						bs->Write(false);
						bs->Write(false);
						bs->Write(false);
						float x = 0.0F;
						float y = 0.0F;
						float z = 0.0F;
						if (params.size() > 2){
							x = std::stof(params.at(0));
							y = std::stof(params.at(1));
							z = std::stof(params.at(2));
						}
						bs->Write(x);
						bs->Write(y);
						bs->Write(z);
						SessionInfo s = SessionsTable::getClientSession(systemAddress);
						std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
						for (unsigned int k = 0; k < sessionsz.size(); k++){
							WorldServer::sendPacket(bs, sessionsz.at(k).addr);
						}
						
						flag = true;
					}

					if (command == L"eq"){ //Equipment
						if (params.size() > 0){
							if (params.at(0) == L"clear"){
								PlayerObject *player = usr->GetPlayer();
								if (player != NULL){
									std::vector<COMPONENT17_EQUIPMENT> *eq = player->getComponent17()->getEquipment();
									eq->clear();
									player->serialize();
								}
							}
						}
						if (params.size() > 1){
							if (params.at(0) == L"set"){
								PlayerObject *player = usr->GetPlayer();
								if (player != NULL){
									std::vector<COMPONENT17_EQUIPMENT> *eq = player->getComponent17()->getEquipment();
									COMPONENT17_EQUIPMENT eqi;
									eqi.objid = 1152921510794154770 + counter;
									counter++;
									eqi.lot = stol(params.at(1));
									if (params.size() > 2){
										eqi.slot = stoi(params.at(2));
									}

									if (eq->size() > 0){
										eq->at(0) = eqi;
									}
									else{
										eq->push_back(eqi);
									}
									player->serialize();
								}
							}
						}
						flag = true;
					}

					if (command == L"item"){
						if (params.size() > 1){
							if (params.at(0) == L"create"){
								long lot = stol(params.at(1));
								long long objid = ObjectsTable::createObject(lot);
								std::wstringstream wstr;
								wstr << L"Object created with id: " << objid - 1152921510794154770;
								RakNet::BitStream *aw = usr->sendMessage(wstr.str());
								rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
							}
							
							if (params.at(0) == L"equip"){
								long long objid = 1152921510794154770 + stoll(params.at(1));	
								PlayerObject *player = usr->GetPlayer();
								if (player != NULL){
									if (params.size() > 2){
										ushort slot = stoi(params.at(2));
										player->getComponent17()->equipItem(objid, slot);
									}
									else{
										player->getComponent17()->equipItem(objid);
									}
									player->serialize();
								}
							}
							if (params.at(0) == L"unequip"){
								long long objid = 1152921510794154770 + stoll(params.at(1));
								PlayerObject *player = usr->GetPlayer();
								if (player != NULL){
									bool un = player->getComponent17()->unequipItem(objid);
									if (!un){
										std::wstringstream wstr;
										wstr << L"Object not found";
										RakNet::BitStream *aw = usr->sendMessage(wstr.str());
										rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
									}
									else{
										player->serialize();
									}
								}
							}
						}
						flag = true;
					}

					if (command == L"setitem"){
						if (params.size() == 3){
							long id = stoi(params.at(0));
							unsigned long amount = stoi(params.at(1));
							unsigned long slot = stoi(params.at(2)) - 1;

							long long objid = ObjectsTable::createObject(id);
							InventoryTable::insertItem(usr->GetCurrentCharacter()->charobjid, objid, amount, slot, false);

							Chat::sendChatMessage(systemAddress, L"Successfully added the requested item to your inventory! Please travel to another world or relog to reload your inventory.");
						}
						else{
							Chat::sendChatMessage(systemAddress, L"Usage: /setitem <ItemID> <Amount> <Slot>");
						}
						flag = true;
					}

					if (command == L"destruct"){
						PlayerObject *player = usr->GetPlayer();
						if (player != NULL){
							player->doDestruction(systemAddress, usr->getWorld(), false);
							
						}
						flag = true;
					}

					if (command == L"create"){
						PlayerObject *player = usr->GetPlayer();
						if (player != NULL){
							player->doCreation(systemAddress, usr->getWorld(), false);
						}
						flag = true;
					}

					if (command == L"pos" || command == L"position"){
						bool flag2 = true;
						if (params.size() >= 4){
							if (params.at(0) == L"set"){
								float newx = stof(params.at(1));
								float newy = stof(params.at(2));
								float newz = stof(params.at(3));
								
								PlayerObject *player = usr->GetPlayer();
								if (player != NULL){
									player->getComponent1()->setPosition(COMPONENT1_POSITION(newx, newy, newz));
									player->doDestruction(systemAddress, usr->getWorld(), false);
								}

								std::wstringstream wstr;
								wstr << L"Set Position: (" << newx << "|" << newy << "|" << newz << ")";
								RakNet::BitStream *aw = usr->sendMessage(wstr.str());
								rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
								std::wstringstream wstr2;
								wstr2 << L"Your character has been disassembled. To reconstruct him at your target location, type '/create'";
								RakNet::BitStream *aw2 = usr->sendMessage(wstr2.str(), L"Toni Teleport");
								rakServer->Send(aw2, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);

								flag = true;
								flag2 = false;
							}
						}
						if (flag2){
							
							std::wstringstream wstr;
							PlayerObject *player = usr->GetPlayer();
							if (player != NULL){
								COMPONENT1_POSITION pos = player->getComponent1()->getPosition();
								wstr << L"Position: (" << pos.x << "|" << pos.y << "|" << pos.z << ")";
							}
							RakNet::BitStream *aw = usr->sendMessage(wstr.str());
							rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
							flag = true;
						}
					}

					if (command == L"friends"){
						if (params.size() > 0){
							if (params.at(0) == L"requests"){
								Friends::checkFriendRequests(usr->GetCurrentCharacter()->charobjid);
							}
						}
						if (params.size() > 1){
							if (params.at(0) == L"status"){
								std::string target = UtfConverter::ToUtf8(params.at(1));
								Friends::sendFriendRequestResponse(usr->GetCurrentCharacter()->charobjid, target);
								RakNet::BitStream *aw = usr->sendMessage(L"Sending Friend Request Response");
								WorldServer::sendPacket(aw, systemAddress);
							}
						}
						else{

						}
						flag = true;
					}

					if (command == L"replica"){
						if (params.size() > 0){
							//Packet 1: 2365 (ZoneControl Object)
							//Packet 2: objid: 288300744895889662 LOT: 6635 SpawnerId: 72567767517768 (Mardolf the Orange)
							//Packet 3: objid: 288300744895889664 LOT: 6636 SpawnerId: 72567767517769 (Rad Eccles)
							//Packet 4: objid: 288300744895889669 LOT: 3495 SpawnerId: 72567767517772
							//Packet 5: objid: 288300744895899827 LOT: 6726 SpawnerId: 72567767517771
							//Packet 6: objid: 288300744895909034 LOT: 5637 SpawnerId: 3695535 NodeID: 1, Starts with Index 20 (position), x:-309.645782, y:288.356626, z: 70.64473 (Robot Dog Pet)
							//Packet 7: objid: 288300744895889606 LOT: 3646 SpawnerId: 3695538 NodeId: 3, 
							//Packet 8: objid: 288300744895930870 LOT: 9717 SpawnerId: 72567767517763, 
							//Packet 9: PLAYER

							

							int num = stoi(params.at(0));
							stringstream fl;
							fl << ".\\worldTest\\NS\\replica\\ns_replica" << num << ".bin";
							auto v = OpenPacket(fl.str());
							ServerSendPacket(rakServer, v, systemAddress);
							std::wstringstream msg;
							msg << "Created Replica Packet " << num;
							RakNet::BitStream *aw = usr->sendMessage(msg.str());
							rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
						}
						flag = true;
					}

					if (command == L"replica2"){
						if (params.size() > 0){
							//Packet 62s: objid: 288300744895892667 LOT: 6368, Spawner: 70368747895233

							int num = stoi(params.at(0));
							stringstream fl;
							std::wstringstream msg;
							if (params.size() > 1){
								int num2 = stoi(params.at(1));
								fl << ".\\worldTest\\replica\\" << num << "s_" << num2 << ".bin";
								msg << "Created Replica Packet " << num << "_" << num2;
							}
							else{
								fl << ".\\worldTest\\replica\\" << num << "s.bin";
								msg << "Created Replica Packet " << num;
							}
							auto v = OpenPacket(fl.str());
							ServerSendPacket(rakServer, v, systemAddress);
							RakNet::BitStream *aw = usr->sendMessage(msg.str());
							rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
						}
						flag = true;
					}

					if (command == L"replica3"){
						//1: objid 1152921510473527054 PLAYER?


						if (params.size() > 0){
							int num = stoi(params.at(0));
							stringstream fl;
							fl << ".\\worldTest\\replica\\replica" << num << ".bin";
							auto v = OpenPacket(fl.str());
							ServerSendPacket(rakServer, v, systemAddress);
							std::wstringstream msg;
							msg << "Created Replica Packet " << num;
							RakNet::BitStream *aw = usr->sendMessage(msg.str());
							rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
						}
						flag = true;
					}

					if (command == L"health"){
						if (params.size() > 0){
							ulong num = stol(params.at(0));
							PlayerObject *player = usr->GetPlayer();
							if (player != NULL){
								COMPONENT7_DATA4 d4 = player->getComponent7()->getData4();
								d4.health = num;
								if (params.size() > 1){
									float max = stof(params.at(1));
									d4.maxHealth = max;
									if (params.size() > 2){
										float max2 = stof(params.at(1));
										d4.maxHealthN = max2;
									}
								}
								player->getComponent7()->setData4(d4);
								player->serialize();
							}
						}
						flag = true;
					}

					if (command == L"armor"){
						if (params.size() > 0){
							ulong num = stol(params.at(0));
							PlayerObject *player = usr->GetPlayer();
							if (player != NULL){
								COMPONENT7_DATA4 d4 = player->getComponent7()->getData4();
								d4.armor = num;
								if (params.size() > 1){
									float max = stof(params.at(1));
									d4.maxArmor = max;
									if (params.size() > 2){
										float max2 = stof(params.at(1));
										d4.maxArmorN = max2;
									}
								}
								player->getComponent7()->setData4(d4);
								player->serialize();
							}
						}
						flag = true;
					}

					if (command == L"imagi"){
						if (params.size() > 0){
							ulong num = stol(params.at(0));
							PlayerObject *player = usr->GetPlayer();
							if (player != NULL){
								COMPONENT7_DATA4 d4 = player->getComponent7()->getData4();
								d4.imagination = num;
								if (params.size() > 1){
									float max = stof(params.at(1));
									d4.maxImagination = max;
									if (params.size() > 2){
										float max2 = stof(params.at(1));
										d4.maxImaginationN = max2;
									}
								}
								player->getComponent7()->setData4(d4);
								player->serialize();
							}
						}
						flag = true;
					}

					if (command == L"d747"){
						if (params.size() > 0){
							ulong num = stol(params.at(0));
							PlayerObject *player = usr->GetPlayer();
							if (player != NULL){
								COMPONENT7_DATA4 d4 = player->getComponent7()->getData4();
								player->getComponent7()->setData4(d4);
								d4.d7 = num;
								player->serialize();
							}
						}
						flag = true;
					}

					if (command == L"packet"){
						std::wstring msg = L"Usage: /packet <path>";
						if (params.size() > 0){
							vector<uchar> v = OpenPacket(UtfConverter::ToUtf8(params.at(0)));
							if (v.size() > 0){
								ServerSendPacket(rakServer, v, systemAddress);
								msg = L"Success sending packet";
							}else{
								msg = L"Error sending packet";
							}
						}
						Chat::sendChatMessage(usr->GetCurrentCharacter()->charobjid, msg, L"System");
						flag = true;
					}

					if (command == L"popup"){
						if (params.size() > 1){
							long long charid;
							std::wstring rec = params.at(0);
							bool flagOne = false;
							if (rec == L"#"){
								charid = usr->GetCurrentCharacter()->charobjid;
								flagOne = true;
							}
							else if (rec == L"*"){
								std::string msg = UtfConverter::ToUtf8(params.at(1));
								std::string title = "Information";
								if (params.size() > 2){
									title = UtfConverter::ToUtf8(params.at(2));
								}
								SessionInfo s = SessionsTable::getClientSession(systemAddress);
								std::vector<SessionInfo> wsessions = SessionsTable::getClientsInWorld(s.zone);
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
									std::string msg = UtfConverter::ToUtf8(params.at(1));
									std::string title = "Information";
									if (params.size() > 2){
										title = UtfConverter::ToUtf8(params.at(2));
									}
									Chat::sendMythranInfo(charid, msg, title);
								}
								else{
									std::wstring response = L"\"" + rec + L"\" is not a valid Playername";
									Chat::sendChatMessage(usr->GetCurrentCharacter()->charobjid, response);
								}
							}
						}
						else{
							Chat::sendChatMessage(usr->GetCurrentCharacter()->charobjid, L"Usage: /popup <playername|*> <message> [title]");
						}
						flag = true;
					}

					if (!flag){
						cout << "[WRLD] Command not found" << endl;
						std::wstring notFoundMessage = L"Command '";
						notFoundMessage.append(command);
						notFoundMessage.append(L"' does not exist");
						RakNet::BitStream *aw = usr->sendMessage(notFoundMessage);
						rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
					}
				}
			}
				break;
			case 888:
			{
				long long object;
				data->Read(object);
				//cout << "Object: " << object << std::endl;
				ObjectInformation o = getObjectInformation(object);
				cout << getObjectDescription(o, object) << endl;
				//Some sort of loading, L8: objid
			}
				break;
			case 1145:
			{
				bool flag;
				data->Read(flag);
				unsigned long d1;
				data->Read(d1);
				std::cout << "[WRLD] [COMBAT] " << std::to_string(flag) << "|" << std::to_string(d1) << std::endl;
				PacketTools::printBytes(data, d1);
				unsigned long num1;
				data->Read(num1);
				unsigned long num2;
				data->Read(num2);
				std::cout << "[WRLD] [COMBAT] " << std::to_string(num1) << "|" << std::to_string(num2) << std::endl;
			}
				break;
			case 1202:
			{
				std::cout << "[WRLD] [HELP] Smash me" << std::endl;
			}
				break;
			case 1308:
			{
				//This handles the requirements of a minigame for the server side.
				//Starting with 8 bytes of objectId?, then 4 bytes for the data length
				//Then that with a null byte terminator?
				//Then two 4 byte values
				ulonglong object;
				data->Read(object);

				cout << "Minigame cost: objid:" << object << endl;

				ulong len;
				data->Read(len);

				if (len > 0){
					vector<wchar_t> mV;
					mV.reserve(len);
					for (ulong k = 0; k < len; k++){
						wchar_t mC;
						data->Read(mC);
						mV.push_back(mC);
					}
					std::wstring script(mV.begin(), mV.end());
					//I guess this is a null terminator as it only appears with text content, but not included in length
					ushort nullT;
					data->Read(nullT);
					cout << "Data: '";
					wcout << script;
					cout << "'" << endl;
				}
				//These 8 bytes should be two values, since the only thing I found so far has content only in the 5th
				ulong dataA;
				ulong dataB;
				data->Read(dataA);
				data->Read(dataB);
				cout << "A: " << dataA << ", B: " << dataB << endl;
			}
				break;
			case 1419:
			{
				ulonglong object;
				data->Read(object);
				cout << "Something: objid: " << object << endl;
				break;
			}
			case 1734:
				//Level UP

				break;
			default:
				cout << "Unknown Game Message [" << msgid << "]" << endl;
				break;
			}
			printData = true;
			break;
		}
		case CLIENT_GENERAL_CHAT_MESSAGE:
		{
			data->SetReadOffset(data->GetReadOffset() + 24); //3 bytes channel?
			ulong length;
			data->Read(length);
			vector<wchar_t> messageVector;
			messageVector.reserve(length);
			for (ulong k = 0; k < length; k++){
				wchar_t mc;
				data->Read(mc);
				if (mc == 0) break; else messageVector.push_back(mc);
			}

			std::wstring message(messageVector.begin(), messageVector.end());

			wstring name;
			Ref<Character> character = usr->GetCurrentCharacter();
			if (character != NULL){
				//This sudddenly seems to work without me having changed anything
				name = StringToWString(character->GetName(), 33);
			}
			else{
				name = StringToWString(usr->GetUsername(), 33);
			}
			cout << "[WRLD] [CHAT] ";
			wcout << name;
			cout << ": '";
			wcout << message;
			cout << "'" << endl;
			Chat::broadcastChatMessage(usr->getWorld(), message, name);
		}
			break;
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

				std::cout << "[WRLD] [MAIL] Request Sending Mail: " << std::endl;
				std::cout << "TO:           ";
				std::wcout << recipient;
				std::cout << std::endl;
				std::cout << "SUBJECT:      ";
				std::wcout << subject;
				std::cout << std::endl;
				std::cout << "TEXT:         ";
				std::wcout << text;
				std::cout << std::endl;
				std::cout << "LANGUAGE:     " << std::to_string(lang) << std::endl;
				std::cout << "ATTACHMENT:   " << std::to_string(attach) << "[" << std::to_string(attcount) << "]" << std::endl;
				std::cout << "D1/D3:        " << std::to_string(d1) << "/" << std::to_string(d3) << std::endl;

				long long charid = usr->GetCurrentCharacter()->charobjid;
				Mail::requestMailSending(charid, subject, text, recipient, attach, attcount);
			}
				break;
			case 3:
				Mail::loadMailboxData(usr->GetCurrentCharacter()->charobjid);
				break;
			case 5:
			{
				ulong ua;
				data->Read(ua);
				
				long long mid;
				data->Read(mid);
				long long objid;
				data->Read(objid);
				Logger::log("WRLD", "MAIL", "Requesting remove attachment for mail id " + std::to_string(mid));
				Logger::log("WRLD", "MAIL", std::to_string(ua));
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
				std::cout << "[WRLD] [MAIL] Mail Read: " << std::to_string(mid) << std::endl;
			}
				break;
			default:
				std::cout << "[WRLD] [MAIL] Mail ID: " << std::to_string(mailid) << std::endl;
				printData = true;
			}
		}
			break;
		
		case WorldPacketID::CLIENT_LEVEL_LOAD_COMPLETE:
			ZoneId z;
			data->Read(z);
			Session::enter(usr->GetCurrentCharacter()->charobjid, z);
			Logger::log("WRLD", "PARSER", "Client: Level loading complete " + z);
			break;
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
			std::cout << "[WRLD] [UGC] Download failed (" << std::to_string(d1) << "|" << std::to_string(d2) << ")" << std::endl;
			std::cout << "[WRLD] [UGC] Object: " << std::to_string(objid) << std::endl;
			std::cout << "[WRLD] [UGC] Player: " << std::to_string(charid) << std::endl;
		}
			break;
		default:
			std::cout << "[WRLD] Recieved unknown SERVER packet (" << packetId << ")" << endl;
			printData = true;
			break;
		}
		break;
	case RemoteConnection::CLIENT:
		//Message to client
		//This connection should never occur as it is a message to the client which this program never functions as
		cout << "[WRLD] Recieved unknown CLIENT packet (" << packetId << ")" << endl;
		printData = true;
		break;
	}

	if (printData){
		if (data->GetNumberOfUnreadBits() > 0){
			PacketTools::printRest(data);
			//cout << RawDataToString(data->GetData(), data->GetNumberOfBytesUsed(), false, (data->GetReadOffset() >> 3)) << endl;
		}else{
			cout << "No more data" << endl;
		}
		
	}
}

bool handleObject(ObjectInformation obj, RakPeerInterface* rakServer, SystemAddress &systemAddress, Ref<User> usr){
	if (obj.type != ObjectType::NONE){
		switch (obj.type)
		{
		case ObjectType::LaunchPad:
		{
			ZoneId zone = getLaunchPadTarget(obj);
			if (zone != ZoneId::NO_ZONE){
				usr->ChangeWorld(zone, rakServer, systemAddress);
			}
		}
			break;
		case ObjectType::PostBox:
		{
			//Chat::sendMythranInfo(usr->GetCurrentCharacter()->charobjid, "We are sorry to inform you the Mail system isn't working yet!", "Our deepest apologies");
			Mail::openMailbox(usr->GetCurrentCharacter()->charobjid);
		}
			break;
		default:
			return false;
			break;
		}
	}
	return true;
}