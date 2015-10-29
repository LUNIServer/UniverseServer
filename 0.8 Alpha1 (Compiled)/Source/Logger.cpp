#include "Logger.h"

#include "time.h"
#include <sstream>
#include <iostream>
#include <iomanip>

void Logger::log(const std::string& source, const std::string& role, const std::string& message, LogLevels logLevel){
	if (logLevel <= activeLogLevel){
		std::stringstream out;
		time_t t = time(0);   // get time now
		struct tm * now = localtime(&t);
		out << "[";
		out << std::setfill('0') << std::setw(2) << now->tm_hour;
		out << ':';
		out << std::setfill('0') << std::setw(2) << now->tm_min;
		out << "] ";
		out << "[" << source << "] ";
		if (role != ""){
			out << "[" << role << "] ";
		}
		out << message << std::endl;
		std::cout << out.str();
	}
}

void Logger::logError(const std::string& source, const std::string&role, const std::string& action, const std::string& error){
	if (error != ""){
		Logger::log(source, role, "Error " + action, LOG_ERROR);
		Logger::log(source, role, error, LOG_ERROR);
	}
}