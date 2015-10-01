#include "ReplicaComponents.h"

#include "Database.h"
#include "InventoryDB.h"

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <sstream>

//TODO: this works for ONE player, but in the end, every player needs to know everything from every other player.
//Therefore we have to add special flags for creation, which don't get reset
//Actually No, it seems serialization is unique for every client, so we would have to store the updated data for every single client!

//TODO: The solution to the problem mentioned above will follow this Procedure:
// 1) Update the player->serialize() to only broadcast to players in this world, like the ->create() is used for
// 2) move that code to the playerObject, resulting in a	->create() calling ->create(SystemAddress)
//															->serialize() calling ->serialize(SystemAddress)
// 3) possibly add destruct in the same way
// 4) extend the ReplicaComponent Interface to have a function to reset it's flags.
//		this will be called by ->serialize() after all serializations are done.
//		then any client that doesn't know of the change will recieve any relevant information through the creation packet
// 5) Consequently, at creation time, the Components should serialize all data that is not set to its respective default value
// EFFECT:	This way of handling serialization will nicely integrate with the current system of seperating worlds,
//			and later instances. It will ensure that only the necessary data will get sent and updated and not e.g.
//			the equipment data which hasn't changed on every position update serialization
//NOTE: Components could use a special struct for their flags, having one struct that holds information about what contains data
//		(for creation) and on that holds information about what has just updated

#pragma region Component108 (Name Unknown)

Component108::Component108() {
	flag1 = true;
	flag2 = true;
	flag3 = true;
}

Component108::~Component108() {
	
}

void Component108::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(flag2);
		if (flag2) {
			packet->Write(driverObjID);
		}
		packet->Write(flag3);
		if (flag3) {
		}			packet->Write(data3_1);

		packet->Write(data4);
	}
}

unsigned int Component108::getComponentID(){ return 108; }

bool Component108::getFlag1(){ return flag1; }
bool Component108::getFlag2(){ return flag2; }
long long Component108::getDriverObjID(){ return driverObjID; }
bool Component108::getFlag3(){ return flag3; }
long Component108::getData3_1(){ return data3_1; }
bool Component108::getData4(){ return data4; }

void Component108::setFlag1(bool flag1){ this->flag1 = flag1; }
void Component108::setFlag2(bool flag2){ this->flag2 = flag2; }
void Component108::setDriverObjID(long long driverObjID){ this->driverObjID = driverObjID; }
void Component108::setFlag3(bool flag3){ this->flag3 = flag3; }
void Component108::setData3_1(long data3_1){ this->data3_1 = data3_1; }
void Component108::setData4(bool data4){ this->data4 = data4; }

#pragma endregion

// No data has been discovered for this component...
#pragma region ModuleAssemblyComponent (Component Unknown)

#pragma endregion

#pragma region ControllablePhysicsComponent (Component 1)
void ControllablePhysicsComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_PACKET_TYPE::REPLICA_CONSTRUCTION_PACKET){
		packet->Write(this->flag1);
		if (this->flag1){
			packet->Write(this->data1.d1);
			packet->Write(this->data1.d2);
			packet->Write(this->data1.d3);
			//this->flag1 = false;
		}
		packet->Write(this->flag2);
		if (this->flag2){
			packet->Write(this->data2);
			//this->flag2 = false;
		}
	}
	packet->Write(this->flag3);
	if (this->flag3){
		packet->Write(this->data3);
		//this->flag3 = false;
	}
	packet->Write(flag4);
	if (flag4){
		packet->Write(data4.d1);
		packet->Write(data4.d2);
		//flag4 = false;
	}
	packet->Write(flag5);
	if (flag5){
		packet->Write(flag5_1);
		if (flag5_1){
			packet->Write(data5.d1);
			packet->Write(data5.d2);
			//flag5_1 = false;
		}
	}
	packet->Write(flag6);
	if (flag6){
		packet->Write(this->data6.position);
		packet->Write(this->data6.rotation);
		packet->Write(this->data6.onGround);
		packet->Write(this->data6.d4);
		packet->Write(this->flag6_1);
		if (flag6_1){
			packet->Write(data6_1);
			//flag6_1 = false;
		}
		packet->Write(this->flag6_2);
		if (flag6_2){
			packet->Write(data6_2);
			//flag6_2 = false;
		}
		packet->Write(flag6_3);
		if (flag6_3){
			packet->Write(data6_3);
			packet->Write(flag6_3_1);
			if (flag6_3_1){
				packet->Write(data6_3_1);
				//flag6_3_1 = false;
			}
			//flag6_3 = false;
		}
		if (packetType == REPLICA_PACKET_TYPE::REPLICA_SERIALIZATION_PACKET){
			packet->Write(flag6_4);
		}
		//flag6 = false;
	}
}

unsigned int ControllablePhysicsComponent::getComponentID(){ return 1; }

ControllablePhysicsComponent::ControllablePhysicsComponent(){
	this->flag1 = false;
	this->flag2 = false;
	this->flag3 = false;
	this->flag4 = false;
	this->flag5 = false;
	this->flag5_1 = false;
	this->flag6 = false;
	this->flag6_1 = false;
	this->flag6_2 = false;
	this->flag6_3 = false;
	this->flag6_4 = false;
}

ControllablePhysicsComponent::~ControllablePhysicsComponent(){

}

