#pragma once

#include "RakNet\BitStream.h"
#include "ReplicaDefinitions.h"

#include <vector>

//Abstract Base Class -> Interface
class ReplicaComponent{
public:
	virtual void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) = 0;
	virtual unsigned int getComponentID() = 0;
};

#pragma region Componet108 (Name Unknown)

class Component108 : public ReplicaComponent {
private:
	bool flag1;
	bool flag2;
	long long driverObjID;
	bool flag3;
	long data3_1;
	bool data4;
public:
	Component108();
	~Component108();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	bool getFlag2();
	long long getDriverObjID();
	bool getFlag3();
	long getData3_1();
	bool getData4();

	void setFlag1(bool flag1);
	void setFlag2(bool flag2);
	void setDriverObjID(long long driverObjID);
	void setFlag3(bool flag3);
	void setData3_1(long data3_1);
	void setData4(bool data4);
};

#pragma endregion

// No data has been discovered for this component...
#pragma region ModuleAssemblyComponent (Component Unknown)

#pragma endregion

#pragma region ControllablePhysicsComponent (Component 1)
//Component 1 Structs
#pragma pack(push, 1)
struct COMPONENT1_DATA1{
	unsigned long d1 = 0;
	bool d2 = false;
	bool d3 = false;
};
struct COMPONENT1_DATA2{
	unsigned long d1 = 0;
	unsigned long d2 = 0;
	unsigned long d3 = 0;
	unsigned long d4 = 0;
	unsigned long d5 = 0;
	unsigned long d6 = 0;
	unsigned long d7 = 0;
};
struct COMPONENT1_DATA3{
	unsigned long d1 = 0;
	unsigned long d2 = 0;
};
struct COMPONENT1_DATA4{
	unsigned long d1 = 0;
	bool d2 = false;
};
struct COMPONENT1_DATA5{
	unsigned long d1 = 0;
	bool d2 = false;
};
struct COMPONENT1_POSITION{
	float x;
	float y;
	float z;
	COMPONENT1_POSITION(){
		this->x = 0; this->y = 0; this->z = 0;
	}
	COMPONENT1_POSITION(float x, float y, float z){
		this->x = x; this->y = y; this->z = z;
	}
	bool isNull(){
		return (this->x == 0 && this->y == 0 && this->z == 0);
	}
};
struct COMPONENT1_ROTATION{
	float x;
	float y;
	float z;
	float w;
	COMPONENT1_ROTATION(){
		this->x = 0; this->y = 0; this->z = 0; this->w = 0;
	}
	COMPONENT1_ROTATION(float x, float y, float z, float w){
		this->x = x; this->y = y; this->z = z; this->w = w;
	}
};
struct COMPONENT1_DATA6{
	COMPONENT1_POSITION position;
	COMPONENT1_ROTATION rotation;
	bool onGround = false;
	bool d4 = false;
};
struct COMPONENT1_VELOCITY{
	float x;
	float y;
	float z;
	COMPONENT1_VELOCITY(){
		this->x = 0; this->y = 0; this->z = 0;
	}
	COMPONENT1_VELOCITY(float x, float y, float z){
		this->x = x; this->y = y; this->z = z;
	}
};
struct COMPONENT1_VELOCITY_ANGULAR{
	float x;
	float y;
	float z;
	COMPONENT1_VELOCITY_ANGULAR(){
		this->x = 0; this->y = 0; this->z = 0;
	}
	COMPONENT1_VELOCITY_ANGULAR(float x, float y, float z){
		this->x = x; this->y = y; this->z = z;
	}
};
struct COMPONENT1_DATA6_3{
	long long objid;
	unsigned long d2;
	unsigned long d3;
	unsigned long d4;
};
struct COMPONENT1_DATA6_3_1{
	unsigned long d1;
	unsigned long d2;
	unsigned long d3;
};
#pragma pack(pop)

