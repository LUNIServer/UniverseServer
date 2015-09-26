#include "Social.h"
#include "Account.h"
#include "CharactersDB.h"
#include "InventoryDB.h"
#include "WorldServer.h"
#include "Packet.h"
#include "UtfConverter.h"
#include "Logger.h"

#include <iostream>

std::pair<bool, std::string> Friends::sendFriendRequest(const std::string &sender, const std::string& reciever, bool bestFriendsRequest){
	std::pair<bool, std::string> ret(true, "Friends Request posted");
	Logger::log("GAME", "SOCIAL", "Requesting Friend '" + sender + "' -> '" + reciever + "'");
	if (bestFriendsRequest) Logger::log("GAME", "SOCIAL", "This is a best-friend request");

	long long userObjid = CharactersTable::getObjidFromCharacter(sender);
	long long friendObjid = CharactersTable::getObjidFromCharacter(reciever);
	
	if (friendObjid > -1){
		if (friendObjid != userObjid){
			Logger::log("GAME", "SOCIAL", "Friends: " + std::to_string(userObjid) + " & " + std::to_string(friendObjid), LOG_ALL);
			std::string status1 = FriendsTable::getFriendsStatus(userObjid, friendObjid);
			if (status1 == "INVALID"){
				//Not Friends == both have to be invalid
				if (bestFriendsRequest){
					ret.second = "Cannot request best-friends without being friends";
				}
				else{
					FriendsTable::requestFriend(userObjid, friendObjid);
					ret.first = true;
				}
			}
			else{
				std::string status2 = FriendsTable::getRequestStatus(userObjid, friendObjid);
				if (status1 == "REQUEST"){
					if (status2 == "REQUEST"){
						//I am asking ->
						ret.second = "Request already registered";
					}
					else{
						//I try sending somone a friend request that is requesting me
						//TODO: Error and accept
					}
				}
				if (status1 == "FRIENDS" || status1 == "ACCEPTED"){
					if (bestFriendsRequest){
						FriendsTable::requestBestFriend(userObjid, friendObjid);
						ret.first = true;
						ret.second = "Best-Friends Request posted";
					}
					else{
						ret.second = "Already friends";
					}
				}
				if (status1 == "DECLINED"){
					if (status2 == "DECLINED"){
						ret.second = "Request already declined";
					}
					else{
						//I ask someone to whom i have declined the friendship without him seeing it
						//Accept the friendship and continue
					}
				}
				ret.first = false;
			}
		}
		else{
			ret.second = "Cannot be friends with yourself";
			ret.first = false;
		}
	}
	else{
		ret.second = "Character " + reciever + " does not exist";
		ret.first = false;
	}
	Logger::log("GAME", "SOCIAL", ret.second, LOG_ALL);
	return ret;
}

void Friends::sendFriendRequestResponse(long long sender, std::string reciever){
	ListCharacterInfo infoSender = CharactersTable::getCharacterInfo(sender);
	ListCharacterInfo infoReciever = CharactersTable::getCharacterInfo(reciever);

	SessionInfo session = SessionsTable::getClientSession(SessionsTable::findCharacter(infoSender.info.objid));
	
	std::string status = FriendsTable::getRequestStatus(infoSender.info.objid, infoReciever.info.objid);
	bool success = Friends::sendFriendRequestResponse(infoSender, infoReciever, status);

	if (success){
		Logger::log("GAME", "SOCIAL", "Sent Response packet, status: " + status, LOG_DEBUG);
		Logger::log("GAME", "SOCIAL", "Client IP: " + std::string(session.addr.ToString()), LOG_DEBUG);
	}
	else{
		Logger::log("GAME", "SOCIAL", "Could not send Friend Response to player: Player not online", LOG_DEBUG);
	}
}

