/*
	Lego Universe Server WIP
	Code done by humanoid24 (aka Triver) as of 01.12.2013

	Additional credits to raffa505 30.01.2013 - 14.02.2013 (Database MySQL, Character, User, World, Color, Threads, Loading both Nimbus 
	Isle and Nimbus Station)

	Added code and additional documentation done by Jon002 20.02.2015 (Character Creation, Custom Logins and success states, Dynamic Packets
	for Auth and Char servers)

	This source code requires RakNet version 3.25 as an external dependency to work with the LUNI Client.

	The source is open and free under the GPL License, Version 3 for use on a non-commercial basis
*/

#include "TimeUtil.h"

#include "Common.h"
#include "serverLoop.h"
#include "Database.h"
#include "AccountsDB.h"

#ifdef DEBUG
	#include "User.h"
	#include "Character.h"
#endif

#include "SUtil\IniReader.h"
#include "SUtil\Kbhit.h"
#include <thread>

#ifdef _WIN32
	// For mkdir command
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif

// Load the config file (config.ini)
void LoadConfig(CONNECT_INFO& auth, CONNECT_INFO& character, CONNECT_INFO& world) {
	CIniReader iniReader(".\\config.ini"); // Load config.ini
	
	// Default IP resorts to the auth.redirectIp BTW
	strcpy(auth.redirectIp, iniReader.ReadString("Settings", "redirect_ip", "127.0.0.1")); // Copy the auth redirect IP from the file
	strcpy(character.redirectIp, auth.redirectIp); // Copy the char redirect IP from the file
	strcpy(world.redirectIp, auth.redirectIp); // Copy the world redirect IP

	std::cout << "Server on IP " << auth.redirectIp << std::endl;

	// Get whether to use encryption
	auth.useEncryption = character.useEncryption = world.useEncryption = iniReader.ReadBoolean("Settings", "use_encryption", false);

	// Get whether to use log file
	auth.logFile = character.logFile = world.logFile = iniReader.ReadBoolean("Settings", "log_file", true);

	// NOTE: The default ports are the ports LU listens on / redirects on
	// Changing these may result in the game not running properly!
	auth.redirectPort = iniReader.ReadInteger("Auth", "redirect_port", 2002); // Get auth redirect port (default: 2002)
	auth.listenPort = iniReader.ReadInteger("Auth", "listen_port", 1001); // Get auth listen port (default: 1001)

	character.redirectPort = iniReader.ReadInteger("Char", "redirect_port", 2003); // Get char redirect port (default: 2003)
	character.listenPort = iniReader.ReadInteger("Char", "listen_port", 2002); // Get char listen port (default: 2002)

	world.redirectPort = iniReader.ReadInteger("World", "redirect_port", 2004); // Get world redirect port (default: 2004)
	world.listenPort = iniReader.ReadInteger("World", "listen_port", 2003); // Get world listen port (default: 2003)

	// Try to connect to the database...
	try {
		Database::Connect(iniReader.ReadString("MYSQL", "host", "localhost"), iniReader.ReadString("MYSQL", "database", "luni"), iniReader.ReadString("MYSQL", "username", "root"), iniReader.ReadString("MYSQL", "password", ""));
	}
	// Otherwise, exit the server
	catch (MySqlException& ex) {
		QuitError(ex.what());
	}

	// Print that the server loaded the config file!
	std::cout << "Loaded config! \nConnected to mysql database!\n\n";
}

