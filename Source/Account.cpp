#include "Account.h"
#include "World.h"
#include "Social.h"
#include "Logger.h"

void Session::connect(SystemAddress address){
	//Logic for connection
	//This creates a client session for handshake and the like,
	//To be used as a reerence for this client

	//Creating the session
	SessionsTable::connect(address);
}

void Session::disconnect(SystemAddress address, SessionPhase source){
	//Logic for disconnection:
	//This function is called whenever a client disconnects. It looks up the session and
	//ensures a safe and error free disconnection without leaving any data behind

	SessionInfo info = SessionsTable::getClientSession(address);
	if (info.phase > source){ //Source is the phase of the server disconnected from, if it is to low, the disconnect should not happen
		return;
	}

	//Now step throug all levels of the disconnection process
	if (info.phase == SessionPhase::PHASE_INWORLD){
		// The player has quit the game from within the game, in a world
		// this can be caused by clicking "Logout" in the main menu or by closing the window
		std::cout << "[GAME] [SESSION] LEAVING WORLD" << std::endl;

		SessionInfo w = SessionsTable::leave(info.activeCharId);
		if (w.phase != SessionPhase::PHASE_NONE){
			std::cout << "[GAME] [SESSION] SUCCESS LEAVING WORLD" << std::endl;
			info = w;
		}
		else{
			std::cout << "[GAME] [SESSION] ERROR LEAVING WORLD" << std::endl;
		}
	}

	if (info.phase == SessionPhase::PHASE_PLAYING){
		//This can either be the next step from above or it may be caused by a disconnect while changing worlds
		std::cout << "[GAME] [SESSION] QUITTING PLAY" << std::endl;

		SessionInfo p = SessionsTable::quit(info.activeCharId);
		if (p.phase != SessionPhase::PHASE_NONE){
			std::cout << "[GAME] [SESSION] SUCCESS QUITTING PLAY" << std::endl;
			info = p;
		}
		else{
			std::cout << "[GAME] [SESSION] ERROR QUITTING PLAY" << std::endl;
		}
	}

	if (info.phase == SessionPhase::PHASE_AUTHENTIFIED){
		//This can be either the next step from above or it may be caused by pressing the back button
		//or closing the window on the character selection screen

		std::cout << "[GAME] [SESSION] LOGGING OUT" << std::endl;
		SessionInfo a = SessionsTable::logout(info.accountid);
		if (a.phase != SessionPhase::PHASE_NONE){
			std::cout << "[GAME] [SESSION] SUCCESS LOGGING OUT" << std::endl;
			info = a;
		}
		else{
			std::cout << "[GAME] [SESSION] ERROR LOGGING OUT" << std::endl;
		}
	}

	if (info.phase == SessionPhase::PHASE_CONNECTED){
		//This should only happen as a result of the last few

		std::cout << "[GAME] [SESSION] DISCONNECTING" << std::endl;
		SessionsTable::disconnect(info.addr);
	}
}

void Session::login(SystemAddress address, unsigned int accountid){
	Logger::log("GAME", "SESSION", std::string(address.ToString()) + " logging in with account id " + std::to_string(accountid));
	SessionsTable::connect(address);
	SessionsTable::login(address, accountid);
}

void Session::play(unsigned int accountid, long long charid){
	Logger::log("GAME", "SESSION", "account id " + std::to_string(accountid) + " playing with char " + std::to_string(charid));
	SessionsTable::play(accountid, charid);
}

void Session::quit(long long charid){
	Logger::log("GAME", "SESSION", "char " + std::to_string(charid) + " quitting play");
	SessionsTable::quit(charid);
}

void Session::enter(long long charid, unsigned short Zone){
	Logger::log("GAME", "SESSION", "char " + std::to_string(charid) + " joined " + (ZoneId)Zone);
	SessionsTable::enter(charid, Zone);
	Friends::checkFriendRequests(charid);
	Friends::checkFriendResponses(charid);
}

void Session::leave(long long charid){
	Logger::log("GAME", "SESSION", "char " + std::to_string(charid) + " left world");
	SessionsTable::leave(charid);
}