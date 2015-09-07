#include "serverLoop.h"

#include "Database.h"
#include "legoPackets.h"
#include "AuthPackets.h"

#include "Account.h"
#include "ServerDB.h"

#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"

#include "Logger.h"
#include "UtfConverter.h"
#include "md5.h"

using namespace RakNet;

/*
	Update 24.08.2015 Xiphoseer
	This function now contains all logic to
		- Read the 53-01-00-00 LOGIN_REQUEST packet
		- Verify the login credentials using the database
		- Sending the response to the client
		- Register the client in SessionsTable and UsersPool
*/
void HandleUserLogin(RakPeerInterface* rakServer, Packet* packet, CONNECT_INFO* cfg) {
	//Make the packet data accessible via a bit stream
	RakNet::BitStream *data = new RakNet::BitStream(packet->data, packet->length, false);
	unsigned char packetID;
	data->Read(packetID);
	unsigned short networkType;
	data->Read(networkType);
	unsigned long packetType;
	data->Read(packetType);
	unsigned char pad;
	data->Read(pad);
	
	//Check if the correct packet is passed to this function
	if (networkType == RemoteConnection::AUTH && packetType == AuthPacketID::LOGIN_REQUEST) {
		// Read all available data from the login packet
		// and log it, TODO: transform that into debug log messages
		std::wstring username = PacketTools::ReadFromPacket(data, 33);
		std::wstring password = PacketTools::ReadFromPacket(data, 41);
		Logger::log("AUTH", "LOGIN", "Username: " + UtfConverter::ToUtf8(username), LOG_DEBUG);
		std::vector<char> pwcv(password.size(), '*');
		std::string pwcs(pwcv.begin(), pwcv.end());
		Logger::log("AUTH", "LOGIN", "Password: " + pwcs, LOG_DEBUG);
		unsigned short language_id;
		data->Read(language_id);
		Logger::log("AUTH", "LOGIN", "Language: " + std::to_string(language_id), LOG_ALL);
		unsigned char uk1;
		data->Read(uk1);
		Logger::log("AUTH", "LOGIN", "Unknown?: " + std::to_string(uk1), LOG_ALL);
		std::wstring process_info = PacketTools::ReadFromPacket(data, 256);
		std::wstring graphics_info = PacketTools::ReadFromPacket(data, 128);
		Logger::log("AUTH", "LOGIN", UtfConverter::ToUtf8(process_info), LOG_ALL);
		Logger::log("AUTH", "LOGIN", UtfConverter::ToUtf8(graphics_info), LOG_ALL);
		unsigned long num_processors, type_processor;
		data->Read(num_processors);
		data->Read(type_processor);
		unsigned short level_processor, revision_processor;
		data->Read(level_processor);
		data->Read(revision_processor);
		Logger::log("AUTH", "LOGIN", "Processor: " + 
			std::to_string(num_processors) + ", " + 
			std::to_string(type_processor) + ", " + 
			std::to_string(level_processor) + ", " + 
			std::to_string(revision_processor),
			LOG_ALL
			);
		unsigned long uk2;
		data->Read(uk2);
		Logger::log("AUTH", "LOGIN", "Unknown?: " + std::to_string(uk2), LOG_ALL);
		if (data->GetNumberOfUnreadBits() > 0){
			unsigned long os_major_version, os_minor_version, os_build_number, os_platform_id;
			data->Read(os_major_version);
			data->Read(os_minor_version);
			data->Read(os_build_number);
			data->Read(os_platform_id);
			Logger::log("AUTH", "LOGIN", "OS: " +
				std::to_string(os_major_version) + '.' +
				std::to_string(os_minor_version) + '.' +
				std::to_string(os_build_number), LOG_ALL);
			Logger::log("AUTH", "LOGIN", "OSP: " + std::to_string(os_platform_id), LOG_ALL);
		}
		
		std::string usernameA, passwordA;
		usernameA = UtfConverter::ToUtf8(username);
		passwordA = UtfConverter::ToUtf8(password);

		//Validating the input
		//Set default values
		UserSuccess currentLoginStatus = UserSuccess::SUCCESS;
		//Ref<User> user = NULL;
		
		//query the account id of the associated with the username from the database
		unsigned int accountid = AccountsTable::getAccountID(usernameA);
		if (accountid == 0){
			//If the query return 0, no user was found
			currentLoginStatus = UserSuccess::INVALID_USER;
		}else{
			//check if the password is correct
			bool passwordCorrect = AccountsTable::checkPassword(passwordA, accountid);
			AccountAccessInfo info = AccountsTable::getAccessInfo(accountid);

			if (info.locked || info.banned){
				if (info.banned){
					Logger::log("USER", "LOGIN", "User is BANNED");
					currentLoginStatus = UserSuccess::BANNED;
				}
				else{
					Logger::log("USER", "LOGIN", "User is LOCKED");
					currentLoginStatus = UserSuccess::LOCKED;
				}
			}
			else{
				if (passwordCorrect){
					//password correct
					if (info.loginTries > 0){
						info.loginTries = 0;
					}
				}
				else{
					//password incorrect
					info.loginTries++;
					if (info.loginTries > 2){
						info.locked = true;
						info.loginTries = 3;
						Logger::log("USER", "LOGIN", "User is LOCKED");
						currentLoginStatus = UserSuccess::LOCKED;
					}else{
						Logger::log("USER", "LOGIN", "User has INCORRECT_PASSWORD");
						currentLoginStatus = UserSuccess::INVALID_PASS;
					}					
				}
				AccountsTable::setAccessInfo(accountid, info);
			}
		}

		//respond to the client

		LoginStatusPacket loginStatusPacket;

		// Set the loginStatus
		loginStatusPacket.loginStatus = currentLoginStatus;

		// Set Talk_Like_A_Pirate_String
		loginStatusPacket.talkLikeAPirate = "Talk_Like_A_Pirate";
		loginStatusPacket.unknownString = "";

		// Set client version
		loginStatusPacket.clientVersion1 = 1;
		loginStatusPacket.clientVersion2 = 10;
		loginStatusPacket.clientVersion3 = 64;

		// This is unknown data...
		loginStatusPacket.unknown = "_";

		time_t t = time(NULL);
		unsigned int addr = packet->systemAddress.binaryAddress;
		long long a = (long long)t * (long long)addr;
		std::string keyhash = md5(std::to_string(a));
		std::wstring key = StringToWString(keyhash, 33);

		// Get the user key
		loginStatusPacket.userKey = key;
		//loginStatusPacket.userKey = "0 9 4 e 7 0 1 a c 3 b 5 5 2 0 b 4 7 8 9 5 b 3 1 8 5 7 b f 1 c 3   ";

		// Set chat IPs/Port and the other IP
		loginStatusPacket.chatIp = "192.168.0.20"; //TODO: make dynamic
		loginStatusPacket.chatPort = 2003;
		loginStatusPacket.anotherIp = "192.168.0.20";

		loginStatusPacket.possibleGuid = "00000000-0000-0000-0000-000000000000";

		loginStatusPacket.zeroLong = 0;

		// Set localization
		loginStatusPacket.localizationChar[0] = 0x55;
		loginStatusPacket.localizationChar[1] = 0x53;
		loginStatusPacket.localizationChar[2] = 0x00;

		// Subscribed?
		loginStatusPacket.firstLoginSubscription = 1;
		loginStatusPacket.subscribed = 0;

		loginStatusPacket.zeroLongLong = 0;

		loginStatusPacket.redirectIp = cfg->redirectIp;
		loginStatusPacket.redirectPort = cfg->redirectPort;

		// Set the error msg and the error msg length
		// This message only shows
		loginStatusPacket.errorMsg = "";
		loginStatusPacket.errorMsgLength = loginStatusPacket.errorMsg.length();

		std::string world_server_address;
		
		SystemAddress serverAddr;
		serverAddr.SetBinaryAddress(cfg->redirectIp);
		serverAddr.port = cfg->redirectPort;

		int instanceid = InstancesTable::getInstanceId(serverAddr);
		if (instanceid == -1){
			loginStatusPacket.loginStatus = UserSuccess::UNKNOWN2;
			loginStatusPacket.errorMsg = "Universe not available";
			currentLoginStatus = UserSuccess::UNKNOWN2;
			Logger::log("AUTH", "LOGIN", "INSTANCE UNAVAILABLE", LOG_ERROR);
		}
		
		if (currentLoginStatus == UserSuccess::SUCCESS){
			// Login the user to the server
			Session::login(packet->systemAddress, accountid, keyhash, instanceid);
			Logger::log("AUTH", "LOGIN", usernameA + " Logged-in");
			SendStatusPacket(rakServer, packet->systemAddress, loginStatusPacket);
			return;
		}
		SendStatusPacket(rakServer, packet->systemAddress, loginStatusPacket);
	}
	Logger::log("AUTH", "LOGIN", "Login failed", LOG_WARNING);
}