// This is the entry point into the server (the main() function)
int main() {
	// Print starting info to the console
	std::cout << std::endl;
	std::cout << "  LL       EE    EE   GG     GG   OO\n";
	std::cout << "  LL       EE    EE   GGG    GG   OO\n";
	std::cout << "  LL       EE    EE   GGGG   GG   OO\n";
	std::cout << "  LL       EE    EE   GG GG  GG   OO\n";
	std::cout << "  LL       EE    EE   GG  GG GG   OO\n";
	std::cout << "  LL       EE    EE   GG   GGGG   OO\n";
	std::cout << "  LLLLLLL   EEEEEE    GG    GGG   OO\n";
	std::cout << std::endl;
	std::cout << "  Custom  LEGO (c)  Universe  Server\n";
	std::cout << std::endl;
	std::cout << "--------------------------------------\n";
	std::cout << "Original project: luniserver.sf.net\n\n";

	std::cout << "This version is based on Jon002s code.\n";
	std::cout << "This version is still very unstable!\nDon't be surprised if the server crashes!\n";
	std::cout << "--------------------------------------\n";
	std::cout << "Initializing LUNI test server...\n\nPress enter type \"help\" and enter again for commands\n\n";

	// Initialize the auth, character, and world CONNECT_INFO structs
	CONNECT_INFO auth, character, world;

	// Load the values for them and store them into their structs
	LoadConfig(auth, character, world);

	// Create the directory .//logs// if it does not exist
	_mkdir("logs");

	// If debug is on, print additional data to the console
	#ifdef DEBUG
	{
		std::cout << "\nDEBUG is ON!\n";
		//getTime();
		//cout << "\n Minifigure size: " << sizeof(MinifigureData) << endl;

		//auto testbin = OpenPacket(".\\char\\char_aw2.bin");
		//PacketCharacter * pk = (PacketCharacter*) testbin;

		/*vector< Ref<Character> > characters(4);
		auto aux = (uchar*)testbin.data() +22;
		characters[0] = Ref<Character>(new Character(aux));
		cout << "\nTest (reading characters form char\\char_aw2.bin) character1 : " << characters[0]->GetName() << endl;
		//cout << characters[0]->Data.travelling << endl;
		
		aux += characters[0]->GetGeneratedPacketSize();
		characters[1] = Ref<Character>(new Character(aux));
		cout << "\nTest character2 : " << characters[1]->GetName() << endl;*/

		//string s = "ciao";
		//RakNet::BitStream bs;
		//bs.WriteCompressed(s);

		//cout << endl << RawDataToString(bs.GetData(), bs.GetNumberOfBytesUsed()) << endl;
		//SavePacket("the Test.bin", (char*)bs.GetData(), bs.GetNumberOfBytesUsed());

		//cout << endl;
	}
	#endif

	// Create the UserPool to store all users by their systemAddress (shared by threads)
	Ref< UsersPool > OnlineUsers = new UsersPool();

	// Create a new CrossThreadQueue for writing output to the console from a thread
	Ref< CrossThreadQueue< std::string > > OutputQueue = new CrossThreadQueue< std::string >();

	//LUNI_AUTH = false;
	//LUNI_CHAR = false;
	//LUNI_WRLD = false;

	// Start the three new threads (Auth, Char, and World servers)
	std::thread thauth(AuthLoop, &auth, OnlineUsers, OutputQueue);
	std::thread thchar(CharactersLoop, &character, OnlineUsers, OutputQueue);
	std::thread thworld(WorldLoop, &world, OnlineUsers, OutputQueue);

	// If quit is ever equal to true, quit the server
	bool quit = false;

	// Keep the server from quitting by using a infinite loop
	while (!quit) {
		if (OutputQueue->Count() > 0) { // Pop the thread messages
			std::cout << OutputQueue->Pop();
		}

		if (_kbhit()) { // Parsing server commands. Press enter to start writing a command (may need to lock consoleoutmutex...)
			std::string command; // Initialize command string...
			std::cout << "> "; // Print "> " to show user where to type
			std::cin >> command; // Get the command
			std::cout << std::endl; // End the line

			// Match command to a pre-specified command here...
			if (command == "help") std::cout << "\nAvailable commands: \nquit = Quit the Server \nregister = Register New User \nuser_online = Show Number of Online Users\n";
			else if (command == "quit") quit = LUNIterminate = true;
			else if (command == "character_log_enable") character.logFile = true;
			else if (command == "character_log_disable") character.logFile = false;
			else if (command == "world_log_enable") world.logFile = true;
			else if (command == "world_log_disable") world.logFile = false;
			else if (command == "user_online") {
				std::cout << "\n Online user: " << OnlineUsers->Count() << std::endl;
			}
			else if (command == "register") {
				std::string username, password;
				std::cout << "Username: ";
				std::cin >> username; // Get the username
				if (AccountsTable::getAccountID( username ) == 0) { // Check to see if username already exists... If not, continue
					std::cout << "Password: ";
					std::cin >> password; // Get the password
					// Create the new user into the database
					ulonglong acid = AccountsTable::addAccount(username, password);
					if (acid > 0){
						std::cout << "[SERV] Account for '" << username << "' has been created with id " << acid << std::endl;
					}
				}
				else std::cout << "Username already exist!\n";
			}
			else if (command == "sessions"){
				std::cout << "Current session count: " << std::to_string(SessionsTable::count()) << std::endl;
			}
			else std::cout << "Invalid Command: " << command << "!" << std::endl;
		}
	}

	// No longer in use...
	thauth.join();
	std::cout << "[MAIN] AUTH ended" << std::endl;
	thchar.join();
	std::cout << "[MAIN] CHAR ended" << std::endl;
	thworld.join();
	std::cout << "[MAIN] WRLD ended" << std::endl;

	// If the loop was terminated, quit the server

	/*LUNIterminate = true;
	std::cout << "[MAIN] Waiting on Threads to exit" << std::endl;
	int i = 0;
	while (LUNI_AUTH || LUNI_CHAR || LUNI_WRLD){
		i++;
		//DO Nothing
		if (_kbhit()) {
			exit(0);
		}
	}*/

	exit(0);
}