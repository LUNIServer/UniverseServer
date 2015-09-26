#include "AuthPackets.h"
#include "serverLoop.h"
#include "Packet.h"
#include "md5.h"
#include "UtfConverter.h"

#include "RakNet\BitStream.h"

using namespace std;

void SendStatusPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, LoginStatusPacket loginStatusPacket) {
	RakNet::BitStream bitStream; // Create the bitStream

	// Always create the packet header
	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 0, &bitStream);

	// ---- CREATE BITSTREAM ---- //
	// Write the connectionId to bitStream
	bitStream.Write(loginStatusPacket.loginStatus);

	// Write the "Talk_Like_A_Pirate" string to bitStream
	WriteStringToBitStream(loginStatusPacket.talkLikeAPirate.c_str(), 18, 33, &bitStream);

	// Write 7 blocks of data (length: 33) to bitStream
	// In the original login_status_ok.bin, these sometimes ended in
	// bytes other than 0x00, but losing that doesn't seem to have much of an effect... yet.
	for (int i = 0; i < 7; i++) {
		WriteStringToBitStream(loginStatusPacket.unknownString.c_str(), 0, 33, &bitStream);
	}

	// Write the clientVersion to bitStream (all 3 parts)
	bitStream.Write(loginStatusPacket.clientVersion1);
	bitStream.Write(loginStatusPacket.clientVersion2);
	bitStream.Write(loginStatusPacket.clientVersion3);

	// Write the userKey, redirectIp, and chatIp to bitStream
	//WriteStringToBitStream(loginStatusPacket.userKey.c_str(), 66, 66, &bitStream);
	PacketTools::WriteToPacket(&bitStream, loginStatusPacket.userKey, 33);
	WriteStringToBitStream(loginStatusPacket.redirectIp.c_str(), sizeof(loginStatusPacket.redirectIp), 33, &bitStream);
	WriteStringToBitStream(loginStatusPacket.chatIp.c_str(), sizeof(loginStatusPacket.chatIp), 33, &bitStream);
	
	// Write the redirectPort and the chatPort to bitStream
	bitStream.Write(loginStatusPacket.redirectPort);
	bitStream.Write(loginStatusPacket.chatPort);
	
	// Write seperate ip(?) and possibleGuid to bitStream
	WriteStringToBitStream(loginStatusPacket.anotherIp.c_str(), sizeof(loginStatusPacket.anotherIp), 33, &bitStream);
	WriteStringToBitStream(loginStatusPacket.possibleGuid.c_str(), 37, 37, &bitStream);
	
	// Write the zeroShort to the errorMsgLength to the bitStream
	bitStream.Write(loginStatusPacket.zeroLong);
	bitStream.Write(loginStatusPacket.localizationChar[0]);
	bitStream.Write(loginStatusPacket.localizationChar[1]);
	bitStream.Write(loginStatusPacket.localizationChar[2]);
	bitStream.Write(loginStatusPacket.firstLoginSubscription);
	bitStream.Write(loginStatusPacket.subscribed);
	bitStream.Write(loginStatusPacket.zeroLongLong);
	bitStream.Write((unsigned short) loginStatusPacket.errorMsg.length());
	
	// Write the error msg string to the bitStream
	writeWString(UtfConverter::FromUtf8(loginStatusPacket.errorMsg), &bitStream, false);
	
	// Create extra packet data (even if not success, creates stamps in the client)
	CreateAllExtraPacketData(&bitStream);

	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
	//SavePacketOverwrite("test_login.bin", (char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
}

// Function to easily create extra packet data and write it to bitStream
void CreateExtraPacketData(unsigned long stampId, signed long bracketNum, unsigned long afterNum, RakNet::BitStream *bitStream) {
	unsigned long zeroPacket = 0;

	bitStream->Write(stampId);
	bitStream->Write(bracketNum);
	bitStream->Write(afterNum);
	bitStream->Write(zeroPacket);
}

// Add extra packet data to the server
void CreateAllExtraPacketData(RakNet::BitStream *bitStream) {
	bitStream->Write((unsigned long)320);
	CreateExtraPacketData(0, 0, 2803442767, bitStream);
	CreateExtraPacketData(7, 37381, 2803442767, bitStream);
	CreateExtraPacketData(8, 6, 2803442767, bitStream);
	CreateExtraPacketData(9, 0, 2803442767, bitStream);
	CreateExtraPacketData(10, 0, 2803442767, bitStream);
	CreateExtraPacketData(11, 1, 2803442767, bitStream);
	CreateExtraPacketData(14, 1, 2803442767, bitStream);
	CreateExtraPacketData(15, 0, 2803442767, bitStream);
	CreateExtraPacketData(17, 1, 2803442767, bitStream);
	CreateExtraPacketData(5, 0, 2803442767, bitStream);
	CreateExtraPacketData(6, 1, 2803442767, bitStream);
	CreateExtraPacketData(20, 1, 2803442767, bitStream);
	CreateExtraPacketData(19, 30854, 2803442767, bitStream);
	CreateExtraPacketData(21, 0, 2803442767, bitStream);
	CreateExtraPacketData(22, 0, 2803442767, bitStream);
	CreateExtraPacketData(23, 4114, 2803442767, bitStream);
	CreateExtraPacketData(27, 4114, 2803442767, bitStream);
	CreateExtraPacketData(28, 1, 2803442767, bitStream);
	CreateExtraPacketData(29, 0, 2803442767, bitStream);
	CreateExtraPacketData(30, 30854, 2803442767, bitStream);
}

