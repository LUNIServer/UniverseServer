#pragma once

#include "Common.h"
#include "GameMessages.h"

#include "RakNet\RakNetTypes.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\PacketFileLogger.h"
#include "RakNet\StringCompressor.h"
#include "RakNet\BitStream.h"

static bool LUNIterminate; // Use for terminating threads
static bool getTerminate(){
	return LUNIterminate;
}

//Threads running?
//static bool LUNI_AUTH;
//static bool LUNI_CHAR;
//static bool LUNI_WRLD;

// Initialize RakNet security
void InitSecurity(RakPeerInterface* rakServer, bool useEncryption);

// Remove first 8 bytes form packet
uchar* CleanPacket(uchar* packetdata, uint len);

// Print packet information
void PrintPacketInfo(Packet* packet, PacketFileLogger* msgFileHandler);

// These two methods are used to send a packet to the client
void ServerSendPacket(RakPeerInterface* rakServer, char* data, uint len, const SystemAddress& addres);
void ServerSendPacket(RakPeerInterface* rakServer, const std::vector<uchar>& msg, const SystemAddress& addres);

// Open a specific packet
std::vector<uchar> OpenPacket(const std::string& filename);

// Save a packet
void SavePacket(const std::string& filename, const std::vector<uchar>& v);
void SavePacket(const std::string& filename, char* data, uint size);
void SavePacketOverwrite(const std::string& filename, const std::vector<uchar>& v);
void SavePacketOverwrite(const std::string& filename, char* data, uint size);

// Thse are the threads of the Authentication, Character, and World servers
void AuthLoop(CONNECT_INFO* cfg);
void WorldLoop(CONNECT_INFO* cfg);

void parsePacket(RakPeerInterface* rakServer, SystemAddress &systemAddress, RakNet::BitStream *data, ulong bytelength);
bool handleObject(ObjectInformation obj, RakPeerInterface* rakServer, SystemAddress &systemAddress);