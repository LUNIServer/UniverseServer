#include "Replica.h"

using namespace std;
using namespace RakNet;

void replicaPacketGeneral(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, long long objid, std::wstring name){
	if (packetType == REPLICA_PACKET_TYPE::REPLICA_CONSTRUCTION_PACKET){
		Packet->Write(objid);
		Packet->Write(1UL);

		Packet->Write((uchar)name.size());
		for (uint k = 0; k < name.size(); k++){
			Packet->Write(name.at(k));
		}

		Packet->Write(0UL);
		Packet->Write(false);
		Packet->Write(false);
		Packet->Write(false);
		Packet->Write(false);
		Packet->Write(false);
		Packet->Write(false);
		bool isMythran = false;
		Packet->Write(isMythran);
		if (isMythran){
			Packet->Write((uchar)2);
		}
	}
	Packet->Write(true);
	Packet->Write(false);
	Packet->Write(false);
}

void replicaPacketLOT1i20(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, REPLICA_OBJECT_POSITION position){ 
	//Component type 1? (rigid body?), the unknown values may have to do with collision
	if (packetType == REPLICA_PACKET_TYPE::REPLICA_CONSTRUCTION_PACKET){
		bool flag_20_1 = false;
		Packet->Write(flag_20_1);
		if (flag_20_1){
			ulong d20_1_1 = 0;
			bool d20_1_2 = false;
			bool d20_1_3 = false;
			Packet->Write(d20_1_1);
			Packet->Write(d20_1_2);
			Packet->Write(d20_1_3);
		}

		bool flag_20_2 = true;
		Packet->Write(flag_20_2);
		if (flag_20_2){
			for (uchar k = 0; k < 7; k++){ //Probably not actually a for-loop
				Packet->Write(0UL);
			}
		}
	}

	bool flag_20_3 = true;
	Packet->Write(flag_20_3);
	if (flag_20_3){
		ulong d20_3_1 = 1065353216;
		ulong d20_3_2 = 1065772647;
		Packet->Write(d20_3_1);
		Packet->Write(d20_3_2);
	}

	bool flag_20_4 = true;
	Packet->Write(flag_20_4);
	if (flag_20_4){
		ulong d20_4_1 = 0;
		Packet->Write(d20_4_1);
		bool d20_4_2 = false;
		Packet->Write(d20_4_2);
	}

	bool flag_20_5 = true;
	Packet->Write(flag_20_5);
	if (flag_20_5){
		bool d20_5_1 = false;
		Packet->Write(d20_5_1);
		if (d20_5_1){
			ulong d20_5_1_1 = 0;
			Packet->Write(d20_5_1_1);
			bool d20_5_1_2 = false;
			Packet->Write(d20_5_1_2);
		}
	}

	bool physicalFlag = true;
	Packet->Write(physicalFlag);
	if (physicalFlag){
		float rotX = (float) 0;
		float rotY = (float) 1; // 0.9248538;
		float rotZ = (float) 0;
		float rotW = (float) 0; // -0.3799558;

		bool onGround = true;
		bool physical_d1 = false;

		Packet->Write(position.posX);
		Packet->Write(position.posY);
		Packet->Write(position.posZ);

		Packet->Write(rotX);
		Packet->Write(rotY);
		Packet->Write(rotZ);
		Packet->Write(rotW);

		Packet->Write(onGround);
		Packet->Write(physical_d1);

		bool physical_velocityFlag = false;
		Packet->Write(physical_velocityFlag);
		if (physical_velocityFlag){
			float velX = 0;
			float velY = 0;
			float velZ = 0;

			Packet->Write(velX);
			Packet->Write(velY);
			Packet->Write(velZ);
		}

		bool physical_angularVelocityFlag = false;
		Packet->Write(physical_angularVelocityFlag);
		if (physical_angularVelocityFlag){
			float avelX = 0;
			float avelY = 0;
			float avelZ = 0;
			Packet->Write(avelX);
			Packet->Write(avelY);
			Packet->Write(avelZ);
		}

		bool physical_d2 = false;
		Packet->Write(physical_d2);
		if (physical_d2){
			ulonglong physical_d2_1 = 0;
			float physical_d2_2 = 0;
			float physical_d2_3 = 0;
			float physical_d2_4 = 0;

			Packet->Write(physical_d2_1);
			Packet->Write(physical_d2_2);
			Packet->Write(physical_d2_3);
			Packet->Write(physical_d2_4);

			bool physical_d2_5 = false;
			Packet->Write(physical_d2_5);
			if (physical_d2_5){
				float physical_d2_5_1 = 0;
				float physical_d2_5_2 = 0;
				float physical_d2_5_3 = 0;
				Packet->Write(physical_d2_5_1);
				Packet->Write(physical_d2_5_2);
				Packet->Write(physical_d2_5_3);
			}
		}

		if (packetType == REPLICA_PACKET_TYPE::REPLICA_SERIALIZATION_PACKET){
			bool physical_d3 = false;
			Packet->Write(physical_d3);
		}
	}
}

