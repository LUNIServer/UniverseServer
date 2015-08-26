#include "Database.h"
#include "PlayerObject.h"
#include "Replica.h"

#include "RakNet\ReplicaManager.h"
#include "UsersPool.h"
#include "AccountsDB.h"

#include "Logger.h"
#include "UtfConverter.h"

#include <map>

extern ReplicaManager replicaManager;
extern std::map<SystemAddress, ZoneId> Player;
extern Ref< UsersPool > WorldOnlineUsers;

PlayerObject::PlayerObject(long long objid, std::wstring name){
	this->objid = objid;
	this->name = name;

	this->addComponent(new Component1());
	this->addComponent(new Component7());
	this->addComponent(new Component4());
	this->addComponent(new Component17());
	this->addComponent(new Component9());
	this->addComponent(new Component2());
	this->addComponent(new Component107());

	COMPONENT7_DATA4 d4 = this->getComponent7()->getData4();
	d4.health = 5;
	d4.maxHealthN = 5.0F;
	d4.maxHealth = 5.0F;
	//d4.imagination = 1;
	//d4.maxImagination = 15.0F;
	//d4.maxImaginationN = 15.0F;
	this->getComponent7()->setData4(d4);

	auto qr = Database::Query("SELECT `object` FROM `equipment` WHERE `owner` = '" + std::to_string(objid) + "';");
	ushort numrows = (ushort) mysql_num_rows(qr);
	for (ushort k = 0; k < numrows; k++){
		auto ftc = mysql_fetch_row(qr);
		long long itemid = std::stoll(ftc[0]);
		this->getComponent17()->equipItem(itemid);
	}
}

PlayerObject::~PlayerObject(){
	this->deleteComponents();
	replicaManager.DereferencePointer(this);
}

std::wstring PlayerObject::getName(){
	return this->name;
}

// --- Components ---

Component1 *PlayerObject::getComponent1(){ return (Component1 *) this->getComponent(1); }
Component7 *PlayerObject::getComponent7(){ return (Component7 *) this->getComponent(7); }
Component4 *PlayerObject::getComponent4(){ return (Component4 *) this->getComponent(4); }
Component17 *PlayerObject::getComponent17(){ return (Component17 *) this->getComponent(17); }
Component9 *PlayerObject::getComponent9(){ return (Component9 *) this->getComponent(9); }
Component2 *PlayerObject::getComponent2(){ return (Component2 *) this->getComponent(2); }
Component107 *PlayerObject::getComponent107(){ return (Component107 *) this->getComponent(107); }

void PlayerObject::doCreation(SystemAddress playerController, ZoneId playerZone, bool createOthers){
	//Add information about the client
	this->clientIP = playerController;
	this->zone = playerZone;
	//Create the object for the client
	this->create(playerController, true);

	std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(playerZone);
	for (std::vector<SessionInfo>::iterator it = sess.begin(); it != sess.end(); ++it){
		if (it->addr != playerController){
			Ref<User> other = WorldOnlineUsers->Find(it->addr);
			PlayerObject *p = other->GetPlayer();
			if (p != NULL){ //Player not null
				if (createOthers) p->create(playerController); //Create Others for me
			}
		}
		this->create(it->addr); //Create me for others
	}

	/*typedef std::map<SystemAddress, ZoneId>::iterator PlayerIterator;
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
	}*/
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
	Logger::log("REPL", "PLAYER", "Send construction of" + UtfConverter::ToUtf8(this->name) + " to " + std::string(systemAddress.ToString()));
	replicaPacketGeneral(outBitStream, REPLICA_CONSTRUCTION_PACKET, this->objid, this->name);
	this->writeToPacket(outBitStream, REPLICA_CONSTRUCTION_PACKET);
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
	this->writeToPacket(outBitStream, REPLICA_SERIALIZATION_PACKET);
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult PlayerObject::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress){
	//TODO implement deserialize once the Serialization isn't static any more
	std::cout << "[WRLD] CLIENT SENT SERIALIZE" << std::endl;
	return REPLICA_PROCESSING_DONE;
}