//Sends a person the information, that their friend status is as such, return true, when client connected
bool Friends::sendFriendRequestResponse(ListCharacterInfo tell, ListCharacterInfo about, std::string status){
	SessionInfo infoTell = SessionsTable::getClientSession(SessionsTable::findCharacter(tell.info.objid));
	if (infoTell.phase > SessionPhase::PHASE_AUTHENTIFIED){
		Logger::log("GAME", "SOCIAL", status + " for friend request between " + tell.info.name + " and " + about.info.name, LOG_DEBUG);

		unsigned char statusid = 3;
		if (status == "FRIENDS") {
			statusid = 1;
		}
		else if (status == "ACCEPTED"){
			statusid = 0;
		}
		else if (status == "DECLINED"){
			statusid = 3;
		}
		else if (status == "INVALID"){
			statusid = 2;
		}
		else if(status == "YOUR_LIST_FULL"){
			statusid = 4;
		}
		else if (status == "HIS_LIST_FULL"){
			statusid = 5;
		}
		else{
			return false;
		}

		RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::FRIEND_REQUEST_RESPONSE);
		bs->Write(statusid);
		unsigned char online = 0;
		SessionInfo infoAbout = SessionsTable::getClientSession(SessionsTable::findCharacter(about.info.objid));
		if (infoAbout.phase > SessionPhase::PHASE_AUTHENTIFIED){
			online = 1;
		}
		bs->Write(online);
		PacketTools::WriteToPacket(bs, UtfConverter::FromUtf8(about.info.name), 33);
		bs->Write(about.info.objid);
		bs->Write((unsigned short)1200); //World id
		bs->Write((unsigned long)0); //Instance
		bs->Write((unsigned short)0);
		bs->Write((unsigned long)0);
		bs->Write((unsigned short)0); //Clone
		bs->Write((unsigned char)0); //Best Friends
		bs->Write((unsigned char)0); //friend FTP
		WorldServer::sendPacket(bs, infoTell.addr);
		return true;
	}
	else{
		return false;
	}
}

void Friends::informRequester(ListCharacterInfo tell, ListCharacterInfo about, std::string status){
	bool f = Friends::sendFriendRequestResponse(tell, about, status);
	if (f){
		//Person has recieved the message
		if (status == "ACCEPTED"){
			FriendsTable::setRequestStatus(tell.info.objid, about.info.objid, "FRIENDS");
		}
		//else if (status == "DECLINED"){} //We could delete the request, but we'll leave it for now
	}
}

void Friends::sendFriendsList(long long charid){
	Logger::log("GAME", "SOCIAL", "Getting friends list for charid: " + std::to_string(charid));
	std::vector<long long> bestFriends = FriendsTable::getBestFriends(charid);
	std::vector<long long> friends = FriendsTable::getFriends(charid);
	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::FRIENDS_LIST);
	bs->Write((unsigned char) 0);
	short bfs = bestFriends.size();
	short friendcount = friends.size() + bfs;
	bs->Write((unsigned short)(96 * friendcount - 1));
	bs->Write((unsigned short)friendcount);
	for (int k = 0; k < friendcount; k++){
		unsigned char isBest = 0;
		long long id = 0;
		if (k < bfs){
			//Is best friend
			id = bestFriends.at(k);
			isBest = 1;
		}
		else{
			id = friends.at(k - bfs);
		}
		SessionInfo info = SessionsTable::getClientSession(SessionsTable::findCharacter(id));
		//unsigned int accountid = CharactersTable::getAccountFromObjid(id);
		ListCharacterInfo l = CharactersTable::getCharacterInfo(id);
		if (info.phase > SessionPhase::PHASE_AUTHENTIFIED){ //Is player online
			bs->Write((unsigned char)1);
		}
		else{
			bs->Write((unsigned char)0);
		}
		bs->Write(isBest); //Best Friends
		if (l.info.isFreeToPlay){
			bs->Write((unsigned char)1);
		}
		else{
			bs->Write((unsigned char)0);
		}
		bs->Write((unsigned char)0); //???
		bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
		bs->Write(l.lastPlace.zoneID);
		bs->Write(l.lastPlace.mapInstance);
		bs->Write(l.lastPlace.mapClone);
		bs->Write(l.info.objid);
		for (unsigned int k = 0; k < 33; k++){
			if (k < l.info.name.size()) bs->Write((unsigned char)l.info.name.at(k));
			else bs->Write((unsigned char)0);
			bs->Write((unsigned char)0);
		}
		Logger::log("GAME", "SOCIAL", "Friend: " + l.info.name, LOG_DEBUG);
		bs->Write((unsigned char)0); //???
		bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
	}
	SessionInfo info2 = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));
	if (info2.phase > SessionPhase::PHASE_AUTHENTIFIED){
		WorldServer::sendPacket(bs, info2.addr);
	}
}

