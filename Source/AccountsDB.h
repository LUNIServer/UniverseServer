#pragma once
#include <string>
#include <unordered_map>
#include "CharactersDB.h"
#include "RakNet\RakNetTypes.h"

struct AccountAccessInfo{
	bool locked = false;
	bool banned = false;
	char loginTries = 0;
};

struct CharacterCount{
	unsigned char count = 0;
	unsigned char front = 0;
};

enum SessionPhase{
	PHASE_NONE = 0, //Session is invalid
	PHASE_CONNECTED, //Client connected, but did not log in
	PHASE_AUTHENTIFIED, //Client has successfully authetified with an account
	PHASE_PLAYING, //Client has choosen a character to play with
	PHASE_INWORLD //Client is playing in a world
};

struct SessionInfo{
	SessionPhase phase = SessionPhase::PHASE_NONE;

	SystemAddress addr;
	std::string sessionkey = "";
	unsigned int accountid = 0;
	long long activeCharId = -1;
	unsigned int worldJoin = 0;
	unsigned short zone = 0;
	
	SessionInfo(){}
	SessionInfo(SystemAddress addr){
		this->addr = addr;
		this->phase = SessionPhase::PHASE_CONNECTED;
		this->accountid = 0;
		this->activeCharId = -1;
		this->worldJoin = 0;
		this->zone = 0;
		this->sessionkey = "";
	}
};

//struct SystemAddressHasher {
//	size_t operator()(const SystemAddress& t) const {
//		return t.binaryAddress;
//	}
//};

class AccountsTable{
public:
	//Returns the id of an Account, and 0 if nothing is found
	static unsigned int getAccountID(std::string username);
	//Returns the id of the created Account and 0 if creation was impossible
	static unsigned long long addAccount(std::string name, std::string password);
	//Returns true if Password is valid for that account
	static bool checkPassword(std::string password, unsigned int accountid );
	//Returns information about the accessability of the account
	static AccountAccessInfo getAccessInfo(unsigned int accountid);
	//Sets the AccountAccessInfo
	static void setAccessInfo(unsigned int accountid, AccountAccessInfo info);

	//static CharacterCount getCharCountInfo(unsigned int accountid);
	static bool setFrontChar(long long charid);
	static void unsetFrontChar(unsigned int accountid);
	static long long getFrontChar(unsigned int accountid);
};

/*
	Since the purpose of these database classes are to seperate the background implementation from the
	core functionality of the game, the sessions table is one example of a table that could be done with a database,
	but is instead done via an internal std::unordered_map
*/

class SessionsTable{
private:
	//static std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher> sessions;
public:
	//Connection
	static SessionInfo connect(SystemAddress address);
	static bool disconnect(SystemAddress address);

	static SessionInfo getClientSession(SystemAddress address);

	//Authentification
	static SessionInfo login(SystemAddress address, unsigned int accountid, std::string key);
	static SessionInfo logout(unsigned int accountid);

	static SystemAddress findAccount(unsigned int accountid);
	
	//Character selection
	static SessionInfo play(unsigned int accountid, long long charid);
	static SessionInfo quit(long long charid);

	static SystemAddress findCharacter(long long charid);
	static std::vector<SessionInfo> getClientsInWorld(unsigned short zoneid);
	static std::vector<SessionInfo> getClientsInInstance(int instanceid);

	//Worlds
	static SessionInfo enter(long long charid, unsigned short zoneId);
	static SessionInfo leave(long long charid);

	static unsigned int count();
};