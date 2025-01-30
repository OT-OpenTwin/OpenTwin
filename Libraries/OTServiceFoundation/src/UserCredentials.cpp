/*
 *	credentials.cpp
 *
 */

// Open Twin header
#include "OTServiceFoundation/UserCredentials.h"		// Corresponding header
#include "OTServiceFoundation/Encryption.h"

std::string ot::UserCredentials::encryptString(std::string data)
{
	return ot::Encryption::encryptString(data);
}

std::string ot::UserCredentials::decryptString(std::string data)
{
	return ot::Encryption::decryptString(data);
}