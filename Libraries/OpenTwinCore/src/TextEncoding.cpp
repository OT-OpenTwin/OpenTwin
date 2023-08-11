#include "OpenTwinCore/TextEncoding.h"
#include <codecvt>

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
	return operator()(&fileContent[0], numberOfBytes);
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


std::string ot::EncodingConverter_ISO88591ToUTF8::operator()(const std::vector<char>& fileContent)
{
	std::string out;
	unsigned char* ch = (unsigned char*)&fileContent[0];
	for (uint32_t i = 0; i < fileContent.size(); i++)
	{
		if (ch[i] < 0x80) {
			out.push_back(ch[i]);
		}
		else {
			out.push_back(0xc0 | ch[i] >> 6);
			out.push_back(0x80 | (ch[i] & 0x3f));
		}
	}
	return out;
}


std::string ot::EncodingConverter_UTF16ToUTF8::operator()(const std::vector<char>& fileContent)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	std::string out;
	char16_t* ch = (char16_t*) &fileContent[0];
	for (int i = 1; i < fileContent.size()/2; i++)
	{
		out+= convert.to_bytes(ch);
		ch++;
	}
	return out;
}

std::string ot::TextEncoding::getString(ot::TextEncoding::EncodingStandard type) const
{
	if (_encodingStandardToString.find(type) == _encodingStandardToString.end())
	{
		throw std::exception("Encoding standard not found");
	}
	else
	{
		return (*_encodingStandardToString.find(type)).second;
	}
}

ot::TextEncoding::EncodingStandard ot::TextEncoding::getType(const std::string& type) const
{
	if (_stringToEncodingStandard.find(type) == _stringToEncodingStandard.end())
	{
		throw std::exception("Encoding standard not found");
	}
	else
	{
		return (*_stringToEncodingStandard.find(type)).second;
	}
}
