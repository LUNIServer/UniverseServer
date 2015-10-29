#pragma once
//This is a new IniReader, replacing the one from SUtil, and not using GetPrivateProfileString
#include <unordered_map>
#include <string>

class IniSection{
	std::unordered_map<std::string, std::string> values;
public:
	const bool getBoolValue(std::string key, bool defaultValue);
	const int getIntValue(std::string key, int defaultValue);
	const std::string getStringValue(std::string key, std::string defaultValue);
	void addPair(std::string key, std::string value);
	IniSection();
};

class IniFile{
	bool valid;
	IniSection * baseSection;
	std::unordered_map<std::string, IniSection *> sections;
public:
	IniSection * getSection(std::string section);
	const std::vector<std::string> getSections();
	bool isValid();
	IniFile(std::string filename);
	~IniFile();
};