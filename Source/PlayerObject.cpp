#include "Database.h"
#include "PlayerObject.h"
#include "Replica.h"

#include "RakNet\ReplicaManager.h"
#include "UsersPool.h"

#include <map>

extern ReplicaManager replicaManager;
extern std::map<SystemAddress, ZoneId> Player;
extern Ref< UsersPool > WorldOnlineUsers;

PlayerObject::PlayerObject(long long objid, std::wstring name){
	this->objid = objid;
	this->name = name;

	this->component1 = new Component1();
	this->component7 = new Component7();
	this->component4 = new Component4();
	this->component17 = new Component17();

	COMPONENT7_DATA4 d4 = this->component7->getData4();
	d4.health = 5;
	d4.maxHealthN = 5.0F;
	d4.maxHealth = 5.0F;
	//d4.imagination = 1;
	//d4.maxImagination = 15.0F;
	//d4.maxImaginationN = 15.0F;
	this->component7->setData4(d4);

	auto qr = Database::Query("SELECT `object` FROM `equipment` WHERE `owner` = '" + std::to_string(objid) + "';");
	ushort numrows = (ushort) mysql_num_rows(qr);
	for (ushort k = 0; k < numrows; k++){
		auto ftc = mysql_fetch_row(qr);
		long long itemid = std::stoll(ftc[0]);
		this->component17->equipItem(itemid);
	}
}

PlayerObject::~PlayerObject(){
	delete this->component1;
	this->component1 = NULL;
	delete this->component7;
	this->component7 = NULL;
	delete this->component4;
	this->component4 = NULL;
	delete this->component17;
	this->component17 = NULL;
	replicaManager.DereferencePointer(this);
}

std::wstring PlayerObject::getName(){
	return this->name;
}

// --- Components ---

Component1 *PlayerObject::getComponent1(){ return this->component1; }
Component7 *PlayerObject::getComponent7(){ return this->component7; }
Component4 *PlayerObject::getComponent4(){ return this->component4; }
Component17 *PlayerObject::getComponent17(){ return this->component17; }

void PlayerObject::doCreation(SystemAddress playerController, ZoneId playerZone, bool createOthers){
	this->clientIP = playerController;
	this->zone = playerZone;
	this->create(playerController, true);
	typedef std::map<SystemAddress, ZoneId>::iterator PlayerIterator;
	for (PlayerIterator iterator = Player.begin(); iterator != Player.end(); iterator++) {
		if (iterator->second == playerZone){ //Same World
			if (iterator->first != playerController){ //Not Same Address
				Ref<User> other = WorldOnlineUsers->Find(iterator->first);
				PlayerObject *p = other->GetPlayer();
				if (p != NULL){ //Player not null
					if (createOthers) p->create(playerController); //Create Others for me
					this->create(iterator->first); //Create me for others
				}
			}
		}
	}
}

void PlayerObject::create(SystemAddress address, bool himself){
	replicaManager.Construct(this, false, address, false);
	if (himself){
		//replicaManager.DisableReplicaInterfaces(this, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE);
		this->isCreated = true;
	}
}

void PlayerObject::doSerialization(SystemAddress playerController, ZoneId playerZone){
	if (!isCreated){
		std::cout << "[WORLD] ERROR: Player [objid:" << this->objid << "] has not been created!" << std::endl;
	}
	else{
		this->serialize(playerController);
		typedef std::map<SystemAddress, ZoneId>::iterator PlayerIterator;
		for (PlayerIterator iterator = Player.begin(); iterator != Player.end(); iterator++) {
			if (iterator->second == playerZone){ //Same World
				if (iterator->first != playerController){ //Not Same Address
					this->serialize(iterator->first);
				}
			}
		}
	}
}

void PlayerObject::serialize(SystemAddress address){
	replicaManager.SignalSerializeNeeded(this, address, false);
}

void PlayerObject::serialize(){
	this->doSerialization(this->clientIP, this->zone);
}

void PlayerObject::doDestruction(SystemAddress playerController, ZoneId playerZone, bool destructOthers){
	if (!isCreated){
		std::cout << "[WORLD] ERROR: Player [objid:" << this->objid << "] has not been created!" << std::endl;
	}
	else{
		isCreated = false;
		typedef std::map<SystemAddress, ZoneId>::iterator PlayerIterator;
		for (PlayerIterator iterator = Player.begin(); iterator != Player.end(); iterator++) {
			if (iterator->second == playerZone){ //Same World
				if (iterator->first != playerController){ //Not Same Address
					
					Ref<User> other = WorldOnlineUsers->Find(iterator->first); 
					PlayerObject *p = other->GetPlayer();
					if (p != NULL){ //Player not null
						this->destruct(iterator->first); //Destroy me for all the others
						if (destructOthers) p->destruct(playerController); //Then destroy all others for me
					}
				}
			}
		}
		this->destruct(playerController); //Destroying my char for me
	}
}

