#pragma once
#include "RakNet\BitStream.h"
#include "Common.h"

struct LDF{
	RakNet::BitStream *stream;
	ulong keynum;

	LDF(){
		stream = new RakNet::BitStream();
		keynum = 0;
	}

	void writeFLOAT(std::wstring key, float data){ //3
		writeWString(key, this->stream, true);
		this->stream->Write((uchar)3);
		this->stream->Write(data);
		keynum++;
	}

	void writeS32(std::wstring key, long data){ //1
		writeWString(key, this->stream, true);
		this->stream->Write((uchar)1);
		this->stream->Write(data);
		keynum++;
	}

	void writeS64(std::wstring key, long long data){ //8
		writeWString(key, this->stream, true);
		this->stream->Write((uchar)8);
		this->stream->Write(data);
		keynum++;
	}

	void writeID(std::wstring key, long long data){ //9
		writeWString(key, this->stream, true);
		this->stream->Write((uchar)9);
		this->stream->Write(data);
		keynum++;
	}

	void writeBOOL(std::wstring key, bool data){ //7
		writeWString(key, this->stream, true);
		this->stream->Write((uchar)7);
		if (data) this->stream->Write((uchar)1); else this->stream->Write((uchar)0);
		keynum++;
	}

	void writeWSTRING(std::wstring key, std::wstring data){ //0
		writeWString(key, this->stream, true);
		this->stream->Write((uchar)0);
		this->stream->Write((ulong)data.size());
		writeWString(data, this->stream, false);
		keynum++;
	}

	void writeBYTES(std::wstring key, RakNet::BitStream *data){ //13
		writeWString(key, this->stream, true);
		this->stream->Write((uchar)13);
		this->stream->Write((ulong)data->GetNumberOfBytesUsed());
		this->stream->Write(data, data->GetNumberOfBytesUsed() * 8);
		keynum++;
	}

	void writeToPacket(RakNet::BitStream *packet){
		packet->Write(keynum);
		packet->Write(stream, stream->GetNumberOfBytesUsed() * 8);
	}

	ulong getSize(){
		return (ulong)(stream->GetNumberOfBytesUsed() + 4);
	}

	~LDF(){
		delete stream;
	}
};