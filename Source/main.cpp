/*
	Lego Universe Server WIP
	Code done by humanoid24 (aka Triver) as of 01.12.2013

	Additional credits to raffa505 30.01.2013 - 14.02.2013 (Database MySQL, Character, User, World, Color, Threads, Loading both Nimbus 
	Isle and Nimbus Station)

	Added code and additional documentation done by Jon002 20.02.2015 (Character Creation, Custom Logins and success states, Dynamic Packets
	for Auth and Char servers)

	This source code requires RakNet version 3.25 as an external dependency to work with the LU Client.

	The source is open and free under the GPL License, Version 3 for use on a non-commercial basis
*/

#include "TimeUtil.h"

#include "Common.h"
#include "serverLoop.h"
#include "Database.h"
#include "AccountsDB.h"

#include "SUtil\IniReader.h"
#include "SUtil\Kbhit.h"
#include <thread>
#include "Logger.h"

#ifdef _WIN32
	// For mkdir command
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif

// Load the config file (config.ini)
void LoadConfig(CONNECT_INFO& auth, CONNECT_INFO& world) {
	CIniReader iniReader(".\\config.ini"); // Load config.ini
	
	strcpy(auth.redirectIp, iniReader.ReadString("Settings", "redirect_ip", "127.0.0.1")); // Copy the auth redirect IP from the file
	strcpy(world.redirectIp, auth.redirectIp); // Copy the world redirect IP

	Logger::log("MAIN", "CONFIG", "Server on IP " + std::string(auth.redirectIp));

	// Get whether to use encryption
	auth.useEncryption = world.useEncryption = iniReader.ReadBoolean("Settings", "use_encryption", false);

	// Get whether to use log file
	auth.logFile = world.logFile = iniReader.ReadBoolean("Settings", "log_file", true);

	// NOTE: The default ports are the ports LU listens on / redirects on
	// Changing these may result in the game not running properly!
	auth.redirectPort = iniReader.ReadInteger("Auth", "redirect_port", 2003); // Get auth redirect port (default: 2002)
	auth.listenPort = iniReader.ReadInteger("Auth", "listen_port", 1001); // Get auth listen port (default: 1001)

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
	Logger::log("MAIN", "CONFIG", "Loaded config!");
	Logger::log("MAIN", "CONFIG", "Connected to mysql database!");
}

// This is the entry point into the server (the main() function)
int main() {
	// Print starting info to the console
	std::cout << std::endl;
	std::cout << "  LL       EE    EE   GG     GG   OO" << std::endl;
	std::cout << "  LL       EE    EE   GGG    GG   OO" << std::endl;
	std::cout << "  LL       EE    EE   GGGG   GG   OO" << std::endl;
	std::cout << "  LL       EE    EE   GG GG  GG   OO" << std::endl;
	std::cout << "  LL       EE    EE   GG  GG GG   OO" << std::endl;
	std::cout << "  LL       EE    EE   GG   GGGG   OO" << std::endl;
	std::cout << "  LLLLLLL   EEEEEE    GG    GGG   OO" << std::endl;
	std::cout << std::endl;
	std::cout << "  Custom  LEGO (c)  Universe  Server" << std::endl;
	std::cout << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	std::cout << "Original project: luniserver.sf.net" << std::endl;
	std::cout << "Github (main LUNI repo):" << std::endl << "github.com/jaller200/LUNIServerProject" << std::endl;
	std::cout << "Github (this version only):" << std::endl << "github.com/dsuser97/LUNI-Latest-Dev" << std::endl;
	std::cout << std::endl;
	std::cout << "This version is based on Jon002s code." << std::endl;
	std::cout << "This version is still very unstable!" << std::endl;
	std::cout << "Don't be surprised if the server crashes!" << std::endl;
	std::cout << "Please report any unreported issues on Github" << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	std::cout << "Press enter type \"help\" and enter again for commands" << std::endl;
	std::cout << std::endl;
	std::cout << "Server Log" << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	Logger::log("MAIN", "", "Initializing LUNI test server...");

	// Initialize the auth, character, and world CONNECT_INFO structs
	CONNECT_INFO auth, world;

	// Load the values for them and store them into their structs
	LoadConfig(auth, world);

	// Create the directory .//logs// if it does not exist
	_mkdir("logs");

	// If debug is on, print additional data to the console
	#ifdef DEBUG
	{
		// Setting this to LOG_NORMAL is done on purpose to avoid confusion
		// when activeLogLevel is below DEBUG but DEBUG is actually on
		Logger::log("MAIN", "", "DEBUG is ON!", LOG_NORMAL);
	}
	#endif

	// Create a new CrossThreadQueue for writing output to the console from a thread
	Ref< CrossThreadQueue< std::string > > OutputQueue = new CrossThreadQueue< std::string >();

	//LUNI_AUTH = false;
	//LUNI_CHAR = false;
	//LUNI_WRLD = false;

	// Start the three new threads (Auth, Char, and World servers)
	std::thread thauth(AuthLoop, &auth, OutputQueue);
	//std::thread thchar(CharactersLoop, &character, OutputQueue);
	std::thread thworld(WorldLoop, &world, OutputQueue);

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


			// Match command to a pre-specified command here...
			if (command == "help") {
				std::stringstream str;
				str << "Available commands:" << std::endl <<
					"quit        = Quit the Server" << std::endl <<
					"register    = Register New User" << std::endl <<
					"user_online = Show Number of Online Users" << std::endl <<
					"sessions    = Show Number of sessions" << std::endl;
				std::cout << str.str();
			}
			else if (command == "quit") quit = LUNIterminate = true;
			else if (command == "world_log_enable") world.logFile = true;
			else if (command == "world_log_disable") world.logFile = false;
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
						std::stringstream str;
						str << "Account for '" << username << "' has been created with id " << acid << std::endl;
						std::cout << str.str();
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
	//thchar.join();
	//std::cout << "[MAIN] CHAR ended" << std::endl;
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