void Friends::broadcastFriendUpdate(long long charid, unsigned char type){
	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::FRIEND_UPDATE);
	bs->Write(type);
	ListCharacterInfo l = CharactersTable::getCharacterInfo(charid);

	for (unsigned int k = 0; k < 33; k++){
		if (k < l.info.name.size()) bs->Write((unsigned char)l.info.name.at(k));
		else bs->Write((unsigned char)0);
		bs->Write((unsigned char)0);
	}
	bs->Write(l.lastPlace.zoneID);
	bs->Write(l.lastPlace.mapInstance);
	bs->Write(l.lastPlace.mapClone);
	bs->Write((unsigned char)0); //Best Friends
	bs->Write((unsigned char)0); //FTP

	std::vector<long long> friends = FriendsTable::getFriends(charid);
	for (unsigned int k = 0; k < friends.size(); k++){
		SessionInfo acc = SessionsTable::getClientSession(SessionsTable::findCharacter(friends.at(k)));
		if (acc.phase > SessionPhase::PHASE_AUTHENTIFIED){
			WorldServer::sendPacket(bs, acc.addr);
		}
	}
}

void Friends::broadcastFriendLogin(long long charid){
	Friends::broadcastFriendUpdate(charid, 1);
}

void Friends::broadcastFriendLogout(long long charid){
	Friends::broadcastFriendUpdate(charid, 0);
}

void Friends::broadcastFriendWorldChange(long long charid){
	Friends::broadcastFriendUpdate(charid, 2);
}

void Friends::handleWorldJoin(long long charid){
	SessionInfo info = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));
	if (info.worldJoin == 1){
		Logger::log("GAME", "SOCIAL", "First world join");
		Friends::sendFriendsList(charid);
		Friends::broadcastFriendLogin(charid);
	}
	else{
		Logger::log("GAME", "SOCIAL", "Following world join");
		Friends::sendFriendsList(charid);
		Friends::broadcastFriendWorldChange(charid);
	}
}

void Friends::checkFriendRequests(long long charid){
	std::vector<long long> fR = FriendsTable::getFriendRequests(charid);
	std::vector<long long> bfR = FriendsTable::getBestFriendRequests(charid);
	bool flag = false;
	if (fR.size() > 0){
		for (unsigned int k = 0; k < fR.size(); k++){
			ListCharacterInfo info = CharactersTable::getCharacterInfo(fR.at(k));
			Friends::getFriendRequest(info.info.name, charid, false);
		}
		flag = true;
	}
	if (bfR.size() > 0){
		for (unsigned int k = 0; k < bfR.size(); k++){
			ListCharacterInfo info = CharactersTable::getCharacterInfo(bfR.at(k));
			Friends::getFriendRequest(info.info.name, charid, true);
		}
	}
	Logger::log("GAME", "SOCIAL", "Queried Friend Requests");
}

void Friends::checkFriendResponses(long long charid){
	std::vector<std::pair<long long, std::string>> responses = FriendsTable::getFriendResponses(charid);
	for (unsigned int k = 0; k < responses.size(); k++){
		std::pair<long long, std::string> pair = responses.at(k);
		ListCharacterInfo requesterI = CharactersTable::getCharacterInfo(charid);
		ListCharacterInfo responderI = CharactersTable::getCharacterInfo(pair.first);
		Friends::informRequester(requesterI, responderI, pair.second);
	}
}

void Friends::getFriendRequest(std::string requester, long long charid, bool isBest){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::FRIEND_REQUEST);
		PacketTools::WriteToPacket(bs, UtfConverter::FromUtf8(requester), 33);
		bs->Write((unsigned char)isBest);
		WorldServer::sendPacket(bs, addr);
	}
}

void Friends::handleFriendRequestResponse(long long responder, std::wstring name, unsigned char response){
	ListCharacterInfo requesterI = CharactersTable::getCharacterInfo(UtfConverter::ToUtf8(name));
	ListCharacterInfo responderI = CharactersTable::getCharacterInfo(responder);
	bool flag = false;
	std::string status;
	if (response == FriendRequestResponse::FRIEND_DECLINED){
		FriendsTable::decline(requesterI.info.objid, responderI.info.objid);
		status = "DECLINED";
		flag = true;
	}
	else if (response == FriendRequestResponse::FRIEND_ACCEPTED){
		FriendsTable::accept(requesterI.info.objid, responderI.info.objid);
		status = "ACCEPTED";
		flag = true;
	}
	else{
		status = "IGNORED";
	}
	Logger::log("GAME", "SOCIAL", "Character " + std::to_string(responder) + " " + status + ": " + UtfConverter::ToUtf8(name), LOG_DEBUG);

	if (flag){
		Friends::sendFriendRequestResponse(responderI, requesterI, status);
		Friends::informRequester(requesterI, responderI, status);
	}
}

