#include "IniReader.h"

#include <algorithm>
#include <fstream>
#include <sstream>

const bool IniSection::getBoolValue(std::string key, bool defaultValue){
	if (this == NULL) return defaultValue;
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
	if (this == NULL) return defaultValue;
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
	if (this == NULL) return defaultValue;
	std::unordered_map<std::string, std::string>::iterator it = this->values.find(key);
	if (it != this->values.end()){
		return it->second;
	}
	else{
		return defaultValue;
	}
}

void IniSection::addPair(std::string key, std::string value){
	this->values.insert(std::make_pair(key, value));
}

IniSection::IniSection(){

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
		this->baseSection = new IniSection();
		IniSection * section = this->baseSection;
		unsigned int sstart = 0;
		unsigned int lasts = 0;
		std::string section_name = "";
		std::string key = "";

		for (unsigned int k = 0; k < r.size(); k++){
			int n = r.at(k);
			switch (mode){
			case 3:
				if (r.at(k) == 10 && r.at(k) == 13){
					break;
				}
				mode = 0;
			case 0:
				//Start of Line
				//91 - [
				//93 - ]
				//35 - #
				if (r.at(k) > 32 && r.at(k) < 127){
					if (r.at(k) == 91){
						//Start of section
						sstart = k + 1;
						mode = 1;
					}
					else {
						if (r.at(k) == 35){
							//Comment
						}else{
							sstart = k;
							lasts = k;
							mode = 4;
						}
					}
				}
				break;
			case 1:
				if (r.at(k) == 93){
					//End of section name
					section_name = std::string(r.begin() + sstart, r.begin() + k);
					section = new IniSection();
					this->sections.insert(std::make_pair(section_name, section));
					mode = 2;
				}
				if (n == 10 || n == 13){
					//section name has no end, ignoring
				}
				break;
			case 2:
				if (n == 10 || n == 13){
					mode = 3;
				}
				break;
			case 4:
				if (r.at(k) > 32 && r.at(k) < 127){
					if (r.at(k) == 61){
						key = std::string(r.begin() + sstart, r.begin() + lasts + 1);
						sstart = 0;
						mode = 5;
					}else{
						lasts = k;
					}
				}
				if (r.at(k) == 10 || r.at(k) == 13){
					sstart = 0;
					lasts = 0;
					mode = 3;
				}
				break;
			case 5:
				if (r.at(k) == 10 || r.at(k) == 13){
					if (sstart > 0){
						std::string value = std::string(r.begin() + sstart, r.begin() + lasts + 1);
						section->addPair(key, value);
					}
					mode = 3;
				}
				if (r.at(k) > 32 && r.at(k) < 127){
					if (sstart == 0) sstart = k;
					lasts = k;
				}
			}
		}

		if (mode == 5){
			//If there is no line break at the end of the file, we might need to still write the value
			if (sstart > 0){
				std::string value = std::string(r.begin() + sstart, r.begin() + lasts + 1);
				section->addPair(key, value);
			}
		}
	}
}

IniSection * IniFile::getSection(std::string section){
	std::unordered_map<std::string, IniSection *>::iterator it = this->sections.find(section);
	if (it != this->sections.end()){
		return it->second;
	}
	else{
		return NULL;
	}
}

bool IniFile::isValid(){
	return this->valid;
}

IniFile::~IniFile(){
	for (auto it : this->sections){
		if (it.second != NULL){
			delete it.second;
			it.second = NULL;
		}
	}
	if (this->baseSection != NULL){
		delete this->baseSection;
		this->baseSection = NULL;
	}
}