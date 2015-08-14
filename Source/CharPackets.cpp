#include "CharPackets.h"
#include "serverLoop.h"
#include "Packet.h"
#include "Database.h"
#include "InventoryDB.h"
#include "CharactersDB.h"
#include "AccountsDB.h"
#include "User.h"

#include "UtfConverter.h"

#include "SUtil\Ref.h"

#include "RakNet\BitStream.h"

using namespace std;

void SendCharPacket(RakPeerInterface *rakServer, SystemAddress& systemAddress, Ref<User> usr) {
	RakNet::BitStream bitStream;

	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 6, &bitStream);
	
	if (usr == NULL) {
		cout << "ERROR: User is null!" << endl;
	}

	uchar charactersLength = (uchar)usr->userChars;
	uchar frontChar = (uchar)usr->userFrontChar;

	bitStream.Write(charactersLength);
	bitStream.Write(frontChar);
	std::cout << "[GAME] [CHAR] #characters: " << std::to_string(charactersLength) << std::endl;
	std::cout << "[GAME] [CHAR] 1characters: " << std::to_string(frontChar) << std::endl;

	uint accountID = usr->GetID();
	std::cout << "[CHAR] Account id: " << std::to_string(accountID) << std::endl;

	// This loop perhaps is very inefficient, but it makes sure
	// 4 chars are loaded (or, if they don't exist, fill with 0x00)
	for (uint i = 0; i <= charactersLength; i++) {
		CharactersPacket charData = CharactersPacket();

		CharacterOwner o(accountID,i);

		ListCharacterInfo ci = CharactersTable::getCharacterInfo(o);

		if (ci.info.objid == 0){
			std::cout << "[CHAR] Char " << std::to_string(o.charIndex) << " doesn't exist yet." << std::endl;
		}else{
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

	// Get number of characters the user has
	uchar usrNumChars = user->userChars;

	uchar index = 0; //New Char
	std::vector<uchar> chars = CharactersTable::getCharacterIndices(user->GetID());
	if (chars.size() > 0){
		index = chars.at(0);
	}

	// Query to subtract one character from user
	//stringstream query2;
	//query2 << "UPDATE `accounts` SET `numChars` = '" << usrNumChars - 1 << "' WHERE `id` = '" << usrIDString << "';";
	AccountsTable::setNumChars(CharacterOwner(user->GetID(), chars.size()));

	// Change front character to 0
	//stringstream query3;
	//query3 << "UPDATE `accounts` SET `frontChar` = '0' WHERE `id` = '" << usrIDString << "';";
	CharacterOwner o(user->GetID(),index);
	AccountsTable::setFrontChar(o);

	// Query the database with the above 3 queries...
	//Database::Query(query.str()); // Delete char...
	//Database::Query(query2.str()); // ... Update account numChars...
	//Database::Query(query3.str()); // ... and finish by setting front char to 0

	EquipmentTable::deleteEquipment(objectID); //Delete Equipment entries
	InventoryTable::deleteInventory(objectID); //Delete inventory entries

	// Set local user's numChars and frontChar
	user->userChars = usrNumChars - 1;
	user->userFrontChar = index;

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

	cout << "Shirt ID is: " << shirtIDFinal << endl;

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
	cout << "Pants ID is: " << pantsID << endl;

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
		std::cout << "[CHAR] equipped item: " << items.at(k) << std::endl;
	}
	std::cout << "[CHAR] User has " << numrows << " items equipped" << std::endl;
}

