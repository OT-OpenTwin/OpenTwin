//! @file Encryption.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTEncryptionKey.h"
#include "OTCore/OTAssert.h"
#include "OTServiceFoundation/Encryption.h"

// Third party header
#include "base64.h"

std::string ot::encryptString(const std::string& _str) {
	const int dataLength = (int)_str.length();
	const int encryptedDataLength = (int)(dataLength * 2 + sizeof(int));

	char* randomDataEncrypted = new char[encryptedDataLength + 1];

	*((int*)randomDataEncrypted) = dataLength;

	for (int i = 0; i < dataLength; i++) {
		randomDataEncrypted[i * 2 + sizeof(int)] = _str[i];
		randomDataEncrypted[i * 2 + 1 + sizeof(int)] = 'a' + rand() % 26;
	}
	randomDataEncrypted[encryptedDataLength] = '\0';

	for (int i = sizeof(int); i < encryptedDataLength; i++) {
		randomDataEncrypted[i] = randomDataEncrypted[i] ^ ot::EncryptionKey[(i - sizeof(int)) % (sizeof(ot::EncryptionKey) / sizeof(int))];
	}

	int encoded_data_length = Base64encode_len(encryptedDataLength + 1);
	char* base64_string = new char[encoded_data_length];

	Base64encode(base64_string, randomDataEncrypted, encryptedDataLength + 1);

	std::string base64Encrypted(base64_string);
	OTAssert(_str == decryptString(base64Encrypted), "Data length mismatch");

	delete[] base64_string;       base64_string = nullptr;
	delete[] randomDataEncrypted; randomDataEncrypted = nullptr;

	return base64Encrypted;
}

std::string ot::decryptString(const std::string& _str) {
	int decoded_compressed_data_length = Base64decode_len(_str.c_str());
	char* decoded_compressed_string = new char[decoded_compressed_data_length];
	char* decoded_decompressed_string = new char[decoded_compressed_data_length];

	Base64decode(decoded_compressed_string, _str.c_str());

	unsigned int originalLength = *((unsigned int*)decoded_compressed_string);

	if (originalLength * 2 + sizeof(int) > decoded_compressed_data_length)
	{
		// This password exceeds the maximum length -> most likely it is an invalid compressed string
		return "";
	}

	for (unsigned int i = sizeof(int); i < originalLength * 2 + sizeof(int); i++) {
		decoded_compressed_string[i] = decoded_compressed_string[i] ^ ot::EncryptionKey[(i - sizeof(int)) % (sizeof(ot::EncryptionKey) / sizeof(int))];
	}

	for (unsigned int i = 0; i < originalLength; i++) {
		decoded_decompressed_string[i] = decoded_compressed_string[i * 2 + sizeof(int)];
	}
	decoded_decompressed_string[originalLength] = '\0';

	std::string base64_decoded(decoded_decompressed_string);

	delete[] decoded_compressed_string;   decoded_compressed_string = nullptr;
	delete[] decoded_decompressed_string; decoded_decompressed_string = nullptr;

	return base64_decoded;
}