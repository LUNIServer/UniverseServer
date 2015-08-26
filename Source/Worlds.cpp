#include "Worlds.h"
#include "RakNet\BitStream.h"
#include "WorldServer.h"
#include "Logger.h"

bool Worlds::loadWorld(SystemAddress address, ZoneId zone, COMPONENT1_POSITION pos, unsigned short instance, unsigned long clone){
	RakNet::BitStream * stream = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::MSG_CLIENT_LOAD_STATIC_ZONE);

	if (zone == ZoneId::NO_ZONE){
		Logger::log("USER", "WARNING", "No zone selected, default to 1200 NIMBUS_STATION", LOG_DEBUG);
		zone = ZoneId::NIMBUS_STATION;
	}

	stream->Write((ushort)zone);
	stream->Write(instance); //Instance
	stream->Write(clone); //Clone

	std::vector<uchar> worldTarget = getWorldTarget(zone); //TODO: Replace this with actual numbers
	if (worldTarget.size() == 0) return false;
	stream->Write(worldTarget[2]); //Checksum
	stream->Write(worldTarget[3]);
	stream->Write(worldTarget[4]);
	stream->Write(worldTarget[5]); //End Checksum

	if (pos.isNull()){
		pos = getZoneSpawnPoint(zone);
	}
	stream->Write(pos.x);
	stream->Write(pos.y);
	stream->Write(pos.z);
	stream->Write((ulong)0);

	WorldServer::sendPacket(stream, address);
	return true;
}