//Component 1 - Position&Movement
class ControllablePhysicsComponent : public ReplicaComponent{
private:
	bool flag1;
	COMPONENT1_DATA1 data1;
	bool flag2;
	COMPONENT1_DATA2 data2;
	bool flag3;
	COMPONENT1_DATA3 data3;
	bool flag4;
	COMPONENT1_DATA4 data4;
	bool flag5;
	bool flag5_1;
	COMPONENT1_DATA5 data5;
	bool flag6;
	COMPONENT1_DATA6 data6;
	bool flag6_1;
	COMPONENT1_VELOCITY data6_1;
	bool flag6_2;
	COMPONENT1_VELOCITY_ANGULAR data6_2;
	bool flag6_3;
	COMPONENT1_DATA6_3 data6_3;
	bool flag6_3_1;
	COMPONENT1_DATA6_3_1 data6_3_1;
	bool flag6_4;
public:
	ControllablePhysicsComponent();
	~ControllablePhysicsComponent();

	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	void setData1(COMPONENT1_DATA1 data1);
	COMPONENT1_DATA1 getData1();
	void setData2(COMPONENT1_DATA2 data2);
	COMPONENT1_DATA2 getData2();
	void setData3(COMPONENT1_DATA3 data3);
	COMPONENT1_DATA3 getData3();
	void setData4(COMPONENT1_DATA4 data4);
	COMPONENT1_DATA4 getData4();
	void setData5(COMPONENT1_DATA5 data5);
	COMPONENT1_DATA5 getData5();
	void setHasData5(bool flag);
	bool getHasData5();
	void setPosition(COMPONENT1_POSITION pos);
	COMPONENT1_POSITION getPosition();
	void setRotation(COMPONENT1_ROTATION rot);
	COMPONENT1_ROTATION getRotation();
	void setOnGround(bool onGround);
	bool getOnGround();
	void setData6_d4(bool d);
	bool getData6_d4();
	void setVelocity(COMPONENT1_VELOCITY vel);
	COMPONENT1_VELOCITY getVelocity();
	void setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR avel);
	COMPONENT1_VELOCITY_ANGULAR getAngularVelocity();
	void setData6_3(COMPONENT1_DATA6_3 data6_3);
	COMPONENT1_DATA6_3 getData6_3();
	void setData6_3_1(COMPONENT1_DATA6_3_1);
	COMPONENT1_DATA6_3_1 getData6_3_1();
};
#pragma endregion

#pragma region SimplePhysicsComponent (Component 3)

struct COMPONENT3_POSITION {
	float posX;
	float posY;
	float posZ;

	COMPONENT3_POSITION(float x, float y, float z) {
		this->posX = x; this->posY = y; this->posZ = z;
	}

	COMPONENT3_POSITION() {
		this->posX = 0; this->posY = 0; this->posZ = 0;
	}

	bool isNull() {
		return (this->posX == 0 && this->posY == 0 && this->posZ == 0);
	}
};

struct COMPONENT3_ROTATION {
	float rotX;
	float rotY;
	float rotZ;
	float rotW;

	COMPONENT3_ROTATION() {
		this->rotX = 0; this->rotY = 0; this->rotZ = 0; this->rotW = 0;
	}

	COMPONENT3_ROTATION(float x, float y, float z, float w) {
		this->rotX = x; this->rotY = y; this->rotZ = z; this->rotW = w;
	}

	bool isNull() {
		return (this->rotX == 0 && this->rotY == 0 && this->rotZ == 0 && this->rotW == 0);
	}
};

class SimplePhysicsComponent : public ReplicaComponent {
private:
	bool flag1; // Creation only
	int data1; // Creation only
	bool flag2;
	int data2_1;
	int data2_2;
	int data2_3;
	int data2_4;
	int data2_5;
	int data2_6;
	bool flag3;
	int data3;
	bool flag4;
	COMPONENT3_POSITION position;
	COMPONENT3_ROTATION rotation;
public:
	SimplePhysicsComponent();
	~SimplePhysicsComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	int getData1();
	bool getFlag2();
	int getData2_1();
	int getData2_2();
	int getData2_3();
	int getData2_4();
	int getData2_5();
	int getData2_6();
	bool getFlag3();
	int getData3();
	bool getFlag4();
	COMPONENT3_POSITION getPosition();
	COMPONENT3_ROTATION getRotation();

	void setFlag1(bool flag1);
	void setData1(int data1);
	void setFlag2(bool flag2);
	void setData2_1(int data2_1);
	void setData2_2(int data2_2);
	void setData2_3(int data2_3);
	void setData2_4(int data2_4);
	void setData2_5(int data2_5);
	void setData2_6(int data2_6);
	void setFlag3(bool flag3);
	void setData3(int data3);
	void setFlag4(bool flag4);
	void setPosition(COMPONENT3_POSITION position);
	void setRotation(COMPONENT3_ROTATION rotation);

};

#pragma endregion

#pragma region RigidBodyPhantomPhysicsComponent (Component 20)