void AuthLoop(CONNECT_INFO* cfg) {
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
	// Otherwise, print an error
	if (rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		Logger::log("AUTH", "", "started! Listening on port " + std::to_string(cfg->listenPort));
	} else QuitError("[AUTH] server init error!");

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is initalized, use it to log the server in ./logs/auth
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\auth");

	// Initialize the Packet class
	Packet* packet;

	//LUNI_AUTH = true;

	// LUNIterminate is the bool used to terminate threads.
	// While it is false, the thread runs, but if it is true, the thread exits
	while (!getTerminate()) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive(); // Get the packets from the client
		if (packet == NULL) continue; // If the packet is null, just continue without processing anything
		PrintPacketInfo(packet, msgFileHandler); // Print the packet information (if packet is not NULL)

		RakNet::BitStream *data = new RakNet::BitStream(packet->data, packet->length, false);
		unsigned char packetID;
		data->Read(packetID);
		// Figure out which packet it is...
		switch (packetID) {
			case ID_LEGO_PACKET:

				unsigned short networkType;
				data->Read(networkType);
				unsigned long packetType;
				data->Read(packetType);
				unsigned char pad;
				data->Read(pad);

				switch (networkType) {
					case GENERAL:
						if (packetType == 0) {
							// Send the Init packet to the client
							//SendInitPacket(rakServer, packet->systemAddress, true);
							DoHandshake(rakServer, packet->systemAddress, data, "AUTH");
						}
						break;

					case AUTH: //user logging into server
						{
							// Handle the user login using the above method
							HandleUserLogin(rakServer, packet, cfg);
						}
						break;

					// The default if the packet the server is recieving has an unidentified ID
					default:
						Logger::log("AUTH", "", "received unknown packet: " + RawDataToString(packet->data, packet->length));
				}

				break;

			// If the server is recieving a new connection, print it.
			case ID_NEW_INCOMING_CONNECTION:
#			ifdef DEBUG
				Logger::log("AUTH", "" , "is receiving a new connection...");
			#endif
				break;

			// If someone is disconnecting from the auth server, print it
			case ID_DISCONNECTION_NOTIFICATION:
				Logger::log("AUTH", "", "User disconnected from Auth server...");
				Session::disconnect(packet->systemAddress, SessionPhase::PHASE_CONNECTED);
				break;
				
			// If the packet has an unidentified RakNet ID (one not listed here), print the
			// packet info
			default:
				Logger::log("AUTH", "", "received unknown packet: " + RawDataToString(packet->data, packet->length));
		}

		// Deallocate the packet to conserve memory
		rakServer->DeallocatePacket(packet);
	}

	// If LUNIServer Auth has been terminated, print it
	Logger::log("AUTH", "", "quitting");

	// Shut down Auth server
	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);

	//LUNI_AUTH = false;
}
