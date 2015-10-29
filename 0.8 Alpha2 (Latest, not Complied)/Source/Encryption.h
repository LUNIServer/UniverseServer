#pragma once
#include <string>

//Wrapper for a hashing algorithm, will be md5 for now, can be changed to a more secure one in the future
std::string hashPassword(std::string password);