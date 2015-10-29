#pragma once

#include "PlayerObject.h"
#include "Common.h"
#include "Character.h"


#include "RakNet\RakNetTypes.h"
#include "RakNet\NetworkIDManager.h"

#include <vector>

// This is a struct that provides names for User login success values
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

class User {
	private:
		//TODO: TEMPORARY player instance. In the future, this has to be coupled to the User Object and you have to be able to reference it in some sort of table (possibly NetworkID)
		PlayerObject *player;
		uint id; // This is the ID of the Users
		std::string username; // This is the username of User
		long long characters[4]; // Array of User Characters IDs
		std::vector< Ref<Character> > charctersData;
		Ref<Character> current; // Current Character
		SystemAddress ip; // This is the SystemAddress from RakNet
		
	public:
		// This is the User initialization. It takes a User ID, a Username, and an SystemAddress
		User(uint id, const std::string& username, const SystemAddress& systemAddress);
		uint numredir = 1; // Number of redirects
		ulong nextcid = 0; // Only for development - this is the next character ID (CID = Character ID)

		uchar nameInUse = 0; // Whether or not the selected username / predefined name is in use (stores locally)

		// Initializer
		User(const User& usr);

		// Return a vector of characters for the selection screen
		std::vector< Ref<Character> > LoadCharacters();
		
		//Set Character on Login
		void SetCharacter(long long cid);
		
		bool ChangeWorld(ZoneId zone, RakPeerInterface* rakServer, SystemAddress &systemAddress);
		bool LoadWorld(ZoneId zone, RakNet::BitStream *stream);
		ZoneId getWorld();

		//PlayerObject
		void SetPlayer(PlayerObject * p);
		void CreatePlayer();
		PlayerObject *GetPlayer();
		void DestructPlayer();

		// Login, return users. NULL if fails to login
		//static Ref<User> Login(const std::string& nikname, const std::string& password, const SystemAddress& systemAddress);

		uint GetID(); // Get the ID of the user
		Ref<Character> GetCurrentCharacter(); // Get the current character
		SystemAddress GetIp(); // Get the current SystemAddress
		std::string GetUsername(); // Get the current Username

		RakNet::BitStream *sendMessage(std::wstring message, std::wstring sender = L"System", bool isMythran = false);
		void getChardata(long long objid, RakNet::BitStream *packet);

		destructor User(); // User destructor
};

void sendGameMessage(long long objid, RakNet::BitStream *packet, ushort message);