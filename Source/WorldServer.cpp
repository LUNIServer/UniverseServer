#include "WorldServer.h"
#include <stdio.h>

RakPeerInterface * WorldServer::rakServer = NULL;
ReplicaManager * WorldServer::replicaManager = NULL;

void WorldServer::publishWorldServer(RakPeerInterface* peer, ReplicaManager * rm){
	WorldServer::rakServer = peer;
	WorldServer::replicaManager = rm;
}

ReplicaManager * WorldServer::getRM(){
	return WorldServer::replicaManager;
}

RakNet::BitStream *WorldServer::initPacket(RemoteConnection conntype, unsigned long packetid){
	RakNet::BitStream *bs = new RakNet::BitStream();
	bs->Write((unsigned char) ID_USER_PACKET_ENUM);
	bs->Write((unsigned short) conntype);
	bs->Write(packetid);
	bs->Write((unsigned char)0);
	return bs;
}

void WorldServer::sendPacket(RakNet::BitStream * packet, SystemAddress address){
	rakServer->Send(packet, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

void WorldServer::savePacket(RakNet::BitStream * packet, std::string filename){
	std::ofstream file(filename, std::ios::out);
	if (!file.is_open()) return;

	file.write((char*)packet->GetData(), packet->GetNumberOfBytesUsed());
	file.close();
}

void writeNumber(RakNet::BitStream *aw, unsigned long n){
	if (n < 128){
		aw->Write((unsigned char)n);
	}
	else{
		aw->Write(true);
		aw->Write(true);
		if (n < 2048){
			aw->Write(false);
		}
		else{
			aw->Write(true);
			if (n < 65536){
				aw->Write(false);
			}
			else{
				if (n > 2097151){
					n = 2097151;
				}

				aw->Write(true);
				aw->Write(false);
				if ((n & 1048576) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 524288) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 262144) > 0) aw->Write(true); else aw->Write(false);

				aw->Write(true);
				aw->Write(false);
				if ((n & 131072) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 65536) > 0) aw->Write(true); else aw->Write(false);
			}
			if ((n & 32768) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 16384) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 8192) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 4096) > 0) aw->Write(true); else aw->Write(false);

			aw->Write(true);
			aw->Write(false);
			if ((n & 2048) > 0) aw->Write(true); else aw->Write(false);
		}
		if ((n & 1024) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 512) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 256) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 128) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 64) > 0) aw->Write(true); else aw->Write(false);

		aw->Write(true);
		aw->Write(false);
		if ((n & 32) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 16) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 8) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 4) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 2) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 1) > 0) aw->Write(true); else aw->Write(false);
	}
}

void writeNumber2(RakNet::BitStream *aw, unsigned long n){
	if (n < 128){
		aw->Write((unsigned char)n);
	}
	else{
		aw->Write(true);
		bool flag = (n > 2097151);
		if (n < 16383){
			if (flag) {
				if ((n & 268435456) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 134217728) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 67108864) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 33554432) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 16777216) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 8388608) > 0) aw->Write(true); else aw->Write(false);
				if ((n & 4194304) > 0) aw->Write(true); else aw->Write(false);
				aw->Write(true);
				if ((n & 2097152) > 0) aw->Write(true); else aw->Write(false);
			}
			if ((n & 1048576) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 524288) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 262144) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 131072) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 65536) > 0) aw->Write(true); else aw->Write(false);
			if ((n & 32768) > 0) aw->Write(true); else aw->Write(false);
			if (flag) {
				aw->Write(true);
			}
			if ((n & 16384) > 0) aw->Write(true); else aw->Write(false);
			if (!flag) {
				aw->Write(true);
			}
		}
		
		if ((n & 8192) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 4096) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 2048) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 1024) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 512) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 256) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 128) > 0) aw->Write(true); else aw->Write(false);

		if (!flag) aw->Write(false);

		if ((n & 64) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 32) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 16) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 8) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 4) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 2) > 0) aw->Write(true); else aw->Write(false);
		if ((n & 1) > 0) aw->Write(true); else aw->Write(false);
	}
}

void WorldServer::writeText(RakNet::BitStream *aw, std::string txt){
	unsigned long s1 = txt.size();
	unsigned long size = (s1 << 1) + 1;
	writeNumber2(aw, size);
	for (unsigned char k = 0; k < s1; k++){
		aw->Write((unsigned char)txt.at(k));
	}
}

void WorldServer::writeText2(RakNet::BitStream *aw, std::string txt){
	aw->Write((unsigned long)txt.size());
	for (unsigned char k = 0; k < txt.size(); k++){
		aw->Write((unsigned char)txt.at(k));
	}
}