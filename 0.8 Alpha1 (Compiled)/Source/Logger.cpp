#include "Logger.h"

#include "time.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

bool Logger::muted = false;
std::vector<std::string> Logger::logBuffer;
bool Logger::tofile = false;
std::string Logger::logfile = "";

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
		std::string msg = out.str();
		if (!Logger::muted)	std::cout << msg; else Logger::logBuffer.push_back(msg);
		if (Logger::tofile){
			std::ofstream file(Logger::logfile, std::ios::app);
			if (!file.is_open()) return;
			file.write(msg.data(), msg.length());
			file.close();
		}
	}
}

void Logger::logError(const std::string& source, const std::string&role, const std::string& action, const std::string& error){
	if (error != ""){
		Logger::log(source, role, "Error " + action, LOG_ERROR);
		Logger::log(source, role, error, LOG_ERROR);
	}
}

void Logger::mute(){
	Logger::muted = true;
}

void Logger::unmute(bool printLog){
	if (Logger::muted){
		if (printLog){
			for (unsigned int k = 0; k < Logger::logBuffer.size(); k++){
				std::cout << Logger::logBuffer.at(k);
			}
		}
		Logger::logBuffer.clear();
	}
	Logger::muted = false;
}

bool Logger::setLogFile(std::string logFile){
	std::ofstream file(logFile, std::ios::out);
	if (!file.is_open()) return false;
	Logger::logfile = logFile;
	Logger::tofile = true;
	file.close();
	return true;
}