void replicaPacketLOT1i22(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){ //part of component type 7?
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		bool flag_22_1 = false;
		Packet->Write(flag_22_1);
		if (flag_22_1){
			ulong d22_1_1 = 0;
			Packet->Write(d22_1_1);
			for (ulong k = 0; k < d22_1_1; k++){
				//TODO implement loop
				//See Documentation LOT 1 Index 22
			}
		}

		bool flag_22_2 = false;
		Packet->Write(flag_22_2);
		if (flag_22_2){
			ulong d22_2_1 = 0;
			Packet->Write(d22_2_1);
			for (ulong k = 0; k < d22_2_1; k++){
				//TODO implement loop
				//See Documentation LOT 1 Index 22
			}
		}
	}
}

void replicaPacketLOT1i23(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, ulong health){
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		bool flag23_1 = true;
		Packet->Write(flag23_1);
		if (flag23_1){
			ulong d23_1[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			for (uchar k = 0; k < 9; k++){
				Packet->Write(d23_1[k]);
			}
		}
	}

	bool hasPlayerStatus = true;
	Packet->Write(hasPlayerStatus);
	if (hasPlayerStatus){
		ulong playerHealth = health;
		float playerHealthMax = 9.0F;
		ulong playerArmor = 0;
		float playerArmorMax = 0.0F;
		ulong playerImagination = 0;
		float playerImaginationMax = 40.0F;
		ulong playerUnknownStatusValue = 0; //Even that data type is not known

		Packet->Write(playerHealth);
		Packet->Write(playerHealthMax);
		Packet->Write(playerArmor);
		Packet->Write(playerArmorMax);
		Packet->Write(playerImagination);
		Packet->Write(playerImaginationMax);
		Packet->Write(playerUnknownStatusValue);

		bool playerBit1 = true;
		bool playerBit2 = false;
		bool playerBit3 = false;

		Packet->Write(playerBit1);
		Packet->Write(playerBit2);
		Packet->Write(playerBit3);

		Packet->Write(playerHealthMax);
		Packet->Write(playerArmorMax);
		Packet->Write(playerImaginationMax);

		ulong playerFactionCounterUnsure = 2;
		Packet->Write(playerFactionCounterUnsure);
		for (ulong k = 0; k < playerFactionCounterUnsure; k++){ //Could also be something else then a loop
			ulong playerFactionIdUnsure = 1;
			Packet->Write(playerFactionIdUnsure);
		}

		bool playerTriggerBit = false;
		Packet->Write(playerTriggerBit);

		if (packetType == REPLICA_CONSTRUCTION_PACKET){
			bool playerBit4 = false;
			bool playerBit5 = false;
			Packet->Write(playerBit4);
			Packet->Write(playerBit5);
			if (playerTriggerBit){
				bool playerBit6 = false;
				bool playerBit7 = false;
				Packet->Write(playerBit6);
				Packet->Write(playerBit7);
				if (playerBit7){
					ulong playerUnknownStatusValue2 = 0;
					Packet->Write(playerUnknownStatusValue2);
				}
			}
		}
	}

	bool flag23_2 = true;
	Packet->Write(flag23_2);
	if (flag23_2){
		bool d23_2_1 = false;
		Packet->Write(d23_2_1);
	}
}

void replicaPacketLOT1i24(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){
	bool flag24_1 = true;
	Packet->Write(flag24_1);
	if (flag24_1){
		bool d24_1_1 = false;
		Packet->Write(d24_1_1);
		if (d24_1_1){
			ulonglong d24_1_1_1 = 0;
			Packet->Write(d24_1_1_1);
		}

		uchar d24_1_2 = 0;
		Packet->Write(d24_1_2);
	}
}

void replicaPacketLOT1i27(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){
	bool updateLevel = true;
	Packet->Write(updateLevel);
	if (updateLevel){
		ulong level = 42;
		Packet->Write(level);
	}
}

void replicaPacketLOT1i28(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){
	bool flag28_1 = true;
	Packet->Write(flag28_1);
	if (flag28_1){
		bool d28_1_1 = true;
		bool d28_1_2 = false;
		Packet->Write(d28_1_1);
		Packet->Write(d28_1_2);
	}
}