void Chat::sendChatMessage(SystemAddress addr, std::wstring message, std::wstring sender, bool isMythran){
	RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CHAT, ChatPacketID::GENERAL_CHAT_MESSAGE);
	unsigned char u8 = 0;
	unsigned short u16 = 0;
	unsigned long u32 = 0;
	unsigned long long u64 = 0;

	aw->Write(u64);
	aw->Write((unsigned char)0x01);
	int len = message.size();
	aw->Write((unsigned char)(len + 1));
	aw->Write(u16);
	aw->Write(u8);

	//Static size of sender name (66 bytes), gets cropped if too long
	for (unsigned int k = 0; k < 33; k++){
		if (k < sender.size()){
			aw->Write(sender.at(k));
		}
		else{
			aw->Write(u16);
		}
	}

	aw->Write((int)1); //sender Object id
	aw->Write(u32);
	aw->Write(u16); //??
	aw->Write((unsigned char)isMythran); //isMythran

	for (unsigned int k = 0; k < message.size(); k++){
		aw->Write(message.at(k));
	}
	aw->Write(u16);
	WorldServer::sendPacket(aw, addr);
}

void Chat::sendChatMessage(long long reciever, std::wstring message, std::wstring sender, bool isMythran){
	SystemAddress addr = SessionsTable::findCharacter(reciever);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo session = SessionsTable::getClientSession(addr);
		//Actually that check is unnecessary since these phases are equivalent to finding a session for a characterid
		if (session.phase > SessionPhase::PHASE_AUTHENTIFIED){
			Chat::sendChatMessage(addr, message, sender, isMythran);
		}
	}
}

void Chat::broadcastChatMessage(unsigned short zone, std::wstring message, std::wstring sender, bool isMythran){
	std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(zone);
	for (unsigned int k = 0; k < sess.size(); k++){
		Logger::log("GAME", "CHAT", "Broadcast to " + std::to_string(sess.at(k).activeCharId), LOG_DEBUG);
		if (sess.at(k).phase > SessionPhase::PHASE_AUTHENTIFIED){
			Chat::sendChatMessage(sess.at(k).addr, message, sender, isMythran);
		}
	}
}

void Chat::sendMythranInfo(long long reciever, std::string message, std::string title){
	SystemAddress addr = SessionsTable::findCharacter(reciever);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
		aw->Write(reciever);
		aw->Write((unsigned short)1184);
		aw->Write((unsigned char)9);
		aw->Write((unsigned char)1);
		WorldServer::writeText(aw, "message");
		aw->Write((unsigned char)6);
		WorldServer::writeText(aw, message);
		WorldServer::writeText(aw, "title");
		aw->Write((unsigned char)6);
		WorldServer::writeText(aw, title);
		WorldServer::writeText(aw, "visible");
		aw->Write((unsigned char)3);
		aw->Write((unsigned char)1);
		WorldServer::writeText2(aw, "ToggleAnnounce");
		WorldServer::savePacket(aw, ".\\world\\mythran_info.bin");
		WorldServer::sendPacket(aw, addr);
	}
}

void Mail::openMailbox(long long charid){
	Logger::log("GAME", "MAIL", "Opening Postbox");
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
		aw->Write(charid);
		aw->Write((unsigned short)1184);
		aw->Write((unsigned char)9);
		aw->Write((unsigned char)1);
		WorldServer::writeText(aw, "state");
		aw->Write((unsigned char)6);
		WorldServer::writeText(aw, "Mail");
		aw->Write((unsigned char)1);
		WorldServer::writeText2(aw, "pushGameState");
		WorldServer::sendPacket(aw, addr);
	}
}

void Mail::closeMailbox(long long charid){
	Logger::log("GAME", "MAIL", "Closing Postbox");
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
		aw->Write(charid);
		aw->Write((unsigned short)1184);
		aw->Write((unsigned char)9);
		aw->Write((unsigned char)1);
		WorldServer::writeText(aw, "visible");
		aw->Write((unsigned char)2);
		aw->Write((unsigned char)1);
		WorldServer::writeText2(aw, "ToggleMail");
		WorldServer::sendPacket(aw, addr);
	}
}

void Mail::loadMailboxData(long long charid){
	Logger::log("GAME", "MAIL", "Loading Mails");
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::MAIL_STUFF);
		aw->Write((unsigned long)4); //Mail id
		aw->Write((unsigned long)0); //return code

		std::vector<MailData> mails = MailsTable::getMails(charid);
		aw->Write((unsigned short)mails.size());
		aw->Write((unsigned short)0);
		for (unsigned short k = 0; k < mails.size(); k++){
			aw->Write(mails.at(k).id);
			PacketTools::WriteToPacket(aw, UtfConverter::FromUtf8(mails.at(k).subject), 50);
			PacketTools::WriteToPacket(aw, UtfConverter::FromUtf8(mails.at(k).text), 400);
			std::wstring s = UtfConverter::FromUtf8(mails.at(k).sender);
			Logger::log("GAME", "MAIL", "Loading ID " + std::to_string(mails.at(k).id) + " FROM " + mails.at(k).sender, LOG_DEBUG);
			PacketTools::WriteToPacket(aw, s, 32);
			aw->Write((unsigned long)0);
			aw->Write((unsigned long long)0);
			long long o = mails.at(k).attachment;
			aw->Write(o);
			long lot = -1;
			if (o > 0){
				ObjectInfo oinfo = ObjectsTable::getItemInfo(o);
				lot = oinfo.lot;
			}
			aw->Write(lot);
			aw->Write((long)0);
			aw->Write((long long)0);
			aw->Write((short)mails.at(k).attachment_count);
			aw->Write((long)0);
			aw->Write((short)0);
			aw->Write((long long)mails.at(k).sent_time);
			aw->Write((long long)mails.at(k).sent_time);
			if (mails.at(k).read) aw->Write((char)1); else aw->Write((char)0);
			aw->Write((char)0);
			aw->Write((short)0);
			aw->Write((long)0);
		}

		WorldServer::sendPacket(aw, addr);
	}
}

void Mail::sendMailSendResponse(long long charid, long status){
	Logger::log("GAME", "MAIL", "Loading Mails");
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::MAIL_STUFF);
		aw->Write((unsigned long)1); //Mail id
		aw->Write(status); //return code

		WorldServer::sendPacket(aw, addr);
	}
}

void Mail::requestMailSending(long long charid, std::wstring subject, std::wstring text, std::wstring recipient, long long attachment, int attcount){
	ListCharacterInfo charinfo = CharactersTable::getCharacterInfo(UtfConverter::ToUtf8(recipient));
	ListCharacterInfo sinfo = CharactersTable::getCharacterInfo(charid);
	if (charinfo.info.objid > 0){
		if (charinfo.info.objid == charid){
			Mail::sendMailSendResponse(charid, 4); //Cannot Mail yourself
		}
		else{
			if (charinfo.info.isFreeToPlay){
				Mail::sendMailSendResponse(charid, 13); //Is free to play
			}
			else{
				Mail::postMail(sinfo.info.name, UtfConverter::ToUtf8(subject), UtfConverter::ToUtf8(text), charinfo.info.objid, attachment, attcount);
				Mail::sendMailSendResponse(charid, 0); //Success
			}
		}
	}
	else{
		Mail::sendMailSendResponse(charid, 5); //recipient not found
	}
}

void Mail::postMail(std::string sender, std::string subject, std::string text, long long recipient, long long attachment, int attcount){
	MailData d;
	d.sender = sender;
	d.recipient = recipient;
	d.subject = subject;
	d.text = text;
	d.attachment = attachment;
	d.attachment_count = attcount;
	MailsTable::addMail(d);
}

void Mail::deleteMail(long long mailid, long long charid){
	MailsTable::deleteMail(mailid);
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::MAIL_STUFF);
		aw->Write((unsigned long)8); //Mail ID
		aw->Write((unsigned long)0); //return code
		aw->Write(mailid);
		Logger::log("GAME", "MAIL", "Send Mail " + std::to_string(mailid) + " delete confirm", LOG_DEBUG);
		WorldServer::sendPacket(aw, addr);
	}
}

void Mail::removeAttachment(long long mailid, long long charid){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		RakNet::BitStream *aw = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::MAIL_STUFF);
		aw->Write((unsigned long)6);
		aw->Write((unsigned long)0);
		aw->Write(mailid);
		Logger::log("GAME", "MAIL", "Send Mail " + std::to_string(mailid) + " attachment remove confirm confirm", LOG_DEBUG);
		WorldServer::sendPacket(aw, addr);
	}
}