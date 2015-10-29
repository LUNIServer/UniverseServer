#pragma once

#include "Common.h"
#include "GameMessages.h"

#include "RakNet\RakNetTypes.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\PacketFileLogger.h"
#include "RakNet\StringCompressor.h"
#include "RakNet\BitStream.h"

#include <vector>

static bool LUNIterminate; // Use for terminating threads
static bool getTerminate(){
	return LUNIterminate;
}

// Initialize RakNet security
void InitSecurity(RakPeerInterface* rakServer, bool useEncryption);

// Remove first 8 bytes form packet
unsigned char* CleanPacket(unsigned char* packetdata, unsigned int len);

// Print packet information
void PrintPacketInfo(Packet* packet, PacketFileLogger* msgFileHandler);

// These two methods are used to send a packet to the client
void ServerSendPacket(RakPeerInterface* rakServer, char* data, unsigned int len, const SystemAddress& addres);
void ServerSendPacket(RakPeerInterface* rakServer, const std::vector<unsigned char>& msg, const SystemAddress& addres);

// Open a specific packet
std::vector<unsigned char> OpenPacket(const std::string& filename);

// Save a packet
void SavePacket(const std::string& filename, const std::vector<unsigned char>& v);
void SavePacket(const std::string& filename, char* data, unsigned int size);
void SavePacketOverwrite(const std::string& filename, const std::vector<unsigned char>& v);
void SavePacketOverwrite(const std::string& filename, char* data, unsigned int size);

// Thse are the threads of the Authentication, Character, and World servers
void AuthLoop(CONNECT_INFO* cfg);
void WorldLoop(CONNECT_INFO* cfg);

void parsePacket(RakPeerInterface* rakServer, SystemAddress &systemAddress, RakNet::BitStream *data, unsigned long bytelength);
bool handleObject(ObjectInformation obj, RakPeerInterface* rakServer, SystemAddress &systemAddress);