#include "Encryption.h"
#include "md5.h"

std::string hashPassword(std::string password){
	return md5(password);
}