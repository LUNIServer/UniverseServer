#pragma once

#include "ReplicaDefinitions.h"

#include "RakNet\BitStream.h"
#include "ReplicaDefinitions.h"
#include "SUtil\Common.h"

#include <vector>

//Abstract Base Class -> Interface
class ReplicaComponent{
public:
	virtual void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType) = 0;
};

#pragma region Component1
//Component 1 Structs
#pragma pack(push, 1)
struct COMPONENT1_DATA1{
	ulong d1 = 0;
	bool d2 = false;
	bool d3 = false;
};
struct COMPONENT1_DATA2{
	ulong d1 = 0;
	ulong d2 = 0;
	ulong d3 = 0;
	ulong d4 = 0;
	ulong d5 = 0;
	ulong d6 = 0;
	ulong d7 = 0;
};
struct COMPONENT1_DATA3{
	ulong d1 = 0;
	ulong d2 = 0;
};
struct COMPONENT1_DATA4{
	ulong d1 = 0;
	bool d2 = false;
};
struct COMPONENT1_DATA5{
	ulong d1 = 0;
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
	ulong d2;
	ulong d3;
	ulong d4;
};
struct COMPONENT1_DATA6_3_1{
	ulong d1;
	ulong d2;
	ulong d3;
};
#pragma pack(pop)

//Component 1 - Position&Movement
class Component1 : public ReplicaComponent{
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
	Component1();
	~Component1();

	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);

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

#pragma region Component4

struct COMPONENT4_DATA3{
	bool d1 = false;
	bool d2 = false;
};

struct COMPONENT4_DATA9{
	bool d1 = false;
	bool d2 = false;
	uchar d3 = 0;
	bool d4 = false;
	uchar d5 = 0;
};

struct COMPONENT4_DATA11{
	ulonglong d1 = 0;
	uchar d2 = 0;
	bool d3 = true;
	ulong d4 = 0xFFFFFFFF;
};

class Component4 : public ReplicaComponent{
private:
	//Index 24
	bool flag1;
	bool flag1_1;
	ulonglong data1_1;
	uchar data1_2;
	//Index 27
	bool hasLevel;
	ulong level;
	//Index 28
	bool flag3;
	COMPONENT4_DATA3 data3;
	bool flag4;
	ulonglong data4;
	bool flag5;
	ulonglong data5;
	bool flag6;
	ulonglong data6;
	bool flag7;
	ulonglong data7;
	PLAYER_STYLE style;
	PLAYER_INFO info;
	PLAYER_STATS stats;
	bool flag8a;
	bool flag8b;
	std::wstring data8;
	bool flag9;
	COMPONENT4_DATA9 data9;
	bool flag10;
	ulong data10;
	bool flag11;
	COMPONENT4_DATA11 data11;
public:
	Component4();
	~Component4();

	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);

	void setData1_1(ulonglong d1_1);
	void setData1_2(uchar d1_2);
	void setLevel(ulong lvl);
	void setData3(COMPONENT4_DATA3 d3);
	void setData4(ulonglong d4);
	void setData5(ulonglong d5);
	void setData6(ulonglong d6);
	void setData7(ulonglong d7);

	void setStyle(PLAYER_STYLE style);
	void setInfo(PLAYER_INFO info);
	void setStats(PLAYER_STATS stats);
	void setData8(std::wstring d8);
	void setData9(COMPONENT4_DATA9 d9);
	void setData10(ulong d10);
	void setData11(COMPONENT4_DATA11 d11);

	ulonglong getData1_1();
	uchar getData1_2();
	ulong getLevel();
	COMPONENT4_DATA3 getData3();
	ulonglong getData4();
	ulonglong getData5();
	ulonglong getData6();
	ulonglong getData7();
	PLAYER_STYLE getStyle();
	PLAYER_INFO getInfo();
	PLAYER_STATS getStats();
	std::wstring getData8();
	COMPONENT4_DATA9 getData9();
	ulong getData10();
	COMPONENT4_DATA11 getData11();
};

#pragma endregion

#pragma region Component7
struct COMPONENT7_DATA1{
	ulong d1 = 0;
	bool d2f = false;
	ulong d2 = 0;
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
	ulonglong d13 = 0;
	ulong d14 = 0;
};

struct COMPONENT7_DATA2{
	ulong d1 = 0;
	bool d2f = false;
	ulong d2 = 0;
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
	ulonglong d13 = 0;
	ulong d14 = 0;
};

struct COMPONENT7_DATA3{
	ulong d1 = 0;
	ulong d2 = 0;
	ulong d3 = 0;
	ulong d4 = 0;
	ulong d5 = 0;
	ulong d6 = 0;
	ulong d7 = 0;
	ulong d8 = 0;
	ulong d9 = 0;
};

struct COMPONENT7_DATA4{
	ulong health = 4;
	float maxHealthN = 4.0F;
	ulong armor = 0;
	float maxArmorN = 0.0F;
	ulong imagination = 0;
	float maxImaginationN = 0.0F;
	ulong d7 = 0;
	bool d8 = false;
	bool d9 = false;
	bool d10 = false;
	float maxHealth = 4.0F;
	float maxArmor = 0.0F;
	float maxImagination = 0.0F;
};

//Index 22,23 -> Player Health etc
class Component7 : public ReplicaComponent{
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
	std::vector<ulong> data4_1;
	bool trigger;
	bool data4_2;
	bool data4_3;
	bool data4_4_1;
	bool flag4_4_2;
	ulong data4_4_2;
	bool flag5;
	bool data5;
public:
	Component7();
	~Component7();

	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	std::vector<COMPONENT7_DATA1> *getData1Ref();
	std::vector<COMPONENT7_DATA2> *getData2Ref();
	COMPONENT7_DATA3 getData3();
	COMPONENT7_DATA4 getData4();
	std::vector<ulong> *getData4_1Ref();
	bool getTrigger();
	bool getData4_2();
	bool getData4_3();
	bool getData4_4_1();
	ulong getData4_4_2();
	bool getData5();
	void setData3(COMPONENT7_DATA3 data3);
	void setData4(COMPONENT7_DATA4 data4);
	void setTrigger(bool trigger);
	void setData4_2(bool data4_2);
	void setData4_3(bool data4_3);
	void setData4_4_1(bool data4_4_1);
	void setData4_4_2(ulong data4_4_2);
	void setData5(bool data5);
};
#pragma endregion

#pragma region Component17
struct COMPONENT17_EQUIPMENT{
	long long objid = 0;
	long lot = LOT::LOT_NONE;
	ulonglong d3 = 0;
	ulong d4 = 1; //Always 1?
	ushort slot = 0;
	ulong d6 = 4; //Always 4?
	std::vector<uchar> d7;
	bool d8;
};

class Component17 : public ReplicaComponent{
private:
	std::vector<COMPONENT17_EQUIPMENT> equipment;
	ulong data2 = 0; //May be a count for something
public:
	Component17();
	~Component17();
	void writeToPacket(RakNet::BitStream *packet, REPLICA_PACKET_TYPE packetType);
	void setData2(ulong d2);
	ulong getData2();
	std::vector<COMPONENT17_EQUIPMENT> *getEquipment();
	long equipItem(long long objid, ushort slot = 0);
	bool unequipItem(long long objid);
};
#pragma endregion