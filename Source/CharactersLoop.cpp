#include "serverLoop.h"

#include "legoPackets.h"
#include "User.h"
#include "CharPackets.h"
#include "Account.h"
#include "Characters.h"

#include "Logger.h"
#include "UtfConverter.h"
#include "WorldServer.h"

#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"

using namespace std;

void CharactersLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< string > > OutputQueue) {
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

	// If the startup of the server is successful, print it to the console
	// Otherwise, quit the server (as the char server is REQUIRED for the
	// server to function properly)
	if (rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		Logger::log("CHAR", "", "started! Listening on port " + std::to_string(cfg->listenPort));
	} else exit(2);

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is not NULL, save logs of char server
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\char");

	// Initialize the Packet class for the packets
	Packet* packet;

	// This will be used in the saving of packets below...
	int i = 0;

	//LUNI_CHAR = true;

	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive(); // Recieve the packets from the server
		if (packet == NULL) continue; // If packet is NULL, just continue without processing anything
		PrintPacketInfo(packet, msgFileHandler); // Save packet info
		
		// This will save all packets recieved from the client if running from DEBUG
		#ifdef DEBUG
			stringstream packetName;
			packetName << ".//Saves//Packet" << i << ".bin";

			SavePacket(packetName.str(), (char *)packet->data, packet->length);
			i++;
		#endif

		RakNet::BitStream *data = new RakNet::BitStream(packet->data, packet->length, false);
		uchar packetID;
		data->Read(packetID);

		switch (packetID) {
			case ID_LEGO_PACKET:
				ushort networkType;
				data->Read(networkType);
				ulong packetType;
				data->Read(packetType);
				uchar pad;
				data->Read(pad);
				switch (networkType) {
					case GENERAL:
						if (packetType == VERSION_CONFIRM) {
							DoHandshake(rakServer, packet->systemAddress, data, "CHAR");
							//SendInitPacket(rakServer, packet->systemAddress, false);
						}
						break;
					case SERVER:
						switch (packetType) {
							case CLIENT_VALIDATION:
							{
								std::wstring name = PacketTools::ReadFromPacket(data, 33);
								std::wstring key = PacketTools::ReadFromPacket(data, 33);
								std::string u_hash = PacketTools::ReadStrFromPacket(data, 32);
								unsigned char flag;
								data->Read(flag);
								Logger::log("CHAR", "VALID", "Client Validation Request", LOG_DEBUG);
								Logger::log("CHAR", "VALID", "Name : " + UtfConverter::ToUtf8(name), LOG_DEBUG);
								Logger::log("CHAR", "VALID", "Key  : " + UtfConverter::ToUtf8(key), LOG_ALL);
								Logger::log("CHAR", "VALID", "Hash?: " + u_hash, LOG_ALL);
								if (flag == 0) Logger::log("CHAR", "VALID", "Flag false", LOG_ALL);
								else Logger::log("CHAR", "VALID", "Flag true", LOG_ALL);

								SessionInfo s = SessionsTable::getClientSession(packet->systemAddress);
								if (s.sessionkey == UtfConverter::ToUtf8(key)){
									Logger::log("CHAR", "VALID", "Validation successful", LOG_DEBUG); //Session keys match
								}
								else{
									Logger::log("CHAR", "VALID", "Validation unsuccessful", LOG_DEBUG); //Session keys do not match
								}
								break;
							}

							case CLIENT_CHARACTER_LIST_REQUEST:
							{
								auto usr = OnlineUsers->Find(packet->systemAddress);
								SystemAddress addr = packet->systemAddress;

								Logger::log("CHAR", "", std::string(addr.ToString()));
								if (usr->nameInUse == 0) {
									Logger::log("CHAR", "", "Sending char packet...");
									SendCharPacket(rakServer, packet->systemAddress, usr->GetID());
								}
								break;
							}

							case CLIENT_CHARACTER_CREATE_REQUEST:
							{
								// Find online user by systemAddress
								auto usr = OnlineUsers->Find(packet->systemAddress);
								SessionInfo s = SessionsTable::getClientSession(packet->systemAddress);

								bool success = false;
								
								// Make SURE user is not null!!!!!!
								if (usr != NULL) {
									success = Characters::CreateCharacter(data, packet->systemAddress, s.accountid);
								}else {
									Logger::logError("CHAR", "", "saving user", "user is NULL");
								}
								
								// If the username is in use, do NOT send the char packet. Otherwise, send it
								if (success) {
									SendCharPacket(rakServer, packet->systemAddress, s.accountid);
								}
							}
								break;

							case CLIENT_CHARACTER_DELETE_REQUEST:
							{
								SessionInfo s = SessionsTable::getClientSession(packet->systemAddress);

								long long charid;
								data->Read(charid);

								Characters::DeleteCharacter(s.accountid, charid);

								RakNet::BitStream bitStream;
								CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 12, &bitStream);
								bitStream.Write((uchar)1); // Success?
								rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

								break;
							}

							case CLIENT_LOGIN_REQUEST:
							{
								// Find online user using systemAddress
								auto usr = OnlineUsers->Find(packet->systemAddress);

								// If user exists, continue
								if (usr != NULL) {
									usr->numredir++; // Add to the number of redirects that the character has
									// Get Character ID
									long long charobjid;
									data->Read(charobjid);
									
									//vector< uchar > t;
									//for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
									usr->SetCharacter(charobjid);
									Session::play(usr->GetID(), charobjid);

									// If DEBUG is on, and user had ID of one of these (It probably won't)
									// Print who the char is logging in with
								#ifdef DEBUG
									/* stringstream s;
									s << "\n[CHAR] Character logging in world with id: " << usr->nextcid;
									if ( usr->nextcid == 2397732190 ) s << " CheekyMonkey!\n";
									else if ( usr->nextcid == 2444680020 ) s << " monkeybrown!\n";
									else if ( usr->nextcid == 1534792735 ) s << " GruntMonkey!\n";
									else if ( usr->nextcid == 1457240027 ) s << " Shafantastic!\n";
									else s << "Unknown Character ID: " << usr->nextcid << endl;
									OutputQueue->Insert(s.str()); */
								#endif
								}

								std::vector<char> str;
								for (int k = 0; k < 16; k++){
									std::cout << "[" << std::to_string(k) << "] " << std::to_string(cfg->redirectIp[k]) << std::endl;
									if (cfg->redirectIp[k] > 0){
										str.push_back(cfg->redirectIp[k]);
										
									}
									else{
										break;
									}
								}
								std::string ip(str.begin(), str.end());
								short port = cfg->redirectPort;
								Logger::log("CHAR", "REDR", ip);
								RakNet::BitStream * redirect = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_REDIRECT);
								PacketTools::WriteToPacket(redirect, ip, 33);
								redirect->Write(port);
								redirect->Write((unsigned char)0);
								//WorldServer::sendPacket(redirect, packet->systemAddress);
								rakServer->Send(redirect, PacketPriority::SYSTEM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, packet->systemAddress, false);

								// Close connections to the char server for now
								//rakServer->CloseConnection(packet->systemAddress, true);
							}
								break;
						}

						break;

					// If packet ID (3rd byte) is unidentified, print the packet data
					default:
						Logger::log("CHAR", "", "received unknown packet: " + RawDataToString(packet->data, packet->length));
				}

				break;

			// Recieving a new connection to the char server
			case ID_NEW_INCOMING_CONNECTION:
			#ifdef DEBUG
				Logger::log("CHAR", "", "is receiving a new connection...");
			#endif
				break;

			// User disconnected from the char server
			case ID_DISCONNECTION_NOTIFICATION:
			{
				auto usr = OnlineUsers->Find(packet->systemAddress);
				if (OnlineUsers->Remove(packet->systemAddress))
					Logger::log("CHAR", "", "disconnected " + usr->GetUsername());
				Session::disconnect(packet->systemAddress, SessionPhase::PHASE_AUTHENTIFIED);
			}
				break;

			// Default msg (if packet is unidentified)
			default:
				Logger::log("CHAR", "", "received unknown packet: " + RawDataToString(packet->data, packet->length));
		}
	}
	//LUNI_CHAR = false;
}
