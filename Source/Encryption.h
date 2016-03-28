#pragma once
#include <string>

//Wrapper for a hashing algorithm, SHA512 with base64 encode
std::string hashPassword(std::string password);