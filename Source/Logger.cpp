#include "Logger.h"

#include "time.h"
#include <iostream>
#include <iomanip>

void Logger::log(const std::string& source, const std::string& role, const std::string& message){
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	std::cout << "[";
	std::cout << std::setfill('0') << std::setw(2) << now->tm_hour;
	std::cout << ':';
	std::cout << std::setfill('0') << std::setw(2) << now->tm_min;
	std::cout << "] ";
	std::cout << "[" << source << "] ";
	if (role != ""){
		std::cout << "[" << role << "] ";
	}
	std::cout << message << std::endl;
}

void Logger::logError(const std::string& source, const std::string&role, const std::string& action, const std::string& error){
	if (error != ""){
		Logger::log(source, role, "Error " + action);
		Logger::log(source, role, error);
	}
}