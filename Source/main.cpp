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

#include "Common.h"
#include "serverLoop.h"
#include "Database.h"
#include "AccountsDB.h"

#include "SUtil\IniReader.h"
#include <sys/stat.h>
#include <conio.h>
#include "Logger.h"

#include <sstream>

#ifdef _WIN32
	// For mkdir command
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif

enum ServerRole : unsigned char{
	ROLE_CONSOLE = 0,
	ROLE_AUTH,
	ROLE_WORLD,
};

struct ConfigInfo{

};

//Test for file existence
inline bool exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

// Load the config file (config.ini)
void LoadConfig(std::string configFile, CONNECT_INFO& auth, CONNECT_INFO& world) {
	bool plain = exists(configFile);
	std::string locals = configFile.insert(0, ".\\");
	bool local = exists(locals);
	bool config = exists(".\\config.ini");	
	
	char *cfile;

	if (local){
		configFile = locals;
	}
	else{
		if (!plain){
			if (config){
				configFile = ".\\config.ini";
			}else{
				configFile = "";
			}
		}
	}

	cfile = new char[configFile.length() + 1];
	strcpy(cfile, configFile.c_str());

	Logger::log("MAIN", "CONFIG", "File: '" + std::string(cfile) + "'");
	CIniReader iniReader(cfile); // Load config.ini
	
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
		char * db_host = iniReader.ReadString("MYSQL", "host", "localhost");
		char * db_database = iniReader.ReadString("MYSQL", "database", "luni");
		char * db_username = iniReader.ReadString("MYSQL", "username", "root");
		char * db_password = iniReader.ReadString("MYSQL", "password", "");
		Database::Connect(db_host, db_database, db_username, db_password);
	}
	// Otherwise, exit the server
	catch (MySqlException& ex) {
		QuitError(ex.what());
	}
	Logger::log("MAIN", "CONFIG", "Connected to mysql database!");

	// Print that the server loaded the config file!
	Logger::log("MAIN", "CONFIG", "Loaded config!");
	
	delete [] cfile;
}

void ConsoleLoop(){
	// If quit is ever equal to true, quit the server
	bool quit = false;

	// Keep the server from quitting by using a infinite loop
	while (!quit) {
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
					"sessions    = Show Number of sessions" << std::endl;
				std::cout << str.str();
			}
			else if (command == "quit") quit = LUNIterminate = true;
			else if (command == "register") {
				std::string username, password;
				std::cout << "Username: ";
				std::cin >> username; // Get the username
				if (AccountsTable::getAccountID(username) == 0) { // Check to see if username already exists... If not, continue
					std::cout << "Password: ";
					std::cin >> password; // Get the password
					// Create the new user into the database
					unsigned long long acid = AccountsTable::addAccount(username, password);
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
}

// This is the entry point into the server (the main() function)
int main(int argc, char* argv[]) {
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
	std::cout << "  Custom   LEGO    Universe   Server" << std::endl;
	std::cout << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	std::cout << "Originally created by raffa505" << std::endl;
	std::cout << "Original project: luniserver.sf.net" << std::endl;
	std::cout << "Github (main LUNI repo):" << std::endl << "github.com/jaller200/LUNIServerProject" << std::endl;
	std::cout << "Github (this version only):" << std::endl << "github.com/dsuser97/LUNI-Latest-Dev" << std::endl;
	std::cout << std::endl;
	std::cout << "This version is based on Jon002s code." << std::endl;
	std::cout << "This version is still very unstable!" << std::endl;
	std::cout << "Don't be surprised if the server crashes!" << std::endl;
	std::cout << "Please report any unreported issues on Github" << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	std::cout << "The  LEGO Group  has not  endorsed  or" << std::endl;
	std::cout << "authorized the  operation of this game " << std::endl;
	std::cout << "and  is  not  liable  for  any  safety" << std::endl;
	std::cout << "issues in relation to the operation of" << std::endl;
	std::cout << "this game." << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	std::cout << " Type \"help\", then ENTER for commands" << std::endl;
	std::cout << std::endl;
	std::cout << "Server Log" << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	Logger::log("MAIN", "", "Initializing LUNI test server...");
	
	// Args parser
	int state = 0;
	char * configFile = "";
	ServerRole Role = ROLE_CONSOLE;

	for (int argi = 0; argi < argc; argi++){
		std::string arg = std::string(argv[argi]);
		Logger::log("MAIN", "ARGS", arg, LOG_ALL);
		switch (state){
		case 0:
			if (arg == "--config"){
				state = 1;
				Logger::log("WRLD", "ARGS", "config");
			}
			if (arg == "--world") Role = ROLE_WORLD;
			if (arg == "--auth") Role = ROLE_AUTH;
			break;
		case 1:
			configFile = argv[argi];
			state = 0;
			break;
		}
	}

	// Initialize the auth, character, and world CONNECT_INFO structs
	CONNECT_INFO auth, world;

	// Load the values for them and store them into their structs
	LoadConfig(configFile, auth, world);

	// Create the directory .//logs// if it does not exist
	_mkdir("logs");

	// If debug is on, print additional data to the console
	#ifdef DEBUG
	{
		// Setting this to LOG_NORMAL is done on purpose to avoid confusion
		// when activeLogLevel is below DEBUG but DEBUG is actually on
		Logger::log("MAIN", "DEBUG", "is ON!", LOG_NORMAL);
	}
	#endif

	// Start the two new threads (Auth and World servers)
	if (Role == ROLE_AUTH) AuthLoop(&auth);
	if (Role == ROLE_WORLD) WorldLoop(&world);
	if (Role == ROLE_CONSOLE) ConsoleLoop();

	exit(0);
}