void replicaPacketLOT1i29(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType, PLAYER_STYLE playerStyle){
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		/*bool coFlag = true; //Not 100% known to be co
		Packet->Write(coFlag);
		if (coFlag){
			ulonglong co = 65552;
			Packet->Write(co);
		}

		bool flag29_1 = false;
		bool flag29_2 = false;
		bool flag29_3 = false;
		Packet->Write(flag29_1);
		if (flag29_1){
			ulonglong d29_1 = 0;
			Packet->Write(d29_1);
		}
		Packet->Write(flag29_2);
		if (flag29_2){
			ulonglong d29_2 = 0;
			Packet->Write(d29_2);
		}
		Packet->Write(flag29_3);
		if (flag29_3){
			ulonglong d29_3 = 0;
			Packet->Write(d29_3);
		}

		Packet->Write(playerStyle);*/

		/*ulong playerStyleHairColor = 96;
		ulong playerStyleHairStyle = 4;
		ulong playerStyleHD_HDC = 0; //hd or hdc? Unknown
		ulong playerStyleShirtColor = 96;
		ulong playerStylePantsColor = 6;
		ulong playerStyleCD = 24; //cd? Unknown
		ulong playerStyleHDC_HD = 0;//hdc or hd? Unknown
		ulong playerStyleEyebrowsStyle = 20;
		ulong playerStyleEyesStyle = 1;
		ulong playerStyleMouthStyle = 8;*/

		/*Packet->Write(playerStyleHairColor);
		Packet->Write(playerStyleHairStyle);
		Packet->Write(playerStyleHD_HDC);
		Packet->Write(playerStyleShirtColor);
		Packet->Write(playerStylePantsColor);
		Packet->Write(playerStyleCD);
		Packet->Write(playerStyleHDC_HD);
		Packet->Write(playerStyleEyebrowsStyle);
		Packet->Write(playerStyleEyesStyle);
		Packet->Write(playerStyleMouthStyle);*/

		/*ulonglong accountID = 104116;
		ulonglong llog = 1327707052;
		ulonglong unknown = 0;
		ulonglong legoScore = 100000; //Ingame Universe Points

		//Packet->Write(accountID);
		//Packet->Write(llog);
		//Packet->Write(unknown);
		//Packet->Write(legoScore);

		bool isFreeToPlay = false;
		//Packet->Write(isFreeToPlay);

		PLAYER_STATS playerStats;
		playerStats.CurrencyCollected = 768568;
		playerStats.BricksCollected = 13644;
		playerStats.SmashablesSmashed = 4683;
		playerStats.QuickBuildsCompleted = 2376;
		playerStats.EnemiesSmashed = 11480;
		playerStats.RocketsUsed = 345;
		playerStats.MissionsCompleted = 471;
		playerStats.PetsTamed = 22;
		playerStats.ImaginationPowerupsCollected = 24177;
		playerStats.LifePowerupsCollected = 13104;
		playerStats.ArmorPowerupsCollected = 13678;
		playerStats.DistanceTraveled = 1951641;
		playerStats.Smashed = 78;
		playerStats.DamageTaken = 11559;
		playerStats.DamageHealed = 947;
		playerStats.ArmorRepaired = 11012;
		playerStats.ImaginationRestored = 17524;
		playerStats.ImaginationUsed = 16499;
		playerStats.DistanceDriven = 963599;
		playerStats.AirborneInRacecar = 612;
		playerStats.RacingImaginationPowerupsCollected = 1673;
		playerStats.RacingImaginationCratesSmashed = 63;
		playerStats.RaceCarBoostActivated = 439;
		playerStats.WrecksInRaceCar = 48;
		playerStats.RacingSmashablesSmashed = 392;
		playerStats.RacesFinished = 22;
		playerStats.Place1RacesFinishes = 15;
		Packet->Write(playerStats);

		bool flag29_4 = false;
		bool flag29_5 = false;
		Packet->Write(flag29_4);
		Packet->Write(flag29_5);
		if (!flag29_4 && flag29_5){
			wstring d29_4_str = L"CheekyMonkey";
			Packet->Write((UINT16)d29_4_str.size());
			for (uint k = 0; k < d29_4_str.size(); k++){
				Packet->Write(d29_4_str.at(k));
			}
		}*/
	}

	bool flag29_6 = true;
	Packet->Write(flag29_6);
	if (flag29_6){
		REPLICA_INDEX29_6_DATA d29_6;
		Packet->Write(d29_6);
	}

	bool flag29_7 = true;
	Packet->Write(flag29_7);
	if (flag29_7){
		ulong d29_7_1 = 0;
		Packet->Write(d29_7_1);
	}

	bool flag29_8 = true;
	Packet->Write(flag29_8);
	if (flag29_8){
		REPLICA_INDEX29_8_DATA d29_8;
		Packet->Write(d29_8);
	}
}