void AddCharToDatabase(RakPeerInterface *rakServer, SystemAddress& systemAddress, uchar *packetData, uint length, Ref<User> usr) {
	if (length < 8) return;

	// Get the ID out the user logged in (from User.h)
	stringstream usrId;
	usrId << usr->GetID();

	// Convert it to a string to use with the queries.
	string usrIDString = usrId.str();

	// Create the variables to hold the char items
	ulong shirtColor;
	ulong shirtStyle;
	ulong pantsColor;
	ulong hairStyle;
	ulong hairColor;
	ulong lh;
	ulong rh;
	ulong eyebrows;
	ulong eyes;
	ulong mouth;

	// Copy the char items from the creation packet
	// to their variables
	memcpy(&shirtColor, packetData + 0x5F, 4);
	memcpy(&shirtStyle, packetData + 0x63, 4);
	memcpy(&pantsColor, packetData + 0x67, 4);
	memcpy(&hairStyle, packetData + 0x6B, 4);
	memcpy(&hairColor, packetData + 0x6F, 4);
	memcpy(&lh, packetData + 0x73, 4);
	memcpy(&rh, packetData + 0x77, 4);
	memcpy(&eyebrows, packetData + 0x7B, 4);
	memcpy(&eyes, packetData + 0x7F, 4);
	memcpy(&mouth, packetData + 0x83, 4);

	// Print to the console the char items... (these
	// will also be in the database to view later)
	cout << "Shirt color is: " << shirtColor << endl;
	cout << "Shirt style is: " << shirtStyle << endl;
	cout << "Pants color is: " << pantsColor << endl;
	cout << "Hair style is: " << hairStyle << endl;
	cout << "Hair color is: " << hairColor << endl;
	cout << "LH is: " << lh << endl;
	cout << "RH is: " << rh << endl;
	cout << "Eyebrows are: " << eyebrows << endl;
	cout << "Eyes are: " << eyes << endl;
	cout << "Mouth is: " << mouth << endl;

	// Figure out the pants and shirt IDs that the
	// char starts with...
	ulong shirtID = FindCharShirtID(shirtColor, shirtStyle);
	ulong pantsID = FindCharPantsID(pantsColor);
	
	// Initialize username string...
	string username;

	// Read the string from the packet
	int i = 8;
	while (true) {
		username.push_back(packetData[i++]);
		if (packetData[++i] == 0) break;
	}
	username.push_back(0);
	i++;

	// Print the username to the console
	cout << "Username is: " << username << endl;

	// Get the line numbers from the packet for the character
	// premade name (in minifigname_first.txt, _middle.txt, and
	// _last.txt)
	ulong firstLineNum;
	ulong middleLineNum;
	ulong lastLineNum;

	// Copy the data into the variables
	memcpy(&firstLineNum, packetData + 0x4A, 4);
	memcpy(&middleLineNum, packetData + 0x4E, 4);
	memcpy(&lastLineNum, packetData + 0x52, 4);

	// Now, figure out the unapproved name and store it as
	// as string
	string unapprovedName = GetUnapprovedUsername(firstLineNum, middleLineNum, lastLineNum);
	

	// Save character into the database
	uchar creationStatus = 0;

	// Get the strings of the username
	string usernameStr = string(username.c_str());
	string unapprovedUserStr = string(unapprovedName.c_str());

	if (usernameStr.size() == 0) usernameStr = unapprovedUserStr;

	// Prepare the first query to check to see if username
	// already exists
	//stringstream query1;
	//query1 << "SELECT `id` FROM `characters` WHERE `name` = '" << usernameStr << "' LIMIT 1;";
	long long extobjid = CharactersTable::getObjidFromCharacter(usernameStr);

	//cout << "Query is: " << query1.str() << endl;

	// Query the database...
	//auto qr = Database::Query(query1.str());

	// If the username does not exist, mysql_num_rows(qr) should
	// equal 0. Otherwise, return the error to the client
	if (extobjid < 0) {
		// Query the database again to see if the unapproved
		// name already exists...
		
		//auto qr2 = Database::Query("SELECT `unapprovedName` FROM `characters` WHERE `unapprovedName` = '" + unapprovedName + "';");
		
		// If the unapproved name does not exist, mysql_num_rows(qr)
		// should equal 0. Otherwise, return the error to the client
		if (!CharactersTable::unapprovedNameExists(unapprovedName)) {
			cout << "Username and predefined username not in use! Storing in database..." << endl;
			
			// Figure out which char ID (NOT objectID) to put the
			// Character in (0 - 3); 0 = first char, 3 = 4th char
			for (uchar i = 1; i <= 4; i++) {
				ostringstream charId;
				charId << i - 1;

				// Get the ID of the character (from 0 - 3)
				string charIDStr = charId.str();
				
				// Query the database to see if character with ID already exists
				auto whichChar = Database::Query("SELECT `id`, `accountID` FROM `characters` WHERE `id` = '" + charIDStr + "' AND `accountID` = '" + usrIDString + "';");
				
				// The ObjectID is basically the unique ID that
				// destingushes all user chars ingame.
				// Create the ObjectID ulonglong

				// Prepare query to insert char into database
				//stringstream query2;
				//query2 << "INSERT INTO `characters` (`id`, `accountID`, `objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, `shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, `shirtID`, `pantsID`, `lastZoneId`) VALUES ('" << charIDStr << "', '" << usrId.str() << "', NULL, '" << usernameStr << "', '" << unapprovedUserStr << "', '0', '0', '" << shirtColor << "', '" << shirtStyle << "', '" << pantsColor << "', '" << hairStyle << "', '" << hairColor << "', '" << lh << "', '" << rh << "', '" << eyebrows << "', '" << eyes << "', '" << mouth << "', '" << shirtID << "', '" << pantsID << "', '1000');";

				// Prepare the increaseChars value
				//stringstream increaseChars;
				//increaseChars << usr->userChars + 1;

				// If the user has less than 4 characters, and
				// i = a charID that does not exists, use that and
				// insert the char in the database.
				if (mysql_num_rows(whichChar) == 0) {
					//cout << "Query to insert is: " << query2.str() << endl;

					//Database::Query(query2.str());

					CharacterInfo info;
					info.name = usernameStr;
					info.unapprovedName = unapprovedUserStr;

					CharacterOwner owner(usr->GetID(), i - 1);

					CharacterStyle style;
					style.shirtColor = shirtColor;
					style.shirtStyle = shirtStyle;
					style.pantsColor = pantsColor;
					style.hairStyle = hairStyle;
					style.hairColor = hairColor;
					style.lh = lh;
					style.rh = rh;
					style.eyebrows = eyebrows;
					style.eyes = eyes;
					style.mouth = mouth;
					
					long long charObjid = CharactersTable::add(style, owner, info);//mysql_insert_id(Database::getConnection());
					AccountsTable::setNumChars(CharacterOwner(usr->GetID(), usr->userChars + 1));
					AccountsTable::setFrontChar(CharacterOwner(usr->GetID(), i - 1));
					//Database::Query("UPDATE `accounts` SET `numChars` = '" + increaseChars.str() + "' WHERE `id` = '" + usrIDString + "';");
					//Database::Query("UPDATE `accounts` SET `frontChar` = '" + charIDStr + "' WHERE `id` = '" + usrIDString + "';");

					//To prepare the starter kit, create a shirtID and a pantsID object
					//Add them to the inventory and equip them
					long long shirtObjid = ObjectsTable::createObject(shirtID);
					InventoryTable::insertItem(charObjid, shirtObjid, 1, 0, true);
					EquipmentTable::equipItem(charObjid, shirtObjid);

					long long pantsObjid = ObjectsTable::createObject(pantsID);
					InventoryTable::insertItem(charObjid, pantsObjid, 1, 1, true);
					EquipmentTable::equipItem(charObjid, pantsObjid);

					usr->userChars = usr->userChars + 1;
					usr->userFrontChar = i - 1;
					usr->nameInUse = 0;
					break;
				}
				else {
					cout << "User with charID " << charId.str() << " already exists for user with id " << usrId.str() << endl;
					continue;
				}
			}
		}
		else {
			cout << "Pre-defined username already in use!" << endl;
			usr->nameInUse = 1;
			creationStatus = 3;
		}
	}
	else {
		cout << "Username already in use!" << endl;
		usr->nameInUse = 1;
		creationStatus = 4;
		if (usernameStr == unapprovedUserStr) creationStatus = 3; //Temp Name used as name
	}

	// Create the bitstream to send to the client with the
	// char create status
	RakNet::BitStream bitStream;
	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 7, &bitStream);

	bitStream.Write(creationStatus);

	// Send the packet
	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

string GetUnapprovedUsername(ulong firstLine, ulong middleLine, ulong lastLine) {
	// First name
	fstream firstNameFile(".//names//minifigname_first.txt");
	GoToLine(firstNameFile, firstLine + 1);

	string firstName;
	firstNameFile >> firstName;
	cout << endl << "First name is: " << firstName << endl;

	// Middle Name
	fstream middleNameFile(".//names//minifigname_middle.txt");
	GoToLine(middleNameFile, middleLine + 1);

	string middleName;
	middleNameFile >> middleName;
	cout << "Middle name is: " << middleName << endl;

	// Last Name
	fstream lastNameFile(".//names//minifigname_last.txt");
	GoToLine(lastNameFile, lastLine + 1);

	string lastName;
	lastNameFile >> lastName;
	cout << "Last name is: " << lastName << endl << endl;

	stringstream unapprovedName;
	unapprovedName << firstName << middleName << lastName;

	cout << "File unapproved char name is: " << unapprovedName.str() << endl;

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
