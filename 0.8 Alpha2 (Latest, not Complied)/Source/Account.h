#pragma once
#include "AccountsDB.h"
#include "RakNet\RakNetTypes.h"

#include <iostream>

enum UserSuccess : char {
	SUCCESS = 0x01,
	BANNED,
	PERMISSION_LOW,
	UNKNOWN1,
	UNKNOWN2,
	INVALID_PASS,
	LOCKED,
	INVALID_USER,
	ACTIVATION_PENDING,
	ACCOUNT_DISABLED,
	GAME_TIME_EXPIRED,
	FREE_TRIAL_ENDED,
	PLAY_SCHEDULE_NOT_ALLOWED,
	ACCOUNT_NOT_ACTIVATED
};

namespace Session{
	void connect(SystemAddress address);
	void disconnect(SystemAddress address, SessionPhase source);

	void login(SystemAddress address, unsigned int accountid, std::string key, int instanceid);
	
	void play(unsigned int accountid, long long charid);
	void quit(long long charid);

	void enter(long long charid, unsigned short Zone);
	void leave(long long charid);

	void clearForInstance(int sessionid);
	bool sendToInstance(SystemAddress client, SystemAddress addr);
}