void ControllablePhysicsComponent::setData1(COMPONENT1_DATA1 data1){ this->data1 = data1; this->flag1 = true; }
COMPONENT1_DATA1 ControllablePhysicsComponent::getData1(){ return this->data1; }
void ControllablePhysicsComponent::setData2(COMPONENT1_DATA2 data2){ this->data2 = data2; this->flag2 = true; }
COMPONENT1_DATA2 ControllablePhysicsComponent::getData2(){ return this->data2; }
void ControllablePhysicsComponent::setData3(COMPONENT1_DATA3 data3){ this->data3 = data3; this->flag3 = true; }
COMPONENT1_DATA3 ControllablePhysicsComponent::getData3(){ return this->data3; }
void ControllablePhysicsComponent::setData4(COMPONENT1_DATA4 data4){ this->data4 = data4; this->flag4 = true; }
COMPONENT1_DATA4 ControllablePhysicsComponent::getData4(){ return this->data4; }
void ControllablePhysicsComponent::setData5(COMPONENT1_DATA5 data5){ this->data5 = data5; this->flag5_1 = true; }
COMPONENT1_DATA5 ControllablePhysicsComponent::getData5(){ return this->data5; }
void ControllablePhysicsComponent::setHasData5(bool flag){ flag5 = flag; }
bool ControllablePhysicsComponent::getHasData5(){ return flag5; }
void ControllablePhysicsComponent::setPosition(COMPONENT1_POSITION pos){ this->data6.position = pos; flag6 = true; }
COMPONENT1_POSITION ControllablePhysicsComponent::getPosition(){ return this->data6.position; }
void ControllablePhysicsComponent::setRotation(COMPONENT1_ROTATION rot){ this->data6.rotation = rot; flag6 = true; }
COMPONENT1_ROTATION ControllablePhysicsComponent::getRotation(){ return this->data6.rotation; }
void ControllablePhysicsComponent::setOnGround(bool onGround){ this->data6.onGround = onGround; flag6 = true; }
bool ControllablePhysicsComponent::getOnGround(){ return this->data6.onGround; }
void ControllablePhysicsComponent::setData6_d4(bool d){ this->data6.d4 = d; flag6 = true; }
bool ControllablePhysicsComponent::getData6_d4(){ return this->data6.d4; }
void ControllablePhysicsComponent::setVelocity(COMPONENT1_VELOCITY vel){ this->data6_1 = vel; flag6 = true;  flag6_1 = true; }
COMPONENT1_VELOCITY ControllablePhysicsComponent::getVelocity(){ return this->data6_1; }
void ControllablePhysicsComponent::setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR avel){ this->data6_2 = avel; flag6 = true, flag6_2 = true; }
COMPONENT1_VELOCITY_ANGULAR ControllablePhysicsComponent::getAngularVelocity(){ return this->data6_2; }
void ControllablePhysicsComponent::setData6_3(COMPONENT1_DATA6_3 data6_3){ this->data6_3 = data6_3; flag6 = true; flag6_3 = true; }
COMPONENT1_DATA6_3 ControllablePhysicsComponent::getData6_3(){ return this->data6_3; }
void ControllablePhysicsComponent::setData6_3_1(COMPONENT1_DATA6_3_1 data6_3_1){ this->data6_3_1 = data6_3_1; flag6 = true; flag6_3 = true; flag6_3_1 = true; }
COMPONENT1_DATA6_3_1 ControllablePhysicsComponent::getData6_3_1(){ return this->data6_3_1; }

#pragma endregion

#pragma region SimplePhysicsComponent (Component 3)

SimplePhysicsComponent::SimplePhysicsComponent() {
	this->flag1 = true;
	this->data1 = 0.0;
	this->flag2 = false;
	this->flag3 = true;
	this->data3 = 5.0;
}

SimplePhysicsComponent::~SimplePhysicsComponent() {

}

void SimplePhysicsComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	if (packetType == REPLICA_CONSTRUCTION_PACKET) {
		packet->Write(flag1);
		if (flag1) {
			packet->Write(data1);
		}
	}

	packet->Write(flag2);
	if (flag2) {
		packet->Write(data2_1);
		packet->Write(data2_2);
		packet->Write(data2_3);
		packet->Write(data2_4);
		packet->Write(data2_5);
		packet->Write(data2_6);
	}

	packet->Write(flag3);
	if (flag3) {
		packet->Write(data3);
	}

	packet->Write(flag4);
	if (flag4) {
		packet->Write(this->position.posX);
		packet->Write(this->position.posY);
		packet->Write(this->position.posZ);
		packet->Write(this->rotation.rotX);
		packet->Write(this->rotation.rotY);
		packet->Write(this->rotation.rotZ);
		packet->Write(this->rotation.rotW);
	}
}

unsigned int SimplePhysicsComponent::getComponentID(){ return 3; }

bool SimplePhysicsComponent::getFlag1(){ return this->flag1; }
int SimplePhysicsComponent::getData1(){ return this->data1; }
bool SimplePhysicsComponent::getFlag2(){ return this->flag2; }
int SimplePhysicsComponent::getData2_1(){ return this->data2_1; }
int SimplePhysicsComponent::getData2_2(){ return this->data2_2; }
int SimplePhysicsComponent::getData2_3(){ return this->data2_3; }
int SimplePhysicsComponent::getData2_4(){ return this->data2_4; }
int SimplePhysicsComponent::getData2_5(){ return this->data2_5; }
int SimplePhysicsComponent::getData2_6(){ return this->data2_6; }
bool SimplePhysicsComponent::getFlag3(){ return this->flag3; }
int SimplePhysicsComponent::getData3(){ return this->data3; }
bool SimplePhysicsComponent::getFlag4(){ return this->flag4; }
COMPONENT3_POSITION SimplePhysicsComponent::getPosition(){ return this->position; }
COMPONENT3_ROTATION SimplePhysicsComponent::getRotation(){ return this->rotation; }

void SimplePhysicsComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void SimplePhysicsComponent::setData1(int data1){ this->data1 = data1; }
void SimplePhysicsComponent::setFlag2(bool flag2){ this->flag2 = flag2; }
void SimplePhysicsComponent::setData2_1(int data2_1){ this->data2_1 = data2_1; }
void SimplePhysicsComponent::setData2_2(int data2_2){ this->data2_2 = data2_2; }
void SimplePhysicsComponent::setData2_3(int data2_3){ this->data2_3 = data2_3; }
void SimplePhysicsComponent::setData2_4(int data2_4){ this->data2_4 = data2_4; }
void SimplePhysicsComponent::setData2_5(int data2_5){ this->data2_5 = data2_5; }
void SimplePhysicsComponent::setData2_6(int data2_6){ this->data2_6 = data2_6; }
void SimplePhysicsComponent::setFlag3(bool flag3){ this->flag3 = flag3; }
void SimplePhysicsComponent::setData3(int data3){ this->data3 = data3; }
void SimplePhysicsComponent::setFlag4(bool flag4){ this->flag4 = flag4; }
void SimplePhysicsComponent::setPosition(COMPONENT3_POSITION position){ this->position = position; }
void SimplePhysicsComponent::setRotation(COMPONENT3_ROTATION rotation){ this->rotation = rotation; }

#pragma endregion

#pragma region RigidBodyPhantomPhysicsComponent (Component 20)

RigidBodyPhantomPhysicsComponent::RigidBodyPhantomPhysicsComponent() {
	flag1 = true;
}

RigidBodyPhantomPhysicsComponent::~RigidBodyPhantomPhysicsComponent() {

}

void RigidBodyPhantomPhysicsComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE type) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(pos.x);
		packet->Write(pos.y);
		packet->Write(pos.z);
		packet->Write(rot.x);
		packet->Write(rot.y);
		packet->Write(rot.z);
		packet->Write(rot.w);
	}
}

unsigned int RigidBodyPhantomPhysicsComponent::getComponentID(){ return 20; }

bool RigidBodyPhantomPhysicsComponent::getFlag1(){ return flag1; }
COMPONENT20_POSITION RigidBodyPhantomPhysicsComponent::getPos(){ return pos; }
COMPONENT20_ROTATION RigidBodyPhantomPhysicsComponent::getRot(){ return rot; }

void RigidBodyPhantomPhysicsComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void RigidBodyPhantomPhysicsComponent::setPos(COMPONENT20_POSITION pos){ this->pos = pos; }
void RigidBodyPhantomPhysicsComponent::setRot(COMPONENT20_ROTATION rot){ this->rot = rot; }

#pragma endregion

#pragma region VehiclePhysicsComponent (Component 30)

VehiclePhysicsComponent::VehiclePhysicsComponent() {

}

VehiclePhysicsComponent::~VehiclePhysicsComponent() {

}

void VehiclePhysicsComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	if (packetType == REPLICA_PACKET_TYPE::REPLICA_CONSTRUCTION_PACKET) {
		packet->Write(data1);
		packet->Write(data2);
	}

	packet->Write(flag3);
	if (flag3) {
		packet->Write(data3);
	}
}

unsigned int VehiclePhysicsComponent::getComponentID(){ return 30; }

char VehiclePhysicsComponent::getData1(){ return data1; }
bool VehiclePhysicsComponent::getData2(){ return data2; }
bool VehiclePhysicsComponent::getFlag3(){ return flag3; }
bool VehiclePhysicsComponent::getData3(){ return data3; }

void VehiclePhysicsComponent::setData1(char data1){ this->data1 = data1; }
void VehiclePhysicsComponent::setData2(bool data2){ this->data2 = data2; }
void VehiclePhysicsComponent::setFlag3(bool flag3){ this->flag3 = flag3; }
void VehiclePhysicsComponent::setData3(bool data3){ this->data3 = data3; }

#pragma endregion

#pragma region PhantomPhysicsComponent (Component 40)

PhantomPhysicsComponent::PhantomPhysicsComponent() {
	this->flag1 = true;
	this->flag2 = true;
	this->flag3 = false;
	this->flag4_1 = false;
	this->flag4_2 = false;
}

PhantomPhysicsComponent::~PhantomPhysicsComponent() {

}

void PhantomPhysicsComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(pos.posX);
		packet->Write(pos.posY);
		packet->Write(pos.posZ);
		packet->Write(rot.rotX);
		packet->Write(rot.rotY);
		packet->Write(rot.rotZ);
		packet->Write(rot.rotW);
	}

	packet->Write(flag2);
	if (flag2) {
		packet->Write(flag3);
		if (flag3) {
			packet->Write(data1.d1);
			packet->Write(data1.d2);
			packet->Write(flag4_1);
			if (flag4_1) {
				packet->Write(data2.d1);
				packet->Write(data2.d2);
			}

			packet->Write(flag4_2);
			if (flag4_2) {
				packet->Write(data3.f1);
				packet->Write(data3.f2);
				packet->Write(data3.f3);
			}
		}
	}
}

unsigned int PhantomPhysicsComponent::getComponentID() {
	return 40;
}

