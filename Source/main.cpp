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
#include "IniReader.h"

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

std::string boolToString(bool value){
	if (value) return "TRUE"; else return "FALSE";
}

// Load the config file (config.ini)
void LoadConfig(std::string configFile, CONNECT_INFO& auth, CONNECT_INFO& world) {
	bool plain = exists(configFile);
	std::string locals = configFile.insert(0, ".\\");
	bool local = exists(locals);
	bool cfg = exists(".\\config.ini");	
	
	//char *cfile;

	if (local){
		configFile = locals;
	}
	else{
		if (!plain){
			if (cfg){
				configFile = ".\\config.ini";
			}else{
				configFile = "";
			}
		}
	}

	Logger::log("MAIN", "CONFIG", "Using Config-File: '" + configFile + "'", LOG_DEBUG);

	IniFile * config = new IniFile(configFile);

	IniSection * Settings = config->getSection("Settings");
	std::string redirect_ip = Settings->getStringValue("redirect_ip", "127.0.0.1");
	bool use_encryption = Settings->getBoolValue("use_encryption", false);
	bool log_file = Settings->getBoolValue("log_file", false);

	Logger::log("MAIN", "CONFIG", "Settings/redirect_ip: " + redirect_ip, LOG_ALL);
	Logger::log("MAIN", "CONFIG", "Settings/use_encryption: " + boolToString(use_encryption), LOG_ALL);
	Logger::log("MAIN", "CONFIG", "Settings/log_file: " + boolToString(log_file), LOG_ALL);

	IniSection * MySQL = config->getSection("MYSQL");

	std::string mysql_host = MySQL->getStringValue("host", "localhost");
	std::string mysql_db = MySQL->getStringValue("database", "luni");
	std::string mysql_user = MySQL->getStringValue("username", "root");
	std::string mysql_pass = MySQL->getStringValue("password", "");

	Logger::log("MAIN", "CONFIG", "MYSQL/host: " + mysql_host, LOG_ALL);
	Logger::log("MAIN", "CONFIG", "MYSQL/database: " + mysql_db, LOG_ALL);
	Logger::log("MAIN", "CONFIG", "MYSQL/username: " + mysql_user, LOG_ALL);
	Logger::log("MAIN", "CONFIG", "MYSQL/password: " + mysql_pass, LOG_ALL);

	IniSection * AuthConfig = config->getSection("Auth");

	//This config option is actually useless, since the client ALWAYS connects to port 1001 initially
	int auth_listen_port = AuthConfig->getIntValue("listen_port", 1001);
	int auth_redirect_port = AuthConfig->getIntValue("redirect_port", 2002);

	Logger::log("MAIN", "CONFIG", "AUTH/listen_port: " + std::to_string(auth_listen_port), LOG_ALL);
	Logger::log("MAIN", "CONFIG", "AUTH/redirect_port: " + std::to_string(auth_redirect_port), LOG_ALL);

	IniSection * WorldConfig = config->getSection("World");

	int wrld_listen_port = WorldConfig->getIntValue("listen_port", 1001);
	int wrld_redirect_port = WorldConfig->getIntValue("redirect_port", 2002);

	Logger::log("MAIN", "CONFIG", "WRLD/listen_port: " + std::to_string(wrld_listen_port), LOG_ALL);
	Logger::log("MAIN", "CONFIG", "WRLD/redirect_port: " + std::to_string(wrld_redirect_port), LOG_ALL);

	auth.listenPort = auth_listen_port;
	auth.redirectPort = auth_redirect_port;
	strcpy(auth.redirectIp, redirect_ip.c_str());
	auth.useEncryption = use_encryption;
	auth.logFile = log_file;

	world.listenPort = wrld_listen_port;
	world.redirectPort = wrld_redirect_port;
	strcpy(world.redirectIp, redirect_ip.c_str());
	world.useEncryption = use_encryption;
	world.logFile = log_file;
	
	unsigned int state = Database::Connect(mysql_host, mysql_db, mysql_user, mysql_pass);
	if (state > 0){
		QuitError("Could not connect to MYSQL database");
	}
	Logger::log("MAIN", "CONFIG", "Connected to mysql database!");

	if (redirect_ip == "127.0.0.1" || redirect_ip == "localhost"){
		Logger::log("MAIN", "WARNING", "AUTH will redirect to localhost, meaning MULTIPLAYER will NOT WORK", LOG_WARNING);
		Logger::log("MAIN", "WARNING", "To fix this, set 'redirect_ip' to the public IP of this computer", LOG_WARNING);
	}
	
	// Print that the server loaded the config file!
	Logger::log("MAIN", "CONFIG", "Loaded config!");
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
	std::cout << "Github (this version only):" << std::endl << "github.com/LUNIServer/UniverseServer" << std::endl;
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
	Logger::setLogFile("server.log");
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
				Logger::log("WRLD", "ARGS", "config", LOG_ALL);
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