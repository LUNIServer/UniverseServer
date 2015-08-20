#include "CharPackets.h"
#include "serverLoop.h"
#include "Packet.h"
#include "Database.h"
#include "InventoryDB.h"
#include "CharactersDB.h"
#include "AccountsDB.h"
#include "User.h"

#include "UtfConverter.h"
#include "Logger.h"

#include "SUtil\Ref.h"

#include "RakNet\BitStream.h"

#include <algorithm>

using namespace std;

void SendCharPacket(RakPeerInterface *rakServer, SystemAddress& systemAddress, unsigned int accountID) {
	RakNet::BitStream bitStream;

	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 6, &bitStream);
	
	Logger::log("CHAR", "PACKETS", "Account ID " + std::to_string(accountID));
	long long frontCharId = AccountsTable::getFrontChar(accountID);

	std::vector<long long> chars = CharactersTable::getCharacters(accountID);
	unsigned char charactersLength = ((chars.size() < 4) ? chars.size() : 4);

	unsigned char frontChar = 0;

	for (int k = 0; k < charactersLength; k++){
		if (chars.at(k) == frontCharId){
			frontChar = k;
		}
	}

	bitStream.Write(charactersLength);
	bitStream.Write(frontChar);

	Logger::log("CHAR", "PACKETS", "#characters: " + std::to_string(charactersLength));
	Logger::log("CHAR", "PACKETS", "1characters: " + std::to_string(frontChar));

	// This loop used to go through 4 chars but doesn't anymore
	for (uint i = 0; i < charactersLength; i++) {
		CharactersPacket charData = CharactersPacket();
		ListCharacterInfo ci = CharactersTable::getCharacterInfo(chars.at(i));

		if (ci.info.objid == 0){
			//Should never happen, old message, means nothing basically
			std::cout << "[CHAR] Char " << std::to_string(i) << " doesn't exist yet." << std::endl;
		}else{
			Logger::log("CHAR", "PACKETS", "Fetching character...");
			Logger::log("CHAR", "PACKETS", "Name is: " + ci.info.name);
			Logger::log("CHAR", "PACKETS", "Unapproved Name is: " + ci.info.unapprovedName);

			charData.objectID = ci.info.objid;
			charData.unknown1 = 0;
			charData.charName = StringToWString(ci.info.name, 66);
			charData.unapprovedName = StringToWString(ci.info.unapprovedName, 66);
			charData.nameRejected = ci.info.nameRejected ? 1 : 0;
			charData.freeToPlay = ci.info.isFreeToPlay ? 1 : 0;

			charData.unknown2 = "";
			charData.shirtColor = ci.style.shirtColor;
			charData.shirtStyle = ci.style.shirtStyle;
			charData.pantsColor = ci.style.pantsColor;
			charData.hairStyle = ci.style.hairStyle;
			charData.hairColor = ci.style.hairColor;
			charData.lh = ci.style.lh;
			charData.rh = ci.style.rh;
			charData.eyebrows = ci.style.eyebrows;
			charData.eyes = ci.style.eyes;
			charData.mouth = ci.style.mouth;
			charData.unknown4 = 0;

			charData.lastZoneId = ci.lastPlace.zoneID;
			charData.mapInstance = ci.lastPlace.mapInstance;
			charData.mapClone = ci.lastPlace.mapClone;
			charData.lastLogout = 0;

			charData.WriteDataToBitstream(&bitStream, i);

			GetCharSpecialItems(charData.objectID, &bitStream);
		}
	}

	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
	SavePacket("char_creation.bin", (char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
}

void SendDeletePacket(RakPeerInterface *rakServer, SystemAddress& systemAddress, Ref<User> user, uchar *packetData, uint length) {
	if (length < 8) return; // If packet data is less than 8 bytes long (counting byte 0), then return as it only contains the packet header

	// User ID stringstream
	stringstream usrId;
	usrId << user->GetID();

	// Get the User ID into a string
	string usrIDString = usrId.str();

	// Get the objectID from the packets
	unsigned long long objectID;
	memcpy(&objectID, packetData + 0x08, 8);

	cout << "Deleting character with ID: " << objectID << " from account: " << usrIDString << endl;

	// Query to delete char from database
	//stringstream query;
	//query << "DELETE FROM `characters` WHERE `objectID` = '" << objectID << "';";
	CharactersTable::deleteCharacter(objectID);

	uchar index = 0; //New Char
	std::vector<uchar> chars = CharactersTable::getCharacterIndices(user->GetID());
	if (chars.size() > 0){
		index = chars.at(0);
	}

	// Query to subtract one character from user
	//stringstream query2;
	//query2 << "UPDATE `accounts` SET `numChars` = '" << usrNumChars - 1 << "' WHERE `id` = '" << usrIDString << "';";
	//AccountsTable::setNumChars(CharacterOwner(user->GetID(), chars.size()));

	// Change front character to 0
	//stringstream query3;
	//query3 << "UPDATE `accounts` SET `frontChar` = '0' WHERE `id` = '" << usrIDString << "';";
	//CharacterOwner o(user->GetID(),index);
	AccountsTable::setFrontChar(0);

	// Query the database with the above 3 queries...
	//Database::Query(query.str()); // Delete char...
	//Database::Query(query2.str()); // ... Update account numChars...
	//Database::Query(query3.str()); // ... and finish by setting front char to 0

	EquipmentTable::deleteEquipment(objectID); //Delete Equipment entries
	InventoryTable::deleteInventory(objectID); //Delete inventory entries

	// Create the packet that will be used to send that char deletion
	// packet to the client
	RakNet::BitStream bitStream;

	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 12, &bitStream);
	bitStream.Write((uchar)1); // Success?

	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

ulong FindCharShirtID(ulong shirtColor, ulong shirtStyle) {
	ulong shirtID = 0;

	// This is a switch case to determine the base shirt color (IDs from CDClient.xml)
	switch (shirtColor) {
	case 0:
	{
			  shirtID = shirtStyle >= 35 ? 5730 : SHIRT_BRIGHT_RED;
			  break;
	}

	case 1:
	{
			  shirtID = shirtStyle >= 35 ? 5736 : SHIRT_BRIGHT_BLUE;
			  break;
	}

	case 3:
	{
			  shirtID = shirtStyle >= 35 ? 5808 : SHIRT_DARK_GREEN;
			  break;
	}

	case 5:
	{
			  shirtID = shirtStyle >= 35 ? 5754 : SHIRT_BRIGHT_ORANGE;
			  break;
	}

	case 6:
	{
			  shirtID = shirtStyle >= 35 ? 5760 : SHIRT_BLACK;
			  break;
	}

	case 7:
	{
			  shirtID = shirtStyle >= 35 ? 5766 : SHIRT_DARK_STONE_GRAY;
			  break;
	}

	case 8:
	{
			  shirtID = shirtStyle >= 35 ? 5772 : SHIRT_MEDIUM_STONE_GRAY;
			  break;
	}

	case 9:
	{
			  shirtID = shirtStyle >= 35 ? 5778 : SHIRT_REDDISH_BROWN;
			  break;
	}

	case 10:
	{
			   shirtID = shirtStyle >= 35 ? 5784 : SHIRT_WHITE;
			   break;
	}

	case 11:
	{
			   shirtID = shirtStyle >= 35 ? 5802 : SHIRT_MEDIUM_BLUE;
			   break;
	}

	case 13:
	{
			   shirtID = shirtStyle >= 35 ? 5796 : SHIRT_DARK_RED;
			   break;
	}

	case 14:
	{
			   shirtID = shirtStyle >= 35 ? 5802 : SHIRT_EARTH_BLUE;
			   break;
	}

	case 15:
	{
			   shirtID = shirtStyle >= 35 ? 5808 : SHIRT_EARTH_GREEN;
			   break;
	}

	case 16:
	{
			   shirtID = shirtStyle >= 35 ? 5814 : SHIRT_BRICK_YELLOW;
			   break;
	}

	case 84:
	{
			   shirtID = shirtStyle >= 35 ? 5820 : SHIRT_SAND_BLUE;
			   break;
	}

	case 96:
	{
			   shirtID = shirtStyle >= 35 ? 5826 : SHIRT_SAND_GREEN;
			   shirtColor = 16;
			   break;
	}
	}

	// Initialize another variable for the shirt color
	ulong editedShirtColor = shirtID;
	
	// This will be the final shirt ID
	ulong shirtIDFinal;

	// For some reason, if the shirt color is 35 - 40,
	// The ID is different than the original... Was this because
	// these shirts were added later?
	if (shirtStyle >= 35) {
		shirtIDFinal = editedShirtColor += (shirtStyle - 35);
	}
	else {
		// Get the final ID of the shirt by adding the shirt
		// style to the editedShirtColor
		shirtIDFinal = editedShirtColor += (shirtStyle - 1);
	}

	//cout << "Shirt ID is: " << shirtIDFinal << endl;

	return shirtIDFinal;
}

ulong FindCharPantsID(ulong pantsColor) {
	ulong pantsID = 2508;

	// This is a switch case to determine 
	// the pants color (IDs from CDClient.xml)
	switch (pantsColor) {
	case 0:
	{
			  pantsID = PANTS_BRIGHT_RED;
			  break;
	}

	case 1:
	{
			  pantsID = PANTS_BRIGHT_BLUE;
			  break;
	}

	case 3:
	{
			  pantsID = PANTS_DARK_GREEN;
			  break;
	}

	case 5:
	{
			  pantsID = PANTS_BRIGHT_ORANGE;
			  break;
	}

	case 6:
	{
			  pantsID = PANTS_BLACK;
			  break;
	}

	case 7:
	{
			  pantsID = PANTS_DARK_STONE_GRAY;
			  break;
	}

	case 8:
	{
			  pantsID = PANTS_MEDIUM_STONE_GRAY;
			  break;
	}

	case 9:
	{
			  pantsID = PANTS_REDDISH_BROWN;
			  break;
	}

	case 10:
	{
			   pantsID = PANTS_WHITE;
			   break;
	}

	case 11:
	{
			   pantsID = PANTS_MEDIUM_BLUE;
			   break;
	}

	case 13:
	{
			   pantsID = PANTS_DARK_RED;
			   break;
	}

	case 14:
	{
			   pantsID = PANTS_EARTH_BLUE;
			   break;
	}

	case 15:
	{
			   pantsID = PANTS_EARTH_GREEN;
			   break;
	}

	case 16:
	{
			   pantsID = PANTS_BRICK_YELLOW;
			   break;
	}

	case 84:
	{
			   pantsID = PANTS_SAND_BLUE;
			   break;
	}

	case 96:
	{
			   pantsID = PANTS_SAND_GREEN;
			   break;
	}
	}

	// Print the pants ID
	//cout << "Pants ID is: " << pantsID << endl;

	return pantsID;
}

void GetCharSpecialItems(long long objectID, RakNet::BitStream *bitStream) {
	std::vector<long long> items = EquipmentTable::getItems(objectID);
	ushort numrows = items.size();
	bitStream->Write(numrows);
	for (ushort k = 0; k < numrows; k++){
		long lot = ObjectsTable::getTemplateOfItem(items.at(k));
		if (lot == -1) lot = 0;
		bitStream->Write(lot);
		Logger::log("CHAR", "PACKETS", "equipped item: " + std::to_string(items.at(k)));
	}
	Logger::log("CHAR", "PACKETS", "User has " + std::to_string(numrows) + " items equipped");
}

string GetUnapprovedUsername(ulong firstLine, ulong middleLine, ulong lastLine) {
	// First name
	fstream firstNameFile(".//names//minifigname_first.txt");
	GoToLine(firstNameFile, firstLine + 1);

	string firstName;
	firstNameFile >> firstName;
	//cout << endl << "First name is: " << firstName << endl;

	// Middle Name
	fstream middleNameFile(".//names//minifigname_middle.txt");
	GoToLine(middleNameFile, middleLine + 1);

	string middleName;
	middleNameFile >> middleName;
	//cout << "Middle name is: " << middleName << endl;

	// Last Name
	fstream lastNameFile(".//names//minifigname_last.txt");
	GoToLine(lastNameFile, lastLine + 1);

	string lastName;
	lastNameFile >> lastName;
	//cout << "Last name is: " << lastName << endl << endl;

	stringstream unapprovedName;
	unapprovedName << firstName << middleName << lastName;

	//cout << "File unapproved char name is: " << unapprovedName.str() << endl;

	return unapprovedName.str();
}


// ----- FILE METHODS ----- //

#ifdef max
#pragma push_macro("max")
#undef max
#define _restore_max_
#endif

fstream& GoToLine(fstream& file, ulong num) {
	file.seekg(std::ios::beg);
	for (uint i = 0; i < num - 1; ++i) {
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return file;
}

#ifdef _restore_max_
#pragma pop_macro("max")
#undef _restore_max_
#endif
