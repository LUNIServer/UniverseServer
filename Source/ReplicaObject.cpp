#include "ReplicaObject.h"
#include <algorithm>

ReplicaComponent *ReplicaObject::getComponent(unsigned int componentid){
	std::vector<ReplicaComponent *>::iterator it = std::find_if(components.begin(), components.end(), [&](ReplicaComponent * o){
		return o->getComponentID() == componentid;
	});
	if (it == components.end()){
		return NULL;
	}
	else{
		return *it;
	}
}

void ReplicaObject::addComponent(ReplicaComponent * component){
	this->components.push_back(component);
}

void ReplicaObject::writeToPacket(RakNet::BitStream * packet, REPLICA_PACKET_TYPE packetType){
	for (std::vector<ReplicaComponent *>::iterator it = components.begin(); it != components.end(); ++it){
		(*it)->writeToPacket(packet, packetType);
	}
}

void ReplicaObject::deleteComponents(){
	for (int i = components.size() - 1; i >= 0; i--){
		delete components.at(i);
		components.pop_back();
	}
}