void replicaPacketLOT1i30(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){
	bool flagEquipment = true;
	Packet->Write(flagEquipment);
	if (flagEquipment){
		
		REPLICA_OBJECT_EQUIPMENT obj1, obj3, obj4, obj5, obj6;
		obj1.obj.objid = 1152921509779764804;
		obj1.obj.LOT = (LOT)16065UL;
		obj1.slot = 54;
		//obj2.obj.objid = 1152921510794154769;
		//obj2.obj.LOT = LOT_JETPACK;
		//obj2.slot = 110;
		obj3.obj.objid = 1152921507385891433;
		obj3.obj.LOT = (LOT)8544UL;
		obj3.slot = 3;
		obj4.obj.objid = 1152921507386600145;
		obj4.obj.LOT = (LOT)8642UL;
		obj4.slot = 16;
		obj5.obj.objid = 1152921507887516864;
		obj5.obj.LOT = (LOT)12677UL;
		obj5.slot = 15;
		obj6.obj.objid = 1152921509701580763;
		obj6.obj.LOT = (LOT)12730UL;
		obj6.slot = 14;
		vector<REPLICA_OBJECT_EQUIPMENT> playerEquipment = vector<REPLICA_OBJECT_EQUIPMENT>({ obj1, obj3, obj4, obj5 });
		vector<bool> flag30_1 = vector<bool>({ false, false, false, false });
		vector<bool> flag30_2 = vector<bool>({ true, true, true, true });
		vector<bool> flag30_3 = vector<bool>({ true, true, true, true });
		vector<bool> flag30_4 = vector<bool>({ false, false, false, false });
		vector<bool> flag30_5 = vector<bool>({ false, false, false, false });
		vector<bool> flag30_6 = vector<bool>({ true, true, true, true });
		
		Packet->Write((ulong)playerEquipment.size());

		for (uint k = 0; k < playerEquipment.size(); k++){
			Packet->Write(playerEquipment.at(k).obj.objid);
			Packet->Write((ulong)playerEquipment.at(k).obj.LOT);
			Packet->Write(flag30_1.at(k));
			if (flag30_1.at(k)){
				ulonglong unknown = 0;
				Packet->Write(unknown);
			}
			Packet->Write(flag30_2.at(k));
			if (flag30_2.at(k)){
				ulong always1Unknown = 1;
				Packet->Write(always1Unknown);
			}
			Packet->Write(flag30_3.at(k));
			if (flag30_3.at(k)){
				Packet->Write(playerEquipment.at(k).slot);
			}
			Packet->Write(flag30_4.at(k));
			if(flag30_4.at(k)){
				ulong always4Unknown = 4;
				Packet->Write(always4Unknown);
			}
			Packet->Write(flag30_5.at(k));
			if (flag30_5.at(k)){
				//TODO: Implement LDF
			}
			Packet->Write(flag30_6.at(k));
		}
	}

	bool flag30_7 = true;
	Packet->Write(flag30_7);
	if (flag30_7){
		ulong d30_7_1 = 0;
		Packet->Write(d30_7_1);
	}
}

void replicaPacketLOT1i31(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		Packet->Write(false);
		//TODO
	}
}

void replicaPacketLOT1i32(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_CONSTRUCTION_PACKET){
		Packet->Write(0UL);
		//TODO
	}
}

void replicaPacketLOT1i36(RakNet::BitStream *Packet, REPLICA_PACKET_TYPE packetType){
	Packet->Write(true);
	Packet->Write(0ULL);
}

/*void replicaCreationPacket(RakNet::BitStream *Packet){
	//NetworkID n = playerObject->GetNetworkID();
	replicaPacketHeader(Packet, REPLICA_CONSTRUCTION_PACKET, 4836);

	replicaPacketGeneral(Packet, REPLICA_CONSTRUCTION_PACKET);

	replicaPacketLOT1i20(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 20
	replicaPacketLOT1i22(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 22
	replicaPacketLOT1i23(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 23
	replicaPacketLOT1i24(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 24
	//Index 25,26 missing
	replicaPacketLOT1i27(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 27
	replicaPacketLOT1i28(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 28
	replicaPacketLOT1i29(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 29

	replicaPacketLOT1i30(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 30
	replicaPacketLOT1i31(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 31
	replicaPacketLOT1i32(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 32

	replicaPacketLOT1i36(Packet, REPLICA_CONSTRUCTION_PACKET); //Index 36
}*/