struct COMPONENT20_POSITION {
	float x;
	float y;
	float z;

	COMPONENT20_POSITION() {
		this->x = 0; this->y = 0; this->z = 0;
	}

	COMPONENT20_POSITION(float x, float y, float z) {
		this->x = x; this->y = y; this->z = z;
	}
};

struct COMPONENT20_ROTATION {
	float x;
	float y;
	float z;
	float w;

	COMPONENT20_ROTATION() {
		this->x = 0; this->y = 0; this->z = 0; this->w = 0;
	}

	COMPONENT20_ROTATION(float x, float y, float z, float w) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}
};

class RigidBodyPhantomPhysicsComponent : public ReplicaComponent {
private:
	bool flag1;
	COMPONENT20_POSITION pos;
	COMPONENT20_ROTATION rot;
public:
	RigidBodyPhantomPhysicsComponent();
	~RigidBodyPhantomPhysicsComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	COMPONENT20_POSITION getPos();
	COMPONENT20_ROTATION getRot();

	void setFlag1(bool flag1);
	void setPos(COMPONENT20_POSITION pos);
	void setRot(COMPONENT20_ROTATION rot);
};

#pragma endregion

#pragma region VehiclePhysicsComponent (Component 30)

class VehiclePhysicsComponent : public ReplicaComponent {
private:
	char data1; // Creation only
	bool data2; // Creation only

	bool flag3;
	bool data3;
public:
	VehiclePhysicsComponent();
	~VehiclePhysicsComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	char getData1();
	bool getData2();
	bool getFlag3();
	bool getData3();

	void setData1(char data1);
	void setData2(bool data2);
	void setFlag3(bool flag3);
	void setData3(bool data3);
};

#pragma endregion

#pragma region PhantomPhysicsComponent (Component 40)

struct COMPONENT40_POSITION {
	float posX;
	float posY;
	float posZ;

	COMPONENT40_POSITION() {
		this->posX = 0.0; this->posY = 0.0; this->posZ = 0.0;
	}

	COMPONENT40_POSITION(float x, float y, float z) {
		this->posX = x; this->posY = y; this->posZ = z;
	}

	bool isNull() {
		return (this->posX == 0.0 && this->posY == 0.0 && this->posZ == 0.0);
	}
};

struct COMPONENT40_ROTATION {
	float rotX;
	float rotY;
	float rotZ;
	float rotW;

	COMPONENT40_ROTATION() {
		this->rotX = 0.0; this->rotY = 0.0; this->rotZ = 0.0; this->rotW = 0.0;
	}

	COMPONENT40_ROTATION(float x, float y, float z, float w) {
		this->rotX = x; this->rotY = y; this->rotZ = z; this->rotW = w;
	}

	bool isNull() {
		return (this->rotX == 0 && this->rotY == 0 && this->rotZ == 0 && this->rotW == 0);
	}
};

struct COMPONENT40_DATA1 {
	long d1;
	long d2;
};

struct COMPONENT40_DATA2 {
	long d1;
	long d2;
};

struct COMPONENT40_DATA3 {
	float f1;
	float f2;
	float f3;
};

class PhantomPhysicsComponent : public ReplicaComponent {
private:
	bool flag1;
	COMPONENT40_POSITION pos;
	COMPONENT40_ROTATION rot;
	bool flag2;
	bool flag3;
	COMPONENT40_DATA1 data1;
	bool flag4_1;
	COMPONENT40_DATA2 data2;
	bool flag4_2;
	COMPONENT40_DATA3 data3;
public:
	PhantomPhysicsComponent();
	~PhantomPhysicsComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	COMPONENT40_POSITION getPosition();
	COMPONENT40_ROTATION getRotation();
	bool getFlag2();
	bool getFlag3();
	COMPONENT40_DATA1 getData1();
	bool getFlag4_1();
	COMPONENT40_DATA2 getData2();
	bool getFlag4_2();
	COMPONENT40_DATA3 getData3();


	void setFlag1(bool flag1);
	void setPosition(COMPONENT40_POSITION position);
	void setRotation(COMPONENT40_ROTATION rotation);
	void setFlag2(bool flag2);
	void setData1(COMPONENT40_DATA1 data1);
	void setData2(COMPONENT40_DATA2 data2);
	void setData3(COMPONENT40_DATA3 data3);
};

#pragma endregion

#pragma region DestructibleComponent (Component 7)