bool PhantomPhysicsComponent::getFlag1(){ return this->flag1; }
COMPONENT40_POSITION PhantomPhysicsComponent::getPosition(){ return this->pos; }
COMPONENT40_ROTATION PhantomPhysicsComponent::getRotation(){ return this->rot; }
bool PhantomPhysicsComponent::getFlag2(){ return this->flag2; }
bool PhantomPhysicsComponent::getFlag3(){ return this->flag3; }
COMPONENT40_DATA1 PhantomPhysicsComponent::getData1(){ return this->data1; }
bool PhantomPhysicsComponent::getFlag4_1(){ return this->flag4_1; }
COMPONENT40_DATA2 PhantomPhysicsComponent::getData2(){ return this->data2; }
bool PhantomPhysicsComponent::getFlag4_2(){ return this->flag4_2; }
COMPONENT40_DATA3 PhantomPhysicsComponent::getData3(){ return this->data3; }

void PhantomPhysicsComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void PhantomPhysicsComponent::setPosition(COMPONENT40_POSITION position){ this->pos = position; }
void PhantomPhysicsComponent::setRotation(COMPONENT40_ROTATION rotation){ this->rot = rotation; }
void PhantomPhysicsComponent::setFlag2(bool flag2){ this->flag2 = flag2; }
void PhantomPhysicsComponent::setData1(COMPONENT40_DATA1 data1){ this->flag3 = true; this->data1 = data1; }
void PhantomPhysicsComponent::setData2(COMPONENT40_DATA2 data2){ this->flag4_1 = true; this->data2 = data2; }
void PhantomPhysicsComponent::setData3(COMPONENT40_DATA3 data3){ this->flag4_2 = true; this->data3 = data3; }

#pragma endregion

#pragma region DestructibleComponent (Component 7)

void DestructibleComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	//Index 22
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		packet->Write(flag1);
		if (flag1){
			unsigned long data1C = data1.size();
			packet->Write(data1C);
			for (unsigned long k = 0; k < data1C; k++){
				packet->Write(data1.at(k).d1);
				packet->Write(data1.at(k).d2f);
				if (data1.at(k).d2f){
					packet->Write(data1.at(k).d2);
				}
				packet->Write(data1.at(k).d3);
				packet->Write(data1.at(k).d4);
				packet->Write(data1.at(k).d5);
				packet->Write(data1.at(k).d6);
				packet->Write(data1.at(k).d7);
				packet->Write(data1.at(k).d8);
				packet->Write(data1.at(k).d9);
				packet->Write(data1.at(k).d10);
				bool t = data1.at(k).d11;
				packet->Write(t);
				packet->Write(data1.at(k).d12);
				if (t){
					packet->Write(data1.at(k).d13);
				}
				packet->Write(data1.at(k).d14);
			}
		}
		packet->Write(flag2);
		if (flag2){
			unsigned long data2C = data2.size();
			packet->Write(data2C);
			for (unsigned long k = 0; k < data2C; k++){
				packet->Write(data2.at(k).d1);
				packet->Write(data2.at(k).d2f);
				if (data2.at(k).d2f){
					packet->Write(data2.at(k).d2);
				}
				packet->Write(data2.at(k).d3);
				packet->Write(data2.at(k).d4);
				packet->Write(data2.at(k).d5);
				packet->Write(data2.at(k).d6);
				packet->Write(data2.at(k).d7);
				packet->Write(data2.at(k).d8);
				packet->Write(data2.at(k).d9);
				packet->Write(data2.at(k).d10);
				bool t = data2.at(k).d11;
				packet->Write(t);
				packet->Write(data2.at(k).d12);
				if (t){
					packet->Write(data2.at(k).d13);
				}
				packet->Write(data2.at(k).d14);
			}
		}
	}
	//Index 23
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		packet->Write(flag3);
		if (flag3){
			packet->Write(data3.d1);
			packet->Write(data3.d2);
			packet->Write(data3.d3);
			packet->Write(data3.d4);
			packet->Write(data3.d5);
			packet->Write(data3.d6);
			packet->Write(data3.d7);
			packet->Write(data3.d8);
			packet->Write(data3.d9);
			//flag3 = false;
		}
	}

	packet->Write(flag4);
	if (flag4){
		packet->Write(data4.health);
		packet->Write(data4.maxHealthN);
		packet->Write(data4.armor);
		packet->Write(data4.maxArmorN);
		packet->Write(data4.imagination);
		packet->Write(data4.maxImaginationN);
		packet->Write(data4.d7);
		packet->Write(data4.d8);
		packet->Write(data4.d9);
		packet->Write(data4.d10);
		packet->Write(data4.maxHealth);
		packet->Write(data4.maxArmor);
		packet->Write(data4.maxImagination);
		unsigned long t = data4_1.size();
		packet->Write(t);
		for (unsigned long k = 0; k < t; k++){
			packet->Write(data4_1.at(k));
		}
		packet->Write(trigger);
		if (packetType == REPLICA_CONSTRUCTION_PACKET){
			packet->Write(data4_2);
			packet->Write(data4_3);
			if (trigger){
				packet->Write(data4_4_1);
				packet->Write(flag4_4_2);
				if (flag4_4_2){
					packet->Write(data4_4_2);
				}
			}
		}
		//flag4 = false;
	}

	packet->Write(flag5);
	if (flag5){
		packet->Write(data5);
		//flag5 = false;
	}
}

unsigned int DestructibleComponent::getComponentID(){ return 7; }

DestructibleComponent::DestructibleComponent(){
	this->flag1 = false;
	this->flag2 = false;
	this->flag3 = false;
	this->flag4 = false;
	this->data4_2 = false;
	this->data4_3 = false;
	this->flag4_4_2 = false;
	this->trigger = false;
	this->flag5 = false;
}

