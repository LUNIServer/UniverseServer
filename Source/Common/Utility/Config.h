#pragma once
#include "../../IniReader.h"
#include "../../Common.h"
#include <string>

struct Settings{
	std::string redirect_ip = "";
	bool log_file = false;
	bool use_encryption = false;
};

struct MySQLSettings{
	std::string host;
	std::string database;
	std::string username;
	std::string password;
};

class Configuration{
	IniFile * ini;
public:
	Configuration(std::string configFile);
	~Configuration();
	IniFile * getConfigFile();
	Settings getSettings();
	MySQLSettings getMySQLSettings();
	void setServerSettings(CONNECT_INFO& connection, Settings& settings, std::string& name);
};