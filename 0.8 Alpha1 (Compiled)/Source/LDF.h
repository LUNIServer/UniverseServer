#pragma once
#include "RakNet\BitStream.h"
#include "Common.h"

struct LDF{
	RakNet::BitStream *stream;
	unsigned long keynum;

	LDF(){
		stream = new RakNet::BitStream();
		keynum = 0;
	}

	void writeFLOAT(std::wstring key, float data){ //3
		writeWString(key, this->stream, true);
		this->stream->Write((unsigned char)3);
		this->stream->Write(data);
		keynum++;
	}

	void writeS32(std::wstring key, long data){ //1
		writeWString(key, this->stream, true);
		this->stream->Write((unsigned char)1);
		this->stream->Write(data);
		keynum++;
	}

	void writeS64(std::wstring key, long long data){ //8
		writeWString(key, this->stream, true);
		this->stream->Write((unsigned char)8);
		this->stream->Write(data);
		keynum++;
	}

	void writeID(std::wstring key, long long data){ //9
		writeWString(key, this->stream, true);
		this->stream->Write((unsigned char)9);
		this->stream->Write(data);
		keynum++;
	}

	void writeBOOL(std::wstring key, bool data){ //7
		writeWString(key, this->stream, true);
		this->stream->Write((unsigned char)7);
		if (data) this->stream->Write((unsigned char)1); else this->stream->Write((unsigned char)0);
		keynum++;
	}

	void writeWSTRING(std::wstring key, std::wstring data){ //0
		writeWString(key, this->stream, true);
		this->stream->Write((unsigned char)0);
		this->stream->Write((unsigned long)data.size());
		writeWString(data, this->stream, false);
		keynum++;
	}

	void writeBYTES(std::wstring key, RakNet::BitStream *data){ //13
		writeWString(key, this->stream, true);
		this->stream->Write((unsigned char)13);
		this->stream->Write((unsigned long)data->GetNumberOfBytesUsed());
		this->stream->Write(data, data->GetNumberOfBytesUsed() * 8);
		keynum++;
	}

	void writeToPacket(RakNet::BitStream *packet){
		packet->Write(keynum);
		packet->Write(stream, stream->GetNumberOfBytesUsed() * 8);
	}

	unsigned long getSize(){
		return (unsigned long)(stream->GetNumberOfBytesUsed() + 4);
	}

	~LDF(){
		delete stream;
	}
};