DestructibleComponent::~DestructibleComponent(){

}

std::vector<COMPONENT7_DATA1> *DestructibleComponent::getData1Ref(){ this->flag1 = true; return &this->data1; } // Replace by a setUpdateData1(); 
std::vector<COMPONENT7_DATA2> *DestructibleComponent::getData2Ref(){ this->flag2 = true; return &this->data2; } // Replace by a setUpdateData2();
COMPONENT7_DATA3 DestructibleComponent::getData3(){ return this->data3; }
COMPONENT7_DATA4 DestructibleComponent::getData4(){ return this->data4; }
std::vector<unsigned long> *DestructibleComponent::getData4_1Ref(){ this->flag4 = true; return &this->data4_1; }
bool DestructibleComponent::getTrigger(){ return this->trigger; }
bool DestructibleComponent::getData4_2(){ return this->data4_2; }
bool DestructibleComponent::getData4_3(){ return this->data4_3; }
bool DestructibleComponent::getData4_4_1(){ return this->data4_4_1; }
unsigned long DestructibleComponent::getData4_4_2(){ return this->data4_4_2; }
bool DestructibleComponent::getData5(){ return this->data5; }
void DestructibleComponent::setData3(COMPONENT7_DATA3 data3){ this->data3 = data3; this->flag3 = true; }
void DestructibleComponent::setData4(COMPONENT7_DATA4 data4){ this->data4 = data4; this->flag4 = true; }
void DestructibleComponent::setTrigger(bool trigger){ this->trigger = trigger; this->flag4 = true; }
void DestructibleComponent::setData4_2(bool data4_2){ this->data4_2 = data4_2; this->flag4 = true; }
void DestructibleComponent::setData4_3(bool data4_3){ this->data4_3 = data4_3; this->flag4 = true; }
void DestructibleComponent::setData4_4_1(bool data4_4_1){ this->data4_4_1 = data4_4_1; this->flag4 = true; }
void DestructibleComponent::setData4_4_2(unsigned long data4_4_2){ this->data4_4_2 = data4_4_2; this->flag4 = true; this->flag4_4_2 = true; }
void DestructibleComponent::setData5(bool data5){ this->data5 = data5; this->flag5 = true; }

#pragma endregion

#pragma region SwitchComponent (Component 49)

SwitchComponent::SwitchComponent() {

}

SwitchComponent::~SwitchComponent() {

}

void SwitchComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(switchState);
}

unsigned int SwitchComponent::getComponentID() {
	return 49;
}

bool SwitchComponent::getSwitchState(){ return switchState; }
void SwitchComponent::setSwitchState(bool switchState){ this->switchState = switchState; }

#pragma endregion

#pragma region PetComponent (Component 26)

PetComponent::PetComponent() {
	this->flag1 = true;
	this->flag3 = false;
	this->flag4 = false;
	this->flag5 = true;
}

PetComponent::~PetComponent() {

}

void PetComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(data1);
		packet->Write(data2);
		packet->Write(flag3);
		if (flag3) {
			packet->Write(data3);
		}
		packet->Write(flag4);
		if (flag4) {
			packet->Write(data4);
		}

		packet->Write(flag5);
		if (flag5) {
			packet->Write(data5_1);
			packet->Write(length1);
			packet->Write(data5_1_1);
			packet->Write(length2);
			packet->Write(data5_1_2);
		}
	}
}

unsigned int PetComponent::getComponentID() {
	return 26;
}

bool PetComponent::getFlag1(){ return flag1; }
long PetComponent::getData1(){ return data1; }
long PetComponent::getData2(){ return data2; }
bool PetComponent::getFlag3(){ return flag3; }
long long PetComponent::getData3(){ return data3; }
bool PetComponent::getFlag4(){ return flag4; }
long long PetComponent::getData4(){ return data4; }
bool PetComponent::getFlag5(){ return flag5; }
long PetComponent::getData5_1(){ return data5_1; }
char PetComponent::getLength1(){ return length1; }
short PetComponent::getData5_1_1(){ return data5_1_1; }
char PetComponent::getLength2(){ return length2; }
short PetComponent::getData5_1_2(){ return data5_1_2; }

void PetComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void PetComponent::setData1(long data1){ this->data1 = data1; }
void PetComponent::setData2(long data2){ this->data2 = data2; }
void PetComponent::setData3(long long data3){ this->flag3 = true; this->data3 = data3; }
void PetComponent::setData4(long long data4){ this->flag4 = true; this->data4 = data4; }
void PetComponent::setData5_1(long data5_1){ this->flag5 = true; this->data5_1 = data5_1; }
void PetComponent::setLength1(char length1){ this->length1 = length1; }
void PetComponent::setData5_1_1(short data5_1_1){ this->data5_1_1 = data5_1_1; }
void PetComponent::setLength2(char length2){ this->length2 = length2; }
void PetComponent::setData5_1_2(short data5_1_2){ this->data5_1_2 = data5_1_2; }

#pragma endregion

#pragma region CharacterComponent (Component 4)

CharacterComponent::CharacterComponent(){
	this->flag1 = false;
	this->flag1_1 = false;
	this->hasLevel = false;
	this->flag3 = false;
	this->flag4 = false;
	this->flag5 = false;
	this->flag6 = false;
	this->flag7 = false;
	this->flag8a = false;
	this->flag8b = false;
	this->flag9 = false;
	this->flag10 = false;
	this->flag11 = false;
}

CharacterComponent::~CharacterComponent(){

}

void CharacterComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	packet->Write(flag1);
	if (flag1){
		packet->Write(flag1_1);
		if (flag1_1){
			packet->Write(data1_1);
		}
		packet->Write(data1_2);
	}
	packet->Write(hasLevel);
	if (hasLevel){
		packet->Write(level);
	}
	packet->Write(flag3);
	if (flag3){
		packet->Write(data3.d1);
		packet->Write(data3.d2);
	}
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		packet->Write(flag4);
		if (flag4) packet->Write(data4);
		packet->Write(flag5);
		if (flag5) packet->Write(data5);
		packet->Write(flag6);
		if (flag6) packet->Write(data6);
		packet->Write(flag7);
		if (flag7) packet->Write(data7);
		packet->Write(this->style);
		packet->Write(info.accountID);
		packet->Write(info.llog);
		packet->Write(info.unknown);
		packet->Write(info.legoScore);
		packet->Write(info.isFreeToPlay);
		
		packet->Write(stats);
		packet->Write(flag8a);
		packet->Write(flag8b);
		if (flag8b && !flag8a){
			unsigned short size = data8.size();
			packet->Write(size);
			for (int i = 0; i < size; i++){
				packet->Write((wchar_t) data8.at(i));
			}
		}
	}
	flag9 = true;
	packet->Write(flag9);
	if (flag9){
		packet->Write(data9.d1);
		packet->Write(data9.d2);
		packet->Write(data9.d3);
		packet->Write(data9.d4);
		packet->Write(data9.d5);
	}

	flag10 = true;
	packet->Write(flag10);
	if (flag10){
		packet->Write(data10);
	}

	flag11 = true;
	packet->Write(flag11);
	if (flag11){
		packet->Write(data11.d1);
		packet->Write(data11.d2);
		packet->Write(data11.d3);
		packet->Write(data11.d4);
	}
}

unsigned int CharacterComponent::getComponentID(){ return 4; }

void CharacterComponent::setData1_1(unsigned long long d1_1){ this->data1_1 = d1_1; this->flag1 = true; this->flag1_1 = true; }
void CharacterComponent::setData1_2(unsigned char d1_2){ data1_2 = d1_2; flag1 = true; }
void CharacterComponent::setLevel(unsigned long lvl){ level = lvl, hasLevel = true; }
void CharacterComponent::setData3(COMPONENT4_DATA3 d3){ data3 = d3; flag3 = true; }
void CharacterComponent::setData4(unsigned long long d4){ data4 = d4; flag4 = true; }
void CharacterComponent::setData5(unsigned long long d5){ data5 = d5; flag5 = true; }
void CharacterComponent::setData6(unsigned long long d6){ data6 = d6; flag6 = true; }
void CharacterComponent::setData7(unsigned long long d7){ data7 = d7; flag7 = true; }
void CharacterComponent::setStyle(PLAYER_STYLE style){ this->style = style; }
void CharacterComponent::setInfo(PLAYER_INFO info){ this->info = info; }
void CharacterComponent::setStats(PLAYER_STATS stats){ this->stats = stats; }
void CharacterComponent::setData8(std::wstring d8){ this->data8 = d8; flag8b = true; }
void CharacterComponent::setData9(COMPONENT4_DATA9 d9){ this->data9 = d9; flag9 = true; }
void CharacterComponent::setData10(unsigned long d10){ data10 = d10; flag10 = true; }
void CharacterComponent::setData11(COMPONENT4_DATA11 d11){ data11 = d11; flag11 = true; }

unsigned long long CharacterComponent::getData1_1(){ return data1_1; }
unsigned char CharacterComponent::getData1_2(){ return data1_2; }
unsigned long CharacterComponent::getLevel(){ return level; }
COMPONENT4_DATA3 CharacterComponent::getData3(){ return data3; }
unsigned long long CharacterComponent::getData4(){ return data4; }
unsigned long long CharacterComponent::getData5(){ return data5; }
unsigned long long CharacterComponent::getData6(){ return data6; }
unsigned long long CharacterComponent::getData7(){ return data7; }
PLAYER_STYLE CharacterComponent::getStyle(){ return style; }
PLAYER_INFO CharacterComponent::getInfo(){ return info; }
PLAYER_STATS CharacterComponent::getStats(){ return stats; }
std::wstring CharacterComponent::getData8(){ return data8; }
COMPONENT4_DATA9 CharacterComponent::getData9(){ return data9; }
unsigned long CharacterComponent::getData10(){ return data10; }
COMPONENT4_DATA11 CharacterComponent::getData11(){ return data11; }

#pragma endregion

#pragma region InventoryComponent (Component 17)

InventoryComponent::InventoryComponent(){

}
InventoryComponent::~InventoryComponent(){

}
void InventoryComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	unsigned long size = this->equipment.size();
	bool hasEquipment = true; //(size > 0);
	packet->Write(hasEquipment);
	if (hasEquipment){
		packet->Write(size);
		//std::cout << "[WORLD] EQUIPMENT COUNT: " << size << std::endl;
		for (unsigned long k = 0; k < size; k++){
			COMPONENT17_EQUIPMENT eq = equipment.at(k);
			packet->Write(eq.objid);
			packet->Write(eq.lot);
			packet->Write(eq.d3 > 0);
			if (eq.d3 > 0){
				packet->Write(eq.d3);
			}
			packet->Write(eq.d4 > 0);
			if (eq.d4 > 0){
				packet->Write(eq.d4);
			}
			packet->Write(eq.slot > 0);
			if (eq.slot > 0){
				packet->Write(eq.slot);
			}
			packet->Write(eq.d6 > 0);
			if (eq.d6 > 0){
				packet->Write(eq.d6);
			}
			unsigned long s = eq.d7.size();
			bool s0 = (s > 0);
			packet->Write(s0);
			if (s0){
				packet->Write(s);
				for (unsigned long i = 0; i < s; i++){
					packet->Write(eq.d7.at(i));
				}
			}
			packet->Write(eq.d8);
		}
	}
	bool f2 = (this->data2 > 0);
	packet->Write(f2);
	if (f2){
		packet->Write(this->data2); //may be a count
	}
}

