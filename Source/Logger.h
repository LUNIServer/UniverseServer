#pragma once

#include <string>

class Logger{
public:
	static void log(const std::string& source, const std::string& role, const std::string& message);
	//Log if an error message is present
	static void logError(const std::string& source, const std::string&role, const std::string& action, const std::string& error);
};