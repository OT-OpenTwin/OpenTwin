// @otlicense

#include "OTCore/EncodingGuesser.h"

ot::TextEncoding::EncodingStandard ot::EncodingGuesser::operator()(const char* fileContent, int64_t numberOfBytes)
{
	using EncodingStandard = ot::TextEncoding::EncodingStandard;

	EncodingStandard encoding = ot::TextEncoding::EncodingStandard::UNKNOWN;
	byte* firstChar = (unsigned char*)(fileContent);
	// detect UTF-16 big-endian with BOM
	if (numberOfBytes > 2 && firstChar[0] == bigEndianBom[0] && firstChar[1] == bigEndianBom[1])
	{
		encoding = EncodingStandard::UTF16_BEBOM;
	}
	// detect UTF-16 little-endian with BOM
	else if (numberOfBytes > 2 && firstChar[0] == littleEndianBom[0] && firstChar[1] == littleEndianBom[1])
	{
		encoding = EncodingStandard::UTF16_LEBOM;
	}
	// detect UTF-8 with BOM
	else if (numberOfBytes > 3 && firstChar[0] == utf8Bom[0] &&
		firstChar[1] == utf8Bom[1] && firstChar[2] == utf8Bom[2])
	{
		encoding = EncodingStandard::UTF8_BOM;
	}

	else
	{
		bool isISOEncoding = CheckIfISO(firstChar, numberOfBytes);

		if (isISOEncoding)
		{
			encoding = EncodingStandard::ANSI;
		}
		else
		{
			encoding = EncodingStandard::UTF8;
		}
	}
	return encoding;
}

ot::TextEncoding::EncodingStandard ot::EncodingGuesser::operator()(const std::vector<char>& fileContent)
{
	uint64_t numberOfBytes = fileContent.size();
	if (numberOfBytes == 0)
	{
		return TextEncoding::UTF8;
	}
	else
	{
		return operator()(&fileContent[0], numberOfBytes);
	}
}

bool ot::EncodingGuesser::CheckIfISO(byte* fileContent, int64_t numberOfBytes)
{
	bool rv = true;
	bool ASCII7only = true;
	byte* currentByte = fileContent;
	byte* lastByte = currentByte + numberOfBytes;

	bool isISOEncoding = false;
	while (currentByte < lastByte)
	{
		if (*currentByte == '\0')
		{
			isISOEncoding = true;
			break;
		}
		else if ((*currentByte & 0x80) == 0x0) // ASCII character are going until 0x7F, thus a char is ASCII if the msb of the byte is not set 
		{
			++currentByte;
		}
		else if ((*currentByte & (0x80 + 0x40)) == 0x80) // The first Hexcode cannot be 8 through B for UTF-8 
		{
			isISOEncoding = true;
			break;
		}
		else if ((*currentByte & (0x80 + 0x40 + 0x20)) == (0x80 + 0x40))// 11 bit character
		{

			if (std::distance(currentByte, lastByte) < 2) {
				isISOEncoding = true;
				break;
			}
			if ((currentByte[1] & (0x80 + 0x40)) != 0x80) {
				isISOEncoding = true;
				break;
			}
			currentByte += 2;
		}
		else if ((*currentByte & (0x80 + 0x40 + 0x20 + 0x10)) == (0x80 + 0x40 + 0x20)) // 16 bit character
		{

			if (std::distance(currentByte, lastByte) < 3) {
				isISOEncoding = true;
				break;
			}
			if ((currentByte[1] & (0x80 + 0x40)) != 0x80 || (currentByte[2] & (0x80 + 0x40)) != 0x80) {
				isISOEncoding = true;
				break;
			}
			currentByte += 3;
		}
		else if ((*currentByte & (0x80 + 0x40 + 0x20 + 0x10 + 0x8)) == (0x80 + 0x40 + 0x20 + 0x10)) // 21 bit character
		{

			if (std::distance(currentByte, lastByte) < 4) {
				isISOEncoding = true;
				break;
			}
			if ((currentByte[1] & (0x80 + 0x40)) != 0x80 || (currentByte[2] & (0x80 + 0x40)) != 0x80 || (currentByte[3] & (0x80 + 0x40)) != 0x80) {
				isISOEncoding = true;
				break;
			}
			currentByte += 4;
		}
		else
		{
			isISOEncoding = true;
			break;
		}
	}
	return isISOEncoding;
}


