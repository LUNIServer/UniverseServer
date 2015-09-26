#pragma once

#include <string>
#include <vector>

enum LogLevels : unsigned char{
	LOG_CRITICAL = 0,
	LOG_ERROR = 1,
	LOG_WARNING = 2,
	LOG_NORMAL = 3,
	LOG_DEBUG = 4,
	LOG_ALL = 5
};

#ifdef DEBUG
const LogLevels activeLogLevel = LogLevels::LOG_DEBUG;
#else
const LogLevels activeLogLevel = LogLevels::LOG_NORMAL;
#endif

class Logger{
	static std::string logfile;
	static bool tofile;
	static bool muted;
	static std::vector<std::string> logBuffer;
public:
	static void log(const std::string& source, const std::string& role, const std::string& message, LogLevels logLevel = LogLevels::LOG_NORMAL);
	//Log if an error message is present
	static void logError(const std::string& source, const std::string&role, const std::string& action, const std::string& error);
	static void mute();
	static void unmute(bool printBuffer = true);
	static bool setLogFile(std::string logFile);
};