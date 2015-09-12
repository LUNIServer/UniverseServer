#include "IniReader.h"

#include <algorithm>
#include <fstream>
#include <sstream>

const bool IniSection::getBoolValue(std::string key, bool defaultValue){
	std::unordered_map<std::string, std::string>::iterator it = this->values.find(key);
	if (it != this->values.end()){
		std::string data = it->second;
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		return (data == "true");
	}else{
		return defaultValue;
	}
}

const int IniSection::getIntValue(std::string key, int defaultValue){
	std::unordered_map<std::string, std::string>::iterator it = this->values.find(key);
	if (it != this->values.end()){
		try {
			return std::stoi(it->second);
		}
		catch (const std::invalid_argument& ia){
			ia.what();
			return defaultValue;
		}
	}else{
		return defaultValue;
	}
}

const std::string IniSection::getStringValue(std::string key, std::string defaultValue){
	std::unordered_map<std::string, std::string>::iterator it = this->values.find(key);
	if (it != this->values.end()){
		return it->second;
	}
	else{
		return defaultValue;
	}
}

IniFile::IniFile(std::string filename){
	this->valid = false;
	std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate); // ios::ate sets inital position at end of file

	// If the file is open for writing, continue
	if (file.is_open()) {
		// Get the current position of the current character in the input stream
		std::streamoff fsz = file.tellg();

		// Initialize a std::vector<unsigned char> to store packet data (allocating the size of the file)
		std::vector<unsigned char> r((unsigned int)fsz);

		// Go to the beginning of the file
		file.seekg(0, std::ios::beg);

		// Read the data of the packet
		file.read((char*)r.data(), fsz);

		// Close the connection to the file
		file.close();

		// Return the std::vector<unsigned char> with the packet data
		this->valid = true;

		unsigned char mode = 0;
		IniSection * section = NULL;
		unsigned int sstart = 0;
		std::string section_name = "";

		for (unsigned int k = 0; k < r.size(); k++){
			switch (mode){
			case 0:
				//91 - [
				//93 - ]
				//35 - #
				if (r.at(k) > 32 && r.at(k) < 127){
					if (r.at(k) == 91){
						//Start of section
						sstart = k + 1;
						mode = 1;
					}
					if (r.at(k) == 35){
						//Comment
					}
				}
				break;
			case 1:
				if (r.at(k) == 93){
					//End of section name
					section_name = std::string(r.begin() + sstart, r.begin() + k - 1);
					section = new IniSection();
					this->sections.insert(std::make_pair(section_name, section));
				}
				if (r.at(k) == 10 || r.at(k) == 13){
					//section name has no end, ignoring
				}
				break;
			case 2:

				break;
			}
		}
	}
}