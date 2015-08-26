#pragma once
#include "ReplicaComponents.h"
#include "RakNet\Replica.h"

#include <vector>

class ReplicaObject : public Replica {
	std::vector<ReplicaComponent *> components;
public:
	ReplicaComponent *getComponent(unsigned int componentid);
	void addComponent(ReplicaComponent * component);
	void writeToPacket(RakNet::BitStream * packet, REPLICA_PACKET_TYPE packetType);
	void deleteComponents();
};