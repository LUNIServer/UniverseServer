#include "Character.h"
#include "Database.h"
#include "CharactersDB.h"
#include <sstream>
using namespace std;

// This is the character initialization by the Character ID
Character::Character(long long charid) {
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(charid);
	
	this->Name = std::vector<unsigned char>(cinfo.info.name.begin(), cinfo.info.name.end());
	this->Name.push_back(0);

	// Copy the world into the stringstream
	this->Data.lastZone = static_cast<ZoneId>(cinfo.lastPlace.zoneID);

	this->pos.x = cinfo.lastPlace.x;
	this->pos.y = cinfo.lastPlace.y;
	this->pos.z = cinfo.lastPlace.z;

	this->level = 6; //TODO: dynamic
	this->charobjid = charid;
		
	this->Data.minifig.shirtcolor = cinfo.style.shirtColor;
	this->Data.minifig.pants = cinfo.style.pantsColor;
	this->Data.minifig.hairstyle = cinfo.style.hairStyle;
	this->Data.minifig.haircolor = cinfo.style.hairColor;
	this->Data.minifig.eyebrows = cinfo.style.eyebrows;
	this->Data.minifig.eyes = cinfo.style.eyes;
	this->Data.minifig.mouth = cinfo.style.mouth;
}

// This is the character initialization by a packet
Character::Character(const uchar* packet) {
	// Initialize an int for counting
	int i = 0;

	// Reading name
	while (true) {
		// Add the next byte into the std::vector<uchar> Name
		Name.push_back(packet[i++]);

		// If the last two bytes are 0x00 0x00 (which means there is no
		// more characters in the name), then break from the loop
		if (packet[++i] == 0) break;
	}
	
	// Add the NULL terminator (0x00) to the end of the name
	Name.push_back(0);

	// Add one more to i
	i++;

	//cout << "\n Size: " << size << " CharacterData size: " << sizeof(CharacterData) << endl;

	// Copy the data into memory
	memcpy(&Data, packet + i, sizeof(CharacterData));
}

// Get the packet size (char name * 2 + the size of the character data)
uint Character::GetGeneratedPacketSize() { return Name.size() * 2 + sizeof(CharacterData); }

// Serialize the character
uchar* Character::Serialize() {
	// Allocate the size generated above
	uchar * r = (uchar*) malloc( GetGeneratedPacketSize() );

	// If r is equal to NULL, return NULL
	if (r == NULL) return NULL;

	// Initialize uint
	uint i = 0;

	// Copy the name into r
	for (uint j = 0; j < Name.size(); i++, j++) {
		r[i] = Name[j]; // Copy letter
		r[++i] = 0; // Next byte is 0x00 (the name is actually a wstring, not a std::string)
	}
	memcpy(r + i, &Data, sizeof(CharacterData)); // Copy the data into memory

	// Return the data as a uchar*
	return r;
}

// Get the character name
string Character::GetName() { return (char*) Name.data(); }