#include "Encryption.h"
#include "sha512.hh"
#include "BaseEncode.h"
#include <iostream>

std::string hashPassword(std::string password){
	std::string hashedPassword = sw::sha512::calculate(password);
	return base64_encode(reinterpret_cast<const unsigned char*>(hashedPassword.c_str()), hashedPassword.length());
}