struct COMPONENT7_DATA1{
	unsigned long d1 = 0;
	bool d2f = false;
	unsigned long d2 = 0;
	bool d3 = false;
	bool d4 = false;
	bool d5 = false;
	bool d6 = false;
	bool d7 = false;
	bool d8 = false;
	bool d9 = false;
	bool d10 = false;
	bool d11 = false; //flag für d13
	bool d12 = false;
	unsigned long long d13 = 0;
	unsigned long d14 = 0;
};

struct COMPONENT7_DATA2{
	unsigned long d1 = 0;
	bool d2f = false;
	unsigned long d2 = 0;
	bool d3 = false;
	bool d4 = false;
	bool d5 = false;
	bool d6 = false;
	bool d7 = false;
	bool d8 = false;
	bool d9 = false;
	bool d10 = false;
	bool d11 = false; //flag für d13
	bool d12 = false;
	unsigned long long d13 = 0;
	unsigned long d14 = 0;
};

struct COMPONENT7_DATA3{
	unsigned long d1 = 0;
	unsigned long d2 = 0;
	unsigned long d3 = 0;
	unsigned long d4 = 0;
	unsigned long d5 = 0;
	unsigned long d6 = 0;
	unsigned long d7 = 0;
	unsigned long d8 = 0;
	unsigned long d9 = 0;
};

struct COMPONENT7_DATA4{
	unsigned long health = 4;
	float maxHealthN = 4.0F;
	unsigned long armor = 0;
	float maxArmorN = 0.0F;
	unsigned long imagination = 0;
	float maxImaginationN = 0.0F;
	unsigned long d7 = 0;
	bool d8 = false;
	bool d9 = false;
	bool d10 = false;
	float maxHealth = 4.0F;
	float maxArmor = 0.0F;
	float maxImagination = 0.0F;
};

//Index 22,23 -> Player Health etc
class DestructibleComponent : public ReplicaComponent{
private:
	//Index 22
	bool flag1;
	std::vector<COMPONENT7_DATA1> data1;
	bool flag2;
	std::vector<COMPONENT7_DATA2> data2;
	//Index 23
	bool flag3;
	COMPONENT7_DATA3 data3;
	bool flag4;
	COMPONENT7_DATA4 data4;
	std::vector<unsigned long> data4_1;
	bool trigger;
	bool data4_2;
	bool data4_3;
	bool data4_4_1;
	bool flag4_4_2;
	unsigned long data4_4_2;
	bool flag5;
	bool data5;
public:
	DestructibleComponent();
	~DestructibleComponent();

	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();
	std::vector<COMPONENT7_DATA1> *getData1Ref();
	std::vector<COMPONENT7_DATA2> *getData2Ref();
	COMPONENT7_DATA3 getData3();
	COMPONENT7_DATA4 getData4();
	std::vector<unsigned long> *getData4_1Ref();
	bool getTrigger();
	bool getData4_2();
	bool getData4_3();
	bool getData4_4_1();
	unsigned long getData4_4_2();
	bool getData5();
	void setData3(COMPONENT7_DATA3 data3);
	void setData4(COMPONENT7_DATA4 data4);
	void setTrigger(bool trigger);
	void setData4_2(bool data4_2);
	void setData4_3(bool data4_3);
	void setData4_4_1(bool data4_4_1);
	void setData4_4_2(unsigned long data4_4_2);
	void setData5(bool data5);
};
#pragma endregion

#pragma region SwitchComponent (Component 49)

class SwitchComponent : public ReplicaComponent {
private:
	bool switchState = false;

public:
	SwitchComponent();
	~SwitchComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getSwitchState();
	void setSwitchState(bool switchState);
};

#pragma endregion

#pragma region PetComponent (Component 26)

class PetComponent : public ReplicaComponent {
private:
	bool flag1;
	long data1;
	long data2;
	bool flag3;
	long long data3;
	bool flag4;
	long long data4;
	bool flag5;
	long data5_1;
	char length1;
	short data5_1_1;
	char length2;
	short data5_1_2;

public:
	PetComponent();
	~PetComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	long getData1();
	long getData2();
	bool getFlag3();
	long long getData3();
	bool getFlag4();
	long long getData4();
	bool getFlag5();
	long getData5_1();
	char getLength1();
	short getData5_1_1();
	char getLength2();
	short getData5_1_2();

