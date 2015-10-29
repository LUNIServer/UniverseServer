#include "Account.h"
#include "World.h"
#include "Social.h"
#include "Logger.h"
#include "WorldServer.h"
#include "ServerDB.h"

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
	
	SystemAddress server = WorldServer::getServerAddress();
	if (server != UNASSIGNED_SYSTEM_ADDRESS){
		int instanceid = InstancesTable::getInstanceId(server);
		if (instanceid > -1){
			if (instanceid != info.instanceid){
				return;
			}
		}
	}

	
	if (info.phase > source){ //Source is the phase of the server disconnected from, if it is to low, the disconnect should not happen
		return;
	}

	//Now step throug all levels of the disconnection process
	if (info.phase == SessionPhase::PHASE_INWORLD){
		// The player has quit the game from within the game, in a world
		// this can be caused by clicking "Logout" in the main menu or by closing the window
		Logger::log("GAME", "SESSION", "LEAVING WORLD", LOG_DEBUG);

		SessionInfo w = SessionsTable::leave(info.activeCharId);
		if (w.phase != SessionPhase::PHASE_NONE){
			Logger::log("GAME", "SESSION", "SUCCESS LEAVING WORLD", LOG_DEBUG);
			info = w;
		}
		else{
			Logger::log("GAME", "SESSION", "ERROR LEAVING WORLD", LOG_DEBUG);
		}
	}

	if (info.phase == SessionPhase::PHASE_PLAYING){
		//This can either be the next step from above or it may be caused by a disconnect while changing worlds
		Logger::log("GAME", "SESSION", "QUITTING PLAY", LOG_DEBUG);

		SessionInfo p = SessionsTable::quit(info.activeCharId);
		if (p.phase != SessionPhase::PHASE_NONE){
			Logger::log("GAME", "SESSION", "SUCCESS QUITTING PLAY", LOG_DEBUG);
			info = p;
		}
		else{
			Logger::log("GAME", "SESSION", "ERROR QUITTING PLAY", LOG_DEBUG);
		}
	}

	if (info.phase == SessionPhase::PHASE_AUTHENTIFIED){
		//This can be either the next step from above or it may be caused by pressing the back button
		//or closing the window on the character selection screen

		Logger::log("GAME", "SESSION", "LOGGING OUT", LOG_DEBUG);
		SessionInfo a = SessionsTable::logout(info.accountid);
		if (a.phase != SessionPhase::PHASE_NONE){
			Logger::log("GAME", "SESSION", "SUCCESS LOGGING OUT", LOG_DEBUG);
			info = a;
		}
		else{
			Logger::log("GAME", "SESSION", "ERROR LOGGING OUT", LOG_DEBUG);
		}
	}

	if (info.phase == SessionPhase::PHASE_CONNECTED){
		//This should only happen as a result of the last few

		Logger::log("GAME", "SESSION", "DISCONNECTING", LOG_DEBUG);
		SessionsTable::disconnect(info.addr);
	}
}

void Session::login(SystemAddress address, unsigned int accountid, std::string key, int instanceid){
	Logger::log("GAME", "SESSION", std::string(address.ToString()) + " logging in with account id " + std::to_string(accountid));
	SessionsTable::connect(address);
	SessionsTable::login(address, accountid, key, instanceid);
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

void Session::clearForInstance(int instanceid){
	std::vector<SessionInfo> sess = SessionsTable::getClientsInInstance(instanceid);
	for (unsigned int i = 0; i < sess.size(); i++){
		SessionsTable::disconnect(sess.at(i).addr);
	}
}

bool Session::sendToInstance(SystemAddress client, SystemAddress addr){
	SessionInfo s = SessionsTable::getClientSession(client);
	int newinstanceid = InstancesTable::getInstanceId(addr);
	if (newinstanceid > -1){
		Logger::log("WRLD", "ACCT", "Sending char to new instance", LOG_DEBUG);
		SessionsTable::setInstanceId(s.accountid, newinstanceid);
		return true;
	}
	return false;
}