unsigned int InventoryComponent::getComponentID(){ return 17; }

std::vector<COMPONENT17_EQUIPMENT> *InventoryComponent::getEquipment(){
	return &this->equipment;
}

unsigned long InventoryComponent::getData2(){
	return this->data2;
}

void InventoryComponent::setData2(unsigned long d2){
	this->data2 = d2;
}

long InventoryComponent::equipItem(long long objid, unsigned short slot){
	long lot = ObjectsTable::getTemplateOfItem(objid);
	if (lot > -1){
		COMPONENT17_EQUIPMENT eqi;
		eqi.objid = objid;
		eqi.lot = lot;
		eqi.slot = slot;
		this->equipment.push_back(eqi);
	}
	return lot;
}

bool InventoryComponent::unequipItem(long long objid){
	std::vector<COMPONENT17_EQUIPMENT>::iterator it = std::find_if(this->equipment.begin(), this->equipment.end(), [&](const COMPONENT17_EQUIPMENT & o){
		return o.objid == objid;
	});
	if (it == this->equipment.end()){
		return false;
	}
	else{
		std::swap(*it, this->equipment.back());
		this->equipment.pop_back();
	}
	return true;
}

#pragma endregion

#pragma region ScriptComponent (Component 5)

ScriptComponent::ScriptComponent() {
	this->flag1 = false;
}

ScriptComponent::~ScriptComponent() {

}

void ScriptComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
}

unsigned int ScriptComponent::getComponentID(){ return 5; }

bool ScriptComponent::getFlag1(){ return this->flag1; }
void ScriptComponent::setFlag1(bool flag1){ this->flag1 = flag1; }

#pragma endregion

#pragma region SkillComponent (Component 9)

SkillComponent::SkillComponent(){

}

SkillComponent::~SkillComponent(){

}

void SkillComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_CONSTRUCTION_PACKET) packet->Write(false);
}

unsigned int SkillComponent::getComponentID(){ return 9; }

#pragma endregion

#pragma region BaseCombatAIComponent (Component 60)

BaseCombatAIComponent::BaseCombatAIComponent() {
	flag1 = true;
}

BaseCombatAIComponent::~BaseCombatAIComponent() {

}

void BaseCombatAIComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(data1);
		packet->Write(data2);
	}
}

unsigned int BaseCombatAIComponent::getComponentID() {
	return 60;
}

bool BaseCombatAIComponent::getFlag1(){ return flag1; }
long BaseCombatAIComponent::getData1(){ return data1; }
long long BaseCombatAIComponent::getData2(){ return data2; }

void BaseCombatAIComponent::writeFlag1(bool flag1){ this->flag1 = flag1; }
void BaseCombatAIComponent::writeData1(long data1){ this->data1 = data1; }
void BaseCombatAIComponent::writeData2(long long data2){ this->data2 = data2; }

#pragma endregion

#pragma region VendorComponent (Component 16)

VendorComponent::VendorComponent() {
	this->flag1 = true;
	this->flag1_1 = true;
	this->flag1_2 = false;
}

VendorComponent::~VendorComponent() {

}

void VendorComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(flag1_1);
		packet->Write(flag1_2);
	}
}

unsigned int VendorComponent::getComponentID() {
	return 16;
}

bool VendorComponent::getFlag1(){ return this->flag1; }
bool VendorComponent::getFlag1_1(){ return this->flag1_1; }
bool VendorComponent::getFlag1_2(){ return this->flag1_2; }

void VendorComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void VendorComponent::setFlag1_1(bool flag1_1){ this->flag1_1 = flag1_1; }
void VendorComponent::setFlag1_2(bool flag1_2){ this->flag1_2 = flag1_2; }

#pragma endregion

#pragma region BouncerComponent (Component 6)

BouncerComponent::BouncerComponent() {
	flag1 = true;
}

BouncerComponent::~BouncerComponent() {

}

void BouncerComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(data1);
	}
}

unsigned int getComponentID(){ return 6; }

bool BouncerComponent::getFlag1(){ return flag1; }
bool BouncerComponent::getData1(){ return data1; }

void BouncerComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void BouncerComponent::setData1(bool data1){ this->data1 = data1; }

#pragma endregion

#pragma region ScriptedActivityComponent (Component 39)

ScriptedActivityComponent::ScriptedActivityComponent() {
	flag1 = true;
	data1 = 0;
	data2 = 0;
}

ScriptedActivityComponent::~ScriptedActivityComponent() {

}

void ScriptedActivityComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(data1);
		packet->Write(data2);
		packet->Write(data2_1);
		packet->Write(data2_2);
		packet->Write(data2_3);
		packet->Write(data2_4);
		packet->Write(data2_5);
		packet->Write(data2_6);
		packet->Write(data2_7);
		packet->Write(data2_8);
		packet->Write(data2_9);
		packet->Write(data2_10);
	}
}

unsigned int ScriptedActivityComponent::getComponentID(){ return 39; }

