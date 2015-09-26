#include "Packet.h"
#include "serverLoop.h"

#include <comdef.h>
#include <climits>
#include <string>
#include <iomanip>

#include "Logger.h"

using namespace RakNet;
using namespace std;

void CreatePacketHeader(MessageID messageId, unsigned short connectionType, unsigned long internalPacketId, BitStream *output) {
	unsigned char unknown1 = 0; // This is an unknown unsigned char
	
	// Write data to provided BitStream
	output->Write(messageId);
	output->Write(connectionType);
	output->Write(internalPacketId);
	output->Write(unknown1);
}

void SendInitPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, bool isAuth) {

	// Initialize the BitStream
	RakNet::BitStream bitStream;

	// Always create the packet header
	CreatePacketHeader(ID_USER_PACKET_ENUM, 0, 0, &bitStream);

	// Create the InitPacket from the InitPacket struct
	InitPacket charInitPacket = InitPacket(isAuth);

	// Write the char init packet to the bitstream
	bitStream.Write((char*)&charInitPacket, sizeof(charInitPacket));

	// Send the BitStream to the client
	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

void DoHandshake(RakPeerInterface *rakServer, const SystemAddress& systemAddress, RakNet::BitStream * data, std::string SERVER){
	unsigned long s_remoteConnectionType = 4;
	if (SERVER == "AUTH") s_remoteConnectionType = 1;
	unsigned long version;
	data->Read(version);
	unsigned long unknown;
	data->Read(unknown);
	unsigned long remoteConnectionType;
	data->Read(remoteConnectionType);
	unsigned long processid;
	data->Read(processid);
	unsigned short port;
	data->Read(port);
	std::vector<unsigned char> addv;
	addv.reserve(33);
	bool flag = true;
	std::string address = PacketTools::ReadStrFromPacket(data, 33); //Unused, no data
	Logger::log(SERVER, "", "Client: " + std::string(systemAddress.ToString()), LOG_ALL);
	Logger::log(SERVER, "", "Client Handshake Request", LOG_DEBUG);
	Logger::log(SERVER, "", "Version:        " + std::to_string(version), LOG_ALL);
	Logger::log(SERVER, "", "ProcessID:      " + std::to_string(processid), LOG_ALL);
	Logger::log(SERVER, "", "ConnectionType: " + std::to_string(remoteConnectionType), LOG_ALL);
	Logger::log(SERVER, "", "Port:           " + std::to_string(port), LOG_ALL);

	//Preparing answer:
	RakNet::BitStream *aw = new RakNet::BitStream(59);
	CreatePacketHeader(ID_USER_PACKET_ENUM, 0, 0, aw);
	unsigned long s_version = 171022UL;
	unsigned long s_unknown = 0x93;
	unsigned long s_processid = GetCurrentProcessId();
	short s_unknown2 = -1; //port = 0xFFFF = -1 -> no port?
	std::string s_ip = rakServer->GetLocalIP(0);
	Logger::log(SERVER, "", "Server Handshake Response", LOG_DEBUG);
	Logger::log(SERVER, "", "Version:        " + std::to_string(s_version), LOG_ALL);
	Logger::log(SERVER, "", "ProcessID:      " + std::to_string(s_processid), LOG_ALL);
	Logger::log(SERVER, "", "ConnectionType: " + std::to_string(s_remoteConnectionType), LOG_ALL);
	Logger::log(SERVER, "", "Address:        " + s_ip, LOG_ALL);
	aw->Write(s_version);
	aw->Write(s_unknown);
	aw->Write(s_remoteConnectionType);
	aw->Write(s_processid);
	aw->Write(s_unknown2);

	for (unsigned char k = 0; k < 33; k++){
		if (k < s_ip.size()) aw->Write((unsigned char)s_ip.at(k));
		else aw->Write((unsigned char)0);
	}
	rakServer->Send(aw, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

void WriteStringToBitStream(const char *myString, int stringSize, int maxChars, RakNet::BitStream *output) {
	// Write the string to provided BitStream along with the size
	output->Write(myString, stringSize);

	// Check to see if there are any bytes remaining according to user
	// specification
	int remaining = maxChars - stringSize;

	// If so, fill with 0x00
	for (int i = 0; i < remaining; i++) {
		unsigned char zero = 0;

		output->Write(zero);
	}
}

void DecompressStringFromBitStream(const char *myString, int maxChars, RakNet::BitStream *input) {
	//TODO: Does nothing right now... To be added later
}

std::wstring StringToWString(std::string& s) {
	// Convert the std::string &s into a wstring provided in the arguments
	std::wstringstream tempStream;
	tempStream << s.c_str();

	return tempStream.str();
}

std::wstring StringToWString(std::string& s, wchar_t size) {
	// Convert the std::string &s into a wstring provided in the arguments
	std::wstringstream tempStream;
	tempStream << s.c_str();

	std::wstring tempWString = tempStream.str();

	// Now resize the string to the specified size
	tempWString.resize(size);

	return tempWString;
}

std::wstring PacketTools::ReadFromPacket(RakNet::BitStream *bs, unsigned int size){
	std::vector<wchar_t> strv;
	strv.reserve(size);
	bool flag = true;
	for (unsigned int k = 0; k < size; k++){
		wchar_t c;
		bs->Read(c);
		if (c == 0){
			flag = false;
		}
		if (flag) {
			strv.push_back(c);
		}
	}
	std::wstring str(strv.begin(), strv.end());
	return str;
}

std::string PacketTools::ReadStrFromPacket(RakNet::BitStream *bs, unsigned int size){
	std::vector<char> strv;
	strv.reserve(size);
	bool flag = true;
	for (unsigned int k = 0; k < size; k++){
		char c;
		bs->Read(c);
		if (c == 0){
			flag = false;
		}
		if (flag) {
			strv.push_back(c);
		}
	}
	std::string str(strv.begin(), strv.end());
	return str;
}

//This Function writes a wstring of the specified size, the last char will always be \0
void PacketTools::WriteToPacket(RakNet::BitStream *bs, std::wstring str, unsigned int size){
	for (unsigned int k = 0; k < size - 1; k++){
		if (k < str.size()) bs->Write(str.at(k)); else bs->Write((unsigned short)0);
	}
	bs->Write((unsigned short)0);
}

//This function writes a wstring, of size str.size() + 1 to add a final \0 char
void PacketTools::WriteToPacket(RakNet::BitStream *bs, std::wstring str){
	PacketTools::WriteToPacket(bs, str, str.size() + 1);
}

void PacketTools::WriteToPacket(RakNet::BitStream *bs, std::string str, unsigned int size){
	for (unsigned int k = 0; k < size - 1; k++){
		if (k < str.size()) bs->Write(str.at(k)); else bs->Write((unsigned char)0);
	}
	bs->Write((unsigned char)0);
}

void PacketTools::printRest(RakNet::BitStream *bs){
	if (bs->GetNumberOfUnreadBits() > 0){
		unsigned long bytes = ((bs->GetNumberOfUnreadBits() - 1) >> 3) + 1;
		printBytes(bs, bytes);
	}
	else{
		Logger::log("DATA", "", "No Data", LOG_DEBUG);
	}
}

void PacketTools::printBytes(RakNet::BitStream *bs, unsigned long number){
	ostringstream buffer;
	for (unsigned long k = 0; k < number; k++){
		unsigned char chr;
		bs->Read(chr);
		buffer << setw(2) << hex << setfill('0') << (int)chr;
		buffer << " ";
		if ((k + 1) % 16 == 0){
			Logger::log("DATA", "", buffer.str(), LOG_DEBUG);
			buffer.str("");
			buffer.clear();
		}
	}
	if ((number % 16) != 0) Logger::log("DATA", "", buffer.str(), LOG_DEBUG);
}

// ----- NOTE: THESE SHOULD NOT BE USED RIGHT NOW AS WINDOWS ALREADY WRITES IN LITTLE ENDIAN ----- //

// Swap a unsigned short between little and big endian
unsigned short SwapUShort(unsigned short number) {
	unsigned short swappedNum = _byteswap_ushort(number);
	return swappedNum;
}

unsigned long SwapULong(unsigned long number) {
	unsigned long swappedNum = _byteswap_ulong(number);
	return swappedNum;
}

unsigned long long SwapULongLong(unsigned long long number) {
	unsigned __int64 numberToSwap = (unsigned __int64)number;
	unsigned __int64 swappedNum = _byteswap_uint64(numberToSwap);

	return (unsigned long long)swappedNum;
}