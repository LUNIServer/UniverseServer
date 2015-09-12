#pragma once

#include "Common.h"

#include "RakNet\BitStream.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\MessageIdentifiers.h"


#include <intrin.h>

struct InitPacket {
	unsigned long versionId; // The game version
	unsigned long unknown1; // Dunno what this is...
	unsigned long remoteConnectionType; // This the remote connection type (1 = auth, 4 = other)
	unsigned long processId; // The process ID of the server
	unsigned short unknown2; // Dunno what this is either... it is "FF FF" in hex
	std::string ipString; // The IP string of the server (will be changed programmatically)

	InitPacket() {}

	InitPacket(bool isAuth) {
		// Set the variables
		versionId = 171022;
		unknown1 = 147;
		remoteConnectionType = isAuth ? 1 : 4; // Make sure to set this!!!! Determines whether remoteConnectionType should be 1 or 4
		processId = 5136;
		unknown2 = 65535;
		ipString = "172.24.8.139.un i = 15.„–<.„–<.Àˆ";
	}
};

// This is just a function that writes strings to a RakNet::BitStream
void WriteStringToBitStream(const char *myString, int stringSize, int maxChars, RakNet::BitStream *output);
void DecompressStringFromBitStream(const char *myString, int stringSize, int maxChars, RakNet::BitStream *input);

// Convert a std::string to std::wstring
std::wstring StringToWString(std::string &s);
std::wstring StringToWString(std::string& s, wchar_t size);

//Some more tools
namespace PacketTools {
	std::wstring ReadFromPacket(RakNet::BitStream *bs, unsigned int size);
	std::string ReadStrFromPacket(RakNet::BitStream *bs, unsigned int size);
	void WriteToPacket(RakNet::BitStream *bs, std::wstring str, unsigned int size);
	void WriteToPacket(RakNet::BitStream *bs, std::wstring str);

	void WriteToPacket(RakNet::BitStream *bs, std::string str, unsigned int size);

	void printRest(RakNet::BitStream *bs);
	void printBytes(RakNet::BitStream *bs, unsigned long number);
}

// Create a packet header that is used for almost all server packets
void CreatePacketHeader(MessageID messageId, unsigned short connectionType, unsigned long internalPacketId, RakNet::BitStream *output);

// Create the init packet known as init_aw2.bin
void SendInitPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, bool isAuth);
void DoHandshake(RakPeerInterface *rakServer, const SystemAddress& systemAddress, RakNet::BitStream * data, std::string SERVER);

// This will convert unsigned short, unsigned long, and unsigned long long to little endian (or back to big endian... does byte swap)
unsigned short SwapUShort(unsigned short number);
unsigned long SwapULong(unsigned long number);
unsigned long long SwapULongLong(unsigned long long number);