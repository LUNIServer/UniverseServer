#include "Common.h"
#include "SUtil\Kbhit.h"
#include <iomanip>

using namespace std;

// This converts raw hex data to a readable ASCII string
string RawDataToString(uchar* data, uint size, bool onlyraw, ulong start) {
	// Initialize the ostringstream buffer
	ostringstream buffer;

	// If onlyraw is false, print "Data in bytes: "
	if (!onlyraw ) buffer << "Data in bytes: ";

	// Copy the data into the stringstream
	for (uint i = start; i < size; i++) {
		if (!onlyraw) if ((i-start) % 16 == 0) buffer << "\n\t\t";
		else if (!onlyraw) buffer << " ";
		buffer << setw(2) << hex << setfill('0') << (int)data[i];
	}

	if (!onlyraw) buffer << "\n\n";

	// Return the stringstream as a std::string
	return buffer.str();
}

// Server QuitError
void QuitError(const string& msg) {
	cout << "Terminated " << msg << endl << "PRESS ENTER TO QUIT" << endl;
	while (!_kbhit());
	exit(1);
}

void writeWString(std::wstring str, RakNet::BitStream *stream, bool writeSize, bool nullChar){
	if (nullChar){
		str.append(L"\0");
	}
	if (writeSize) stream->Write((uchar) (str.size()*2));
	for (uint k = 0; k < str.size(); k++){
		stream->Write(str.at(k));
	}
}

bool startsWith(std::wstring haystack, std::wstring needle){
	if (haystack.size() < needle.size()){
		return false;
	}
	std::wstring a = haystack.substr(0, needle.size());
	return (a.compare(needle) == 0);
}

bool endsWith(std::wstring haystack, std::wstring needle){
	if (haystack.size() < needle.size()){
		return false;
	}
	std::wstring a = haystack.substr(haystack.size() - needle.size(), needle.size());
	return (a.compare(needle) == 0);
}

std::wstring removeRight(std::wstring str, unsigned int num){
	if (num > str.size()) return L"";
	return str.substr(0, str.size() - num);

}

std::wstring removeLeft(std::wstring str, unsigned int num){
	if (num > str.size()) return L"";
	return str.substr(num, str.size() - num);
}

std::wstring remove(std::wstring str, unsigned int left, unsigned int right){
	if ((left + right) > str.size()) return L"";
	return str.substr(left, str.size() - left - right);
}