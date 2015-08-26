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

#pragma region Component1
void Component1::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
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

unsigned int Component1::getComponentID(){ return 1; }

Component1::Component1(){
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

Component1::~Component1(){

}

void Component1::setData1(COMPONENT1_DATA1 data1){ this->data1 = data1; this->flag1 = true; }
COMPONENT1_DATA1 Component1::getData1(){ return this->data1; }
void Component1::setData2(COMPONENT1_DATA2 data2){ this->data2 = data2; this->flag2 = true; }
COMPONENT1_DATA2 Component1::getData2(){ return this->data2; }
void Component1::setData3(COMPONENT1_DATA3 data3){ this->data3 = data3; this->flag3 = true; }
COMPONENT1_DATA3 Component1::getData3(){ return this->data3; }
void Component1::setData4(COMPONENT1_DATA4 data4){ this->data4 = data4; this->flag4 = true; }
COMPONENT1_DATA4 Component1::getData4(){ return this->data4; }
void Component1::setData5(COMPONENT1_DATA5 data5){ this->data5 = data5; this->flag5_1 = true; }
COMPONENT1_DATA5 Component1::getData5(){ return this->data5; }
void Component1::setHasData5(bool flag){ flag5 = flag; }
bool Component1::getHasData5(){ return flag5; }
void Component1::setPosition(COMPONENT1_POSITION pos){ this->data6.position = pos; flag6 = true; }
COMPONENT1_POSITION Component1::getPosition(){ return this->data6.position; }
void Component1::setRotation(COMPONENT1_ROTATION rot){ this->data6.rotation = rot; flag6 = true; }
COMPONENT1_ROTATION Component1::getRotation(){ return this->data6.rotation; }
void Component1::setOnGround(bool onGround){ this->data6.onGround = onGround; flag6 = true; }
bool Component1::getOnGround(){ return this->data6.onGround; }
void Component1::setData6_d4(bool d){ this->data6.d4 = d; flag6 = true; }
bool Component1::getData6_d4(){ return this->data6.d4; }
void Component1::setVelocity(COMPONENT1_VELOCITY vel){ this->data6_1 = vel; flag6 = true;  flag6_1 = true; }
COMPONENT1_VELOCITY Component1::getVelocity(){ return this->data6_1; }
void Component1::setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR avel){ this->data6_2 = avel; flag6 = true, flag6_2 = true; }
COMPONENT1_VELOCITY_ANGULAR Component1::getAngularVelocity(){ return this->data6_2; }
void Component1::setData6_3(COMPONENT1_DATA6_3 data6_3){ this->data6_3 = data6_3; flag6 = true; flag6_3 = true; }
COMPONENT1_DATA6_3 Component1::getData6_3(){ return this->data6_3; }
void Component1::setData6_3_1(COMPONENT1_DATA6_3_1 data6_3_1){ this->data6_3_1 = data6_3_1; flag6 = true; flag6_3 = true; flag6_3_1 = true; }
COMPONENT1_DATA6_3_1 Component1::getData6_3_1(){ return this->data6_3_1; }
#pragma endregion

#pragma region Component4