void PlayerObject::destruct(SystemAddress address){
	replicaManager.Destruct(this, address, false);
}

void PlayerObject::destruct(){
	if (isCreated){
		this->doDestruction(this->clientIP, this->zone);
	}
}

ReplicaReturnResult PlayerObject::SendConstruction(RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp){
	//This is the construction Packet

	std::cout << "Send construction of '";
	std::wcout << this->name;
	std::cout << "' to " << systemAddress.ToString() << std::endl;
	replicaPacketGeneral(outBitStream, REPLICA_CONSTRUCTION_PACKET, this->objid, this->name);

	this->component1->writeToPacket(outBitStream, REPLICA_CONSTRUCTION_PACKET);
	this->component7->writeToPacket(outBitStream, REPLICA_CONSTRUCTION_PACKET);
	this->component4->writeToPacket(outBitStream, REPLICA_CONSTRUCTION_PACKET);
	//replicaPacketLOT1i24(outBitStream, REPLICA_CONSTRUCTION_PACKET); //Index 24
	//replicaPacketLOT1i27(outBitStream, REPLICA_CONSTRUCTION_PACKET); //Index 27
	//replicaPacketLOT1i28(outBitStream, REPLICA_CONSTRUCTION_PACKET); //Index 28
	//replicaPacketLOT1i29(outBitStream, REPLICA_CONSTRUCTION_PACKET, PLAYER_STYLE()); //Index 29

	this->component17->writeToPacket(outBitStream, REPLICA_CONSTRUCTION_PACKET);
	
	//replicaPacketLOT1i30(outBitStream, REPLICA_CONSTRUCTION_PACKET); //Index 30

	replicaPacketLOT1i31(outBitStream, REPLICA_CONSTRUCTION_PACKET); //Index 31
	replicaPacketLOT1i32(outBitStream, REPLICA_CONSTRUCTION_PACKET); //Index 32

	replicaPacketLOT1i36(outBitStream, REPLICA_CONSTRUCTION_PACKET); //Index 36

	replicaManager.SetScope(this, true, systemAddress, false);

	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult PlayerObject::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp){
	//This is the destruction packet
	std::cout << "Send destruction of '";
	std::wcout << this->name;
	std::cout << "' to " << systemAddress.ToString() << std::endl;

	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult PlayerObject::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp){
	std::cout << "Recieve destruction of '";
	std::wcout << this->name;
	std::cout << "' to " << systemAddress.ToString() << std::endl;
	//This happens only on the client side
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult PlayerObject::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp){

	//We have to do this, so the scope change isn't cancelled
	outBitStream->Write(inScope);
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult PlayerObject::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp){
	//This happens only on the client side
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult PlayerObject::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime,
	PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags){
	//This is the Serialization packet

	replicaPacketGeneral(outBitStream, REPLICA_SERIALIZATION_PACKET, this->objid, this->name);
	//std::cout << "[WRLD] Serializing Player" << std::endl;
	this->component1->writeToPacket(outBitStream, REPLICA_SERIALIZATION_PACKET);
	this->component7->writeToPacket(outBitStream, REPLICA_SERIALIZATION_PACKET);
	this->component4->writeToPacket(outBitStream, REPLICA_SERIALIZATION_PACKET);
	//replicaPacketLOT1i24(outBitStream, REPLICA_SERIALIZATION_PACKET); //Index 24
	//replicaPacketLOT1i27(outBitStream, REPLICA_SERIALIZATION_PACKET); //Index 27
	//replicaPacketLOT1i28(outBitStream, REPLICA_SERIALIZATION_PACKET); //Index 28
	//replicaPacketLOT1i29(outBitStream, REPLICA_SERIALIZATION_PACKET, PLAYER_STYLE()); //Index 29

	this->component17->writeToPacket(outBitStream, REPLICA_SERIALIZATION_PACKET);
	//replicaPacketLOT1i30(outBitStream, REPLICA_SERIALIZATION_PACKET); //Index 30
	replicaPacketLOT1i31(outBitStream, REPLICA_SERIALIZATION_PACKET); //Index 31
	replicaPacketLOT1i32(outBitStream, REPLICA_SERIALIZATION_PACKET); //Index 32

	replicaPacketLOT1i36(outBitStream, REPLICA_SERIALIZATION_PACKET); //Index 36

	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult PlayerObject::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress){
	//TODO implement deserialize once the Serialization isn't static any more
	std::cout << "[WRLD] CLIENT SENT SERIALIZE" << std::endl;
	return REPLICA_PROCESSING_DONE;
}