	void setFlag1(bool flag1);
	void setData1(long data1);
	void setData2(long data2);
	void setData3(long long data3);
	void setData4(long long data4);
	void setData5_1(long data5_1);
	void setLength1(char length1);
	void setData5_1_1(short data5_1_1);
	void setLength2(char length2);
	void setData5_1_2(short data5_1_2);
};

#pragma endregion

#pragma region CharacterComponent (Component 4)

struct COMPONENT4_DATA3{
	bool d1 = false;
	bool d2 = false;
};

struct COMPONENT4_DATA9{
	bool d1 = false;
	bool d2 = false;
	unsigned char d3 = 0;
	bool d4 = false;
	unsigned char d5 = 0;
};

struct COMPONENT4_DATA11{
	unsigned long long d1 = 0;
	unsigned char d2 = 0;
	bool d3 = true;
	unsigned long d4 = 0xFFFFFFFF;
};

class CharacterComponent : public ReplicaComponent{
private:
	//Index 24
	bool flag1;
	bool flag1_1;
	unsigned long long data1_1;
	unsigned char data1_2;
	//Index 27
	bool hasLevel;
	unsigned long level;
	//Index 28
	bool flag3;
	COMPONENT4_DATA3 data3;
	bool flag4;
	unsigned long long data4;
	bool flag5;
	unsigned long long data5;
	bool flag6;
	unsigned long long data6;
	bool flag7;
	unsigned long long data7;
	PLAYER_STYLE style;
	PLAYER_INFO info;
	PLAYER_STATS stats;
	bool flag8a;
	bool flag8b;
	std::wstring data8;
	bool flag9;
	COMPONENT4_DATA9 data9;
	bool flag10;
	unsigned long data10;
	bool flag11;
	COMPONENT4_DATA11 data11;
public:
	CharacterComponent();
	~CharacterComponent();

	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	void setData1_1(unsigned long long d1_1);
	void setData1_2(unsigned char d1_2);
	void setLevel(unsigned long lvl);
	void setData3(COMPONENT4_DATA3 d3);
	void setData4(unsigned long long d4);
	void setData5(unsigned long long d5);
	void setData6(unsigned long long d6);
	void setData7(unsigned long long d7);

	void setStyle(PLAYER_STYLE style);
	void setInfo(PLAYER_INFO info);
	void setStats(PLAYER_STATS stats);
	void setData8(std::wstring d8);
	void setData9(COMPONENT4_DATA9 d9);
	void setData10(unsigned long d10);
	void setData11(COMPONENT4_DATA11 d11);

	unsigned long long getData1_1();
	unsigned char getData1_2();
	unsigned long getLevel();
	COMPONENT4_DATA3 getData3();
	unsigned long long getData4();
	unsigned long long getData5();
	unsigned long long getData6();
	unsigned long long getData7();
	PLAYER_STYLE getStyle();
	PLAYER_INFO getInfo();
	PLAYER_STATS getStats();
	std::wstring getData8();
	COMPONENT4_DATA9 getData9();
	unsigned long getData10();
	COMPONENT4_DATA11 getData11();
};

#pragma endregion

#pragma region InventoryComponent (Component 17)

struct COMPONENT17_EQUIPMENT{
	long long objid = 0;
	long lot = LOT::LOT_NONE;
	unsigned long long d3 = 0;
	unsigned long d4 = 1; //Always 1?
	unsigned short slot = 0;
	unsigned long d6 = 4; //Always 4?
	std::vector<unsigned char> d7;
	bool d8;
};

class InventoryComponent : public ReplicaComponent{
private:
	std::vector<COMPONENT17_EQUIPMENT> equipment;
	unsigned long data2 = 0; //May be a count for something
public:
	InventoryComponent();
	~InventoryComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();
	void setData2(unsigned long d2);
	unsigned long getData2();
	std::vector<COMPONENT17_EQUIPMENT> *getEquipment();
	long equipItem(long long objid, unsigned short slot = 0);
	bool unequipItem(long long objid);
};

#pragma endregion

#pragma region ScriptComponent (Component 5)

class ScriptComponent : public ReplicaComponent {
private:
	bool flag1;
public:
	ScriptComponent();
	~ScriptComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();

	void setFlag1(bool flag1);
};

#pragma endregion

#pragma region SkillComponent (Component 9)

class SkillComponent : public ReplicaComponent{
public:
	SkillComponent();
	~SkillComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();
};

#pragma endregion

#pragma region BaseCombatAIComponent (Component 60)

