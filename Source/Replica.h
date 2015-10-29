#pragma once
#include "ReplicaData.h"

#include "Common.h"

#include "RakNet\BitStream.h"
#include "RakNet\NetworkIDManager.h"
#include "RakNet\Replica.h"

#include <string>
#include <vector>
#include <comdef.h>



void replicaPacketGeneral(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, long long objid, std::wstring name);

void replicaCreationPacket(RakNet::BitStream *Packet);

void replicaPacketLOT1i20(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, REPLICA_OBJECT_POSITION position);
void replicaPacketLOT1i22(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);
void replicaPacketLOT1i23(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, ulong health);
void replicaPacketLOT1i24(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);

void replicaPacketLOT1i27(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);
void replicaPacketLOT1i28(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);
void replicaPacketLOT1i29(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, PLAYER_STYLE playerStyle);

void replicaPacketLOT1i30(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);
void replicaPacketLOT1i31(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);
void replicaPacketLOT1i32(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);

void replicaPacketLOT1i36(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType);