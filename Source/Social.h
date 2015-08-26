#pragma once
#include "CharactersDB.h"
#include "RakNet\RakNetTypes.h"
#include <string>

enum FriendRequestResponse : unsigned char{
	FRIEND_ACCEPTED = 0,
	FRIEND_DECLINED = 1,
	FRIEND_CLOSED = 3
};

class Friends{
	static void broadcastFriendUpdate(long long charid, unsigned char type);
public:
	static std::pair<bool, std::string> sendFriendRequest(const std::string& sender, const std::string& reciever, bool bestFriendsRequest = false);
	static void sendFriendRequestResponse(long long requester, std::string reciever);
	static bool sendFriendRequestResponse(ListCharacterInfo tell, ListCharacterInfo about, std::string status);
	static void informRequester(ListCharacterInfo tell, ListCharacterInfo about, std::string status);

	static void getFriendRequest(std::string requester, long long charid, bool isBest = false);

	static void sendFriendsList(long long charid);
	static void broadcastFriendLogin(long long charid);
	static void broadcastFriendLogout(long long charid);
	static void broadcastFriendWorldChange(long long charid);

	static void handleWorldJoin(long long charid);
	static void checkFriendRequests(long long charid);
	static void checkFriendResponses(long long charid);

	static void handleFriendRequestResponse(long long responder, std::wstring name, unsigned char response);
};

class Chat{
public:
	static void sendChatMessage(SystemAddress addr, std::wstring message, std::wstring sender = L"", bool isMythran = false);
	static void sendChatMessage(long long reciever, std::wstring message, std::wstring sender = L"", bool isMythran = false);
	static void broadcastChatMessage(unsigned short zone, std::wstring message, std::wstring sender = L"", bool isMythran = false);
	static void sendMythranInfo(long long reciever, std::string message, std::string title);
};

class Mail{
public:
	static void openMailbox(long long charid);
	static void closeMailbox(long long charid);
	static void loadMailboxData(long long charid);
	static void sendMailSendResponse(long long charid, long status);
	static void requestMailSending(long long sender, std::wstring subject, std::wstring text, std::wstring recipient, long long attachment = 0, int attcount = 0);
	//For client initiated mail please use #requestMailSending()
	static void postMail(std::string sender, std::string subject, std::string text, long long recipient, long long attachment, int attcount);
	static void deleteMail(long long mailid, long long charid);
	static void removeAttachment(long long mailid, long long charid);
};