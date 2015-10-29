#pragma once
#include "AccountsDB.h"
#include "RakNet\RakNetTypes.h"

#include <iostream>

namespace Session{
	void connect(SystemAddress address);
	void disconnect(SystemAddress address, SessionPhase source);

	void login(SystemAddress address, unsigned int accountid, std::string key);
	
	void play(unsigned int accountid, long long charid);
	void quit(long long charid);

	void enter(long long charid, unsigned short Zone);
	void leave(long long charid);

	void clearForInstance(int sessionid);
}