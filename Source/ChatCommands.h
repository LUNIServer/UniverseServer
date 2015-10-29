#pragma once
#include "AccountsDB.h"

#include <vector>
#include <iomanip>
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

class FlightCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class TeleportCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class WhisperCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class TestmapCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class SwitchCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AddItemCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class PositionCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class ClientCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AttributeCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class PacketCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AnnouncementCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AdminCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class SpawnObjectCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class DeleteObjectCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class EquipNPCCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class UnequipNPCCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class NearMeCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};