bool ScriptedActivityComponent::getFlag1(){ return flag1; }
long ScriptedActivityComponent::getData1(){ return data1; }
long long ScriptedActivityComponent::getData2(){ return data2; }
float ScriptedActivityComponent::getData2_1(){ return data2_1; }
float ScriptedActivityComponent::getData2_2(){ return data2_2; }
float ScriptedActivityComponent::getData2_3(){ return data2_3; }
float ScriptedActivityComponent::getData2_4(){ return data2_4; }
float ScriptedActivityComponent::getData2_5(){ return data2_5; }
float ScriptedActivityComponent::getData2_6(){ return data2_6; }
float ScriptedActivityComponent::getData2_7(){ return data2_7; }
float ScriptedActivityComponent::getData2_8(){ return data2_8; }
float ScriptedActivityComponent::getData2_9(){ return data2_9; }
float ScriptedActivityComponent::getData2_10(){ return data2_10; }

void ScriptedActivityComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void ScriptedActivityComponent::setData1(long data1){ this->data1 = data1; }
void ScriptedActivityComponent::setData2(long long data2){ this->data2 = data2; }
void ScriptedActivityComponent::setData2_1(float data2_1){ this->data2_1 = data2_1; }
void ScriptedActivityComponent::setData2_2(float data2_2){ this->data2_2 = data2_2; }
void ScriptedActivityComponent::setData2_3(float data2_3){ this->data2_3 = data2_3; }
void ScriptedActivityComponent::setData2_4(float data2_4){ this->data2_4 = data2_4; }
void ScriptedActivityComponent::setData2_5(float data2_5){ this->data2_5 = data2_5; }
void ScriptedActivityComponent::setData2_6(float data2_6){ this->data2_6 = data2_6; }
void ScriptedActivityComponent::setData2_7(float data2_7){ this->data2_7 = data2_7; }
void ScriptedActivityComponent::setData2_8(float data2_8){ this->data2_8 = data2_8; }
void ScriptedActivityComponent::setData2_9(float data2_9){ this->data2_9 = data2_9; }
void ScriptedActivityComponent::setData2_10(float data2_10){ this->data2_10 = data2_10; }

#pragma endregion

#pragma region RacingControlComponent (Component 71)

RacingControlComponent::RacingControlComponent() {
	flag1 = true;
	shortData = 0; // Just a placeholder for now
}

RacingControlComponent::~RacingControlComponent() {

}

void RacingControlComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) {
	packet->Write(flag1);
	if (flag1) {
		packet->Write(shortData);
	}

	packet->Write(flag2);
	if (flag2) {
		while (true) {
			packet->Write(data1.not_break);
			if (!data1.not_break){ break; }

			packet->Write(data1.playerObjID);
			packet->Write(data1.racecarObjID);
			packet->Write(data1.data1);
			packet->Write(data1.data2);
		}
	}

	packet->Write(flag3);
	if (flag3) {
		while (true) {
			packet->Write(data2.not_break);
			if (!data2.not_break){ break; }

			packet->Write(data2.playerObjID);
			packet->Write(data2.data1);
		}
	}

	packet->Write(flag4);
	if (flag4) {
		packet->Write(data3.remainingLaps);
		packet->Write(data3.length);
		packet->Write(data3.pathName);
	}

	packet->Write(flag5);
	if (flag5) {
		packet->Write(data4.flag1);
		if (data4.flag1) {
			packet->Write(data4.data1);
			packet->Write(data4.data2);
			packet->Write(data4.data3);
		}
	}
}

unsigned int RacingControlComponent::getComponentID(){ return 71; }

bool RacingControlComponent::getFlag1(){ return flag1; }
short RacingControlComponent::getShortData(){ return shortData; }
bool RacingControlComponent::getFlag2(){ return flag2; }
COMPONENT71_DATA1 RacingControlComponent::getData1(){ return data1; }
bool RacingControlComponent::getFlag3(){ return flag3; }
COMPONENT71_DATA2 RacingControlComponent::getData2(){ return data2; }
bool RacingControlComponent::getFlag4(){ return flag4; }
COMPONENT71_DATA3 RacingControlComponent::getData3(){ return data3; }
bool RacingControlComponent::getFlag5(){ return flag5; }
COMPONENT71_DATA4 RacingControlComponent::getData4(){ return data4; }

void RacingControlComponent::setFlag1(bool flag1){ this->flag1 = flag1; }
void RacingControlComponent::setShortData(short shortData){ this->shortData = shortData; }
void RacingControlComponent::setData1(COMPONENT71_DATA1 data1){ this->flag2 = true; this->data1 = data1; }
void RacingControlComponent::setData2(COMPONENT71_DATA2 data2){ this->flag3 = true; this->data2 = data2; }
void RacingControlComponent::setData3(COMPONENT71_DATA3 data3){ this->flag4 = true; this->data3 = data3; }
void RacingControlComponent::setData4(COMPONENT71_DATA4 data4){ this->flag5 = true; this->data4 = data4; }

#pragma endregion

#pragma region RenderComponent (Component 2)

RenderComponent::RenderComponent(){

}

RenderComponent::~RenderComponent(){

}

void RenderComponent::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_CONSTRUCTION_PACKET) packet->Write(0UL);
}

unsigned int RenderComponent::getComponentID(){ return 2; }

#pragma endregion

#pragma region Index36Component (Component 107)

Index36Component::Index36Component(){

}

Index36Component::~Index36Component(){

}

void Index36Component::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	packet->Write(true);
	packet->Write(0ULL);
}

unsigned int Index36Component::getComponentID(){ return 107; }

#pragma endregion