class BaseCombatAIComponent : public ReplicaComponent {
private:
	bool flag1;
	long data1;
	long long data2;
public:
	BaseCombatAIComponent();
	~BaseCombatAIComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();


	bool getFlag1();
	long getData1();
	long long getData2();

	void writeFlag1(bool flag1);
	void writeData1(long data1);
	void writeData2(long long data2);
};

#pragma endregion

#pragma region VendorComponent (Component 16)

class VendorComponent : public ReplicaComponent {
private:
	bool flag1;
	bool flag1_1;
	bool flag1_2;
public:
	VendorComponent();
	~VendorComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	bool getFlag1_1();
	bool getFlag1_2();

	void setFlag1(bool flag1);
	void setFlag1_1(bool flag1_1);
	void setFlag1_2(bool flag1_2);
};

#pragma endregion

#pragma region BouncerComponent (Component 6)

class BouncerComponent : public ReplicaComponent {
private:
	bool flag1;
	bool data1;
public:
	BouncerComponent();
	~BouncerComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	bool getData1();

	void setFlag1(bool flag1);
	void setData1(bool data1);
};

#pragma endregion

#pragma region ScriptedActivityComponent (Component 39)

class ScriptedActivityComponent : public ReplicaComponent {
private:
	bool flag1;
	long data1;
	long long data2;
	float data2_1;
	float data2_2;
	float data2_3;
	float data2_4;
	float data2_5;
	float data2_6;
	float data2_7;
	float data2_8;
	float data2_9;
	float data2_10;
public:
	ScriptedActivityComponent();
	~ScriptedActivityComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	long getData1();
	long long getData2();
	float getData2_1();
	float getData2_2();
	float getData2_3();
	float getData2_4();
	float getData2_5();
	float getData2_6();
	float getData2_7();
	float getData2_8();
	float getData2_9();
	float getData2_10();

	void setFlag1(bool flag1);
	void setData1(long data1);
	void setData2(long long data2);
	void setData2_1(float data2_1);
	void setData2_2(float data2_2);
	void setData2_3(float data2_3);
	void setData2_4(float data2_4);
	void setData2_5(float data2_5);
	void setData2_6(float data2_6);
	void setData2_7(float data2_7);
	void setData2_8(float data2_8);
	void setData2_9(float data2_9);
	void setData2_10(float data2_10);
};

#pragma endregion

#pragma region RacingControlComponent (Component 71)

struct COMPONENT71_DATA1 {
	bool not_break;
	long long playerObjID;
	long long racecarObjID;
	unsigned long data1;
	bool data2;
};

struct COMPONENT71_DATA2 {
	bool not_break;
	long long playerObjID;
	unsigned long data1;
};

struct COMPONENT71_DATA3 {
	unsigned short remainingLaps;
	short length; // Why do we need a length?
	short pathName; // Maybe the current path??
};

struct COMPONENT71_DATA4 {
	bool flag1;
	long long data1;
	float data2;
	float data3;
};

class RacingControlComponent : public ReplicaComponent {
private:
	bool flag1;
	short shortData;
	bool flag2;
	COMPONENT71_DATA1 data1;
	bool flag3;
	COMPONENT71_DATA2 data2;
	bool flag4;
	COMPONENT71_DATA3 data3;
	bool flag5;
	COMPONENT71_DATA4 data4;
public:
	RacingControlComponent();
	~RacingControlComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();

	bool getFlag1();
	short getShortData();
	bool getFlag2();
	COMPONENT71_DATA1 getData1();
	bool getFlag3();
	COMPONENT71_DATA2 getData2();
	bool getFlag4();
	COMPONENT71_DATA3 getData3();
	bool getFlag5();
	COMPONENT71_DATA4 getData4();

	void setFlag1(bool flag1);
	void setShortData(short shortData);
	void setData1(COMPONENT71_DATA1 data1);
	void setData2(COMPONENT71_DATA2 data2);
	void setData3(COMPONENT71_DATA3 data3);
	void setData4(COMPONENT71_DATA4 data4);
};

#pragma endregion

#pragma region RenderComponent (Component 2)

class RenderComponent : public ReplicaComponent{
public:
	RenderComponent();
	~RenderComponent();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();
};

#pragma endregion

#pragma region Index36Component (Component 107)

class Index36Component : public ReplicaComponent{
public:
	Index36Component();
	~Index36Component();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	unsigned int getComponentID();
};

#pragma endregion