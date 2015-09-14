#include "Config.h"
#include "../../Logger.h"
#include <sys/stat.h>

inline bool exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

Configuration::Configuration(std::string configFile){
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
			}
			else{
				configFile = "";
			}
		}
	}

	Logger::log("MAIN", "CONFIG", "Using File: '" + configFile + "'", LOG_DEBUG);

	this->ini = new IniFile(configFile);
}

Configuration::~Configuration(){
	delete this->ini;
}

IniFile * Configuration::getConfigFile(){
	return this->ini;
}

Settings Configuration::getSettings(){
	Settings settings;
	IniSection * Settings = this->ini->getSection("Settings");
	settings.redirect_ip = Settings->getStringValue("redirect_ip", "127.0.0.1");
	settings.use_encryption = Settings->getBoolValue("use_encryption", false);
	settings.log_file = Settings->getBoolValue("log_file", false);
	return settings;
}

MySQLSettings Configuration::getMySQLSettings(){
	MySQLSettings mysql;
	IniSection * MySQL = this->ini->getSection("MYSQL");
	mysql.host = MySQL->getStringValue("host", "localhost");
	mysql.database = MySQL->getStringValue("database", "luni");
	mysql.username = MySQL->getStringValue("username", "root");
	mysql.password = MySQL->getStringValue("password", "");
	return mysql;
}

void Configuration::setServerSettings(CONNECT_INFO& connection, Settings& settings, std::string& name){
	IniSection * AuthConfig = this->ini->getSection(name);

	//This config option is actually useless, since the client ALWAYS connects to port 1001 initially
	int listen_port = AuthConfig->getIntValue("listen_port", 1001);
	int redirect_port = AuthConfig->getIntValue("redirect_port", 2002);

	connection.listenPort = listen_port;
	connection.redirectPort = redirect_port;
	strcpy(connection.redirectIp, settings.redirect_ip.c_str());
	connection.useEncryption = settings.use_encryption;
	connection.logFile = settings.log_file;
}