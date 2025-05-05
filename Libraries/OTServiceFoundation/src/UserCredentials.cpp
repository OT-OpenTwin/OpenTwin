//! @file UserCredentials.cpp
// ###########################################################################################################################################################################################################################################################################################################################

// Open Twin header
#include "OTServiceFoundation/UserCredentials.h"		// Corresponding header
#include "OTServiceFoundation/Encryption.h"

std::string ot::UserCredentials::encryptString(const std::string& _raw) {
	return ot::Encryption::encryptString(_raw);
}

std::string ot::UserCredentials::decryptString(const std::string& _encrypted) {
	return ot::Encryption::decryptString(_encrypted);
}