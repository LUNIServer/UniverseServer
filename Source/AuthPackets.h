#pragma once

#include "Common.h"
#include "Packet.h"

#include "RakNet\RakPeerInterface.h"
#include "RakNet\MessageIdentifiers.h"
#include "RakNet\BitStream.h"

using namespace std;

struct LoginStatusPacket {
	unsigned char loginStatus; // This is the Connection ID
	std::string talkLikeAPirate; // Always is "Talk_Like_A_Pirate", yet 33 bytes long
	std::string unknownString; // Have NO idea what this is... Is it relevant?

	unsigned short clientVersion1; // For some reason, the client version
	unsigned short clientVersion2; // is split up over 3 unsigned shorts
	unsigned short clientVersion3; // I believe...

	std::string unknown; // This is 237 bytes long...

	std::wstring userKey; // Generated User Key - Should be wstring
	std::string redirectIp; // Redirect IP Address
	std::string chatIp; // Chat IP Address
	unsigned short redirectPort; // Redirect Port
	unsigned short chatPort; // Chat Port
	std::string anotherIp; // Another IP Address? 33 bytes long
	std::string possibleGuid; // In the form of a UUID (maybe a GUID?)
	unsigned long zeroLong; // Always 0 for some reason...
	unsigned char localizationChar[3]; // Can be IT, US, etc... Always seems to end in 0
	unsigned char firstLoginSubscription; // Check - Is this the first login after subscribing? 
	unsigned char subscribed; // 0 if subscribed, 1 if not subscribed (for LUNI, always 0)
	unsigned long long zeroLongLong; // This is all zeros...
	unsigned short errorMsgLength; // This is the length of the error msg
	std::string errorMsg; // This is the error message displayed if connection ID is 0x05. Can be X bytes long (I believe) - Should be wstring
	
	// Extra bytes
	//unsigned long extraBytesLength; // This is the number of bytes left (number of each chunk of extra data = 16 bytes * x chunks + these 4 bytes

	// Initializer
	LoginStatusPacket() { }
};

// Sends a packet with the login status to the client (good, bad, etc..)
void SendStatusPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, LoginStatusPacket loginStatusPacket);

// This extra packet data occurs in auth packets.
void CreateExtraPacketData(unsigned long stampId, signed long bracketNum, unsigned long afterNum, RakNet::BitStream *bitStream);

// Create the Extra Packet data based on success packet (login_ok.bin)
void CreateAllExtraPacketData(RakNet::BitStream *bitStream);