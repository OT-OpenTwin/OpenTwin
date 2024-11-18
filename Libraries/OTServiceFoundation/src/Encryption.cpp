//! @file Encryption.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTEncryptionKey.h"
#include "OTCore/OTAssert.h"
#include "OTServiceFoundation/Encryption.h"

#include <zlib.h>
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

OT_SERVICEFOUNDATION_API_EXPORT std::string ot::decryptAndUnzipString(const std::string& _content, uint64_t _uncompressedLength)
{
	int decoded_compressed_data_length = Base64decode_len(_content.c_str());
	char* decodedCompressedContent = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedContent, _content.c_str());

	// Decompress the data
	char* decodedUncompressesContent = new char[_uncompressedLength];
	uLongf destLen = (uLongf)_uncompressedLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)decodedUncompressesContent, &destLen, (Bytef*)decodedCompressedContent, sourceLen);

	delete[] decodedCompressedContent;
	decodedCompressedContent = nullptr;
	std::string decodedAndUnzipped(decodedUncompressesContent, _uncompressedLength);
	
	delete[] decodedUncompressesContent;
	decodedUncompressesContent = nullptr;

	return decodedAndUnzipped;
}

OT_SERVICEFOUNDATION_API_EXPORT std::string ot::encryptAndZipString(const std::string& _content)
{
	
	// Compress the file data content
	uLong compressedSize = compressBound((uLong)_content.size());

	char* compressedData = new char[compressedSize];
	if (_content.size() > UINT32_MAX)
	{
		throw std::exception("Message to large for compression.");
	}
	compress((Bytef*)compressedData, &compressedSize, (Bytef*)_content.data(), static_cast<uint32_t>(_content.size()));

	// Convert the binary to an encoded string
	int encoded_data_length = Base64encode_len(compressedSize);
	char* base64_string = new char[encoded_data_length];

	Base64encode(base64_string, compressedData, compressedSize); // "base64_string" is a then null terminated string that is an encoding of the binary data pointed to by "data"

	delete[] compressedData;
	compressedData = nullptr;

	std::string compressedEncryptedContent(base64_string, encoded_data_length);
	delete[] base64_string;
	base64_string = nullptr;
	return compressedEncryptedContent;
}
