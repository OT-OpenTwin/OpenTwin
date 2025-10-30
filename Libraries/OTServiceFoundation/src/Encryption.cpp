// @otlicense
// File: Encryption.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTEncryptionKey.h"
#include "OTSystem/OTAssert.h"
#include "OTSystem/Exception.h"
#include "OTServiceFoundation/Encryption.h"

// Third party header
#include "base64.h"
#include <zlib.h>

std::string ot::Encryption::encryptString(const std::string& _str) {
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

std::string ot::Encryption::decryptString(const std::string& _str) {
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