Component4::Component4(){
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

Component4::~Component4(){

}

void Component4::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
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
			ushort size = data8.size();
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

unsigned int Component4::getComponentID(){ return 4; }

void Component4::setData1_1(ulonglong d1_1){ this->data1_1 = d1_1; this->flag1 = true; this->flag1_1 = true; }
void Component4::setData1_2(uchar d1_2){ data1_2 = d1_2; flag1 = true; }
void Component4::setLevel(ulong lvl){ level = lvl, hasLevel = true; }
void Component4::setData3(COMPONENT4_DATA3 d3){ data3 = d3; flag3 = true; }
void Component4::setData4(ulonglong d4){ data4 = d4; flag4 = true; }
void Component4::setData5(ulonglong d5){ data5 = d5; flag5 = true; }
void Component4::setData6(ulonglong d6){ data6 = d6; flag6 = true; }
void Component4::setData7(ulonglong d7){ data7 = d7; flag7 = true; }
void Component4::setStyle(PLAYER_STYLE style){ this->style = style; }
void Component4::setInfo(PLAYER_INFO info){ this->info = info; }
void Component4::setStats(PLAYER_STATS stats){ this->stats = stats; }
void Component4::setData8(std::wstring d8){ this->data8 = d8; flag8b = true; }
void Component4::setData9(COMPONENT4_DATA9 d9){ this->data9 = d9; flag9 = true; }
void Component4::setData10(ulong d10){ data10 = d10; flag10 = true; }
void Component4::setData11(COMPONENT4_DATA11 d11){ data11 = d11; flag11 = true; }

ulonglong Component4::getData1_1(){ return data1_1; }
uchar Component4::getData1_2(){ return data1_2; }
ulong Component4::getLevel(){ return level; }
COMPONENT4_DATA3 Component4::getData3(){ return data3; }
ulonglong Component4::getData4(){ return data4; }
ulonglong Component4::getData5(){ return data5; }
ulonglong Component4::getData6(){ return data6; }
ulonglong Component4::getData7(){ return data7; }
PLAYER_STYLE Component4::getStyle(){ return style; }
PLAYER_INFO Component4::getInfo(){ return info; }
PLAYER_STATS Component4::getStats(){ return stats; }
std::wstring Component4::getData8(){ return data8;  }
COMPONENT4_DATA9 Component4::getData9(){ return data9; }
ulong Component4::getData10(){ return data10; }
COMPONENT4_DATA11 Component4::getData11(){ return data11; }

#pragma endregion

#pragma region Component7
void Component7::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	//Index 22
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		packet->Write(flag1);
		if (flag1){
			ulong data1C = data1.size();
			packet->Write(data1C);
			for (ulong k = 0; k < data1C; k++){
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
			ulong data2C = data2.size();
			packet->Write(data2C);
			for (ulong k = 0; k < data2C; k++){
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
		ulong t = data4_1.size();
		packet->Write(t);
		for (ulong k = 0; k < t; k++){
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

unsigned int Component7::getComponentID(){ return 7; }

Component7::Component7(){
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

Component7::~Component7(){

}

std::vector<COMPONENT7_DATA1> *Component7::getData1Ref(){ this->flag1 = true; return &this->data1; } // Replace by a setUpdateData1(); 
std::vector<COMPONENT7_DATA2> *Component7::getData2Ref(){ this->flag2 = true; return &this->data2; } // Replace by a setUpdateData2();
COMPONENT7_DATA3 Component7::getData3(){ return this->data3; }
COMPONENT7_DATA4 Component7::getData4(){ return this->data4; }
std::vector<ulong> *Component7::getData4_1Ref(){ this->flag4 = true; return &this->data4_1; }
bool Component7::getTrigger(){ return this->trigger; }
bool Component7::getData4_2(){ return this->data4_2; }
bool Component7::getData4_3(){ return this->data4_3; }
bool Component7::getData4_4_1(){ return this->data4_4_1; }
ulong Component7::getData4_4_2(){ return this->data4_4_2; }
bool Component7::getData5(){ return this->data5; }
void Component7::setData3(COMPONENT7_DATA3 data3){ this->data3 = data3; this->flag3 = true;  }
void Component7::setData4(COMPONENT7_DATA4 data4){ this->data4 = data4; this->flag4 = true;  }
void Component7::setTrigger(bool trigger){ this->trigger = trigger; this->flag4 = true; }
void Component7::setData4_2(bool data4_2){ this->data4_2 = data4_2; this->flag4 = true; }
void Component7::setData4_3(bool data4_3){ this->data4_3 = data4_3; this->flag4 = true;}
void Component7::setData4_4_1(bool data4_4_1){ this->data4_4_1 = data4_4_1; this->flag4 = true; }
void Component7::setData4_4_2(ulong data4_4_2){ this->data4_4_2 = data4_4_2; this->flag4 = true; this->flag4_4_2 = true; }
void Component7::setData5(bool data5){ this->data5 = data5; this->flag5 = true; }
#pragma endregion

#pragma region Component17

Component17::Component17(){

}
Component17::~Component17(){

}
void Component17::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	ulong size = this->equipment.size();
	bool hasEquipment = true; //(size > 0);
	packet->Write(hasEquipment);
	if (hasEquipment){
		packet->Write(size);
		//std::cout << "[WORLD] EQUIPMENT COUNT: " << size << std::endl;
		for (ulong k = 0; k < size; k++){
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
			ulong s = eq.d7.size();
			bool s0 = (s > 0);
			packet->Write(s0);
			if (s0){
				packet->Write(s);
				for (ulong i = 0; i < s; i++){
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

unsigned int Component17::getComponentID(){ return 17; }

std::vector<COMPONENT17_EQUIPMENT> *Component17::getEquipment(){
	return &this->equipment;
}

ulong Component17::getData2(){
	return this->data2;
}

void Component17::setData2(ulong d2){
	this->data2 = d2;
}

long Component17::equipItem(long long objid, ushort slot){
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

bool Component17::unequipItem(long long objid){
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

#pragma region Component9

Component9::Component9(){

}

Component9::~Component9(){

}

void Component9::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_CONSTRUCTION_PACKET) packet->Write(false);
}

unsigned int Component9::getComponentID(){ return 9; }

#pragma endregion

#pragma region Component2

Component2::Component2(){

}

Component2::~Component2(){

}

void Component2::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_CONSTRUCTION_PACKET) packet->Write(0UL);
}

unsigned int Component2::getComponentID(){ return 2; }

#pragma endregion

#pragma region Component107

Component107::Component107(){

}

Component107::~Component107(){

}

void Component107::writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType){
	packet->Write(true);
	packet->Write(0ULL);
}

unsigned int Component107::getComponentID(){ return 107; }

#pragma endregion