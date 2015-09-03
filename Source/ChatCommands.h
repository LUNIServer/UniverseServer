#pragma once
#include "AccountsDB.h"

#include <vector>
#include <unordered_map>

class ChatCommandHandler{
public:
	virtual void handleCommand(SessionInfo *s, std::vector<std::wstring> * params) = 0;
	virtual std::vector<std::wstring> getCommandNames() = 0;
	virtual std::wstring getDescription() = 0;
	virtual std::wstring getShortDescription() = 0;
	virtual std::wstring getSyntax() = 0;
};

class ChatCommandManager{
	static std::vector<ChatCommandHandler *> ChatCommandHandlers;
	static std::unordered_map<std::wstring, ChatCommandHandler *> ChatCommands;
public:
	static void registerCommands(ChatCommandHandler * CommandHandler);
	static void handleCommand(std::wstring command, SessionInfo *s, std::vector<std::wstring> * params);
};

class FlightCommandHandler : ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class TeleportCommandHandler : ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class WhisperCommandHandler : ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class TestmapCommandHandler : ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class SwitchCommandHandler : ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};