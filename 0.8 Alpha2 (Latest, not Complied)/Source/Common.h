#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

#include "RakNet\BitStream.h"

// Makes it easier to use uchar, ushort, etc. for unsigned chars, shorts, etc.
typedef unsigned char uchar; // unsigned char = uchar
typedef unsigned short ushort; // unsigned short = ushort
typedef unsigned int uint; // unsigned int = uint
typedef unsigned long ulong; // unsinged long = ulong
typedef unsigned long long ulonglong; // unsigned long long = ulonglong

struct CONNECT_INFO {
	char redirectIp[16];// AnIPv4 address can only occupy up to 15 bytes, do we assume a 16th byte as end char?
	short redirectPort;	// Ports occupy 2 bytes - This is the redirect port
	short listenPort; // Port to listen on

	bool useEncryption; // Should the server use encryption or not
	bool logFile; // Should the server log a file with its information

	// CONNECT_INFO initialization - sets the redirectIp to all 0s.
	CONNECT_INFO() { for (unsigned int i = 0; i < 16; i++) redirectIp[i] = 0; }
};

// Convert raw hex data to string
std::string RawDataToString(unsigned char* data, unsigned int size, bool onlyraw = false, unsigned long start = 0);

// Error message
void QuitError(const std::string& msg = "");

void writeWString(std::wstring str, RakNet::BitStream *stream, bool writeSize = true, bool nullChar = false);

bool startsWith(std::wstring haystack, std::wstring needle);
bool endsWith(std::wstring haystack, std::wstring needle);
std::vector<std::string> split(std::string text, char sep);
std::wstring removeRight(std::wstring str, unsigned int num);
std::wstring removeLeft(std::wstring str, unsigned int num);
std::wstring remove(std::wstring str, unsigned int left, unsigned int right);
bool isNumber(std::string str);