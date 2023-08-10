#include "OpenTwinCore/TextEncoding.h"
#include <codecvt>

ot::TextEncoding::EncodingStandard ot::EncodingGuesser::operator()(const char* fileContent, int64_t numberOfBytes)
{
	using EncodingStandard = ot::TextEncoding::EncodingStandard;

	EncodingStandard encoding = ot::TextEncoding::EncodingStandard::UNKNOWN;
	utf8* firstChar = (unsigned char*)(fileContent);
	// detect UTF-16 big-endian with BOM
	if (numberOfBytes > 2 && firstChar[0] == bigEndian_Bom[0] && firstChar[1] == bigEndian_Bom[1])
	{
		encoding = EncodingStandard::UTF16_BEBOM;
	}
	// detect UTF-16 little-endian with BOM
	else if (numberOfBytes > 2 && firstChar[0] == littleEndian_Bom[0] && firstChar[1] == littleEndian_Bom[1])
	{
		encoding = EncodingStandard::UTF16_LEBOM;
	}
	// detect UTF-8 with BOM
	else if (numberOfBytes > 3 && firstChar[0] == utf8_Bom[0] &&
		firstChar[1] == utf8_Bom[1] && firstChar[2] == utf8_Bom[2])
	{
		encoding = EncodingStandard::UTF8_BOM;
	}
	//Code uses windows API
	// try to detect UTF-16 little-endian without BOM
	/*else if (numberOfBytes > 2 && numberOfBytes -1 % 2 == 0 && fileContent[0] != 0 && fileContent[1] == 0 && IsTextUnicode(fileContent, static_cast<int32_t>(numberOfBytes), &uniTest))
	{
		m_eEncoding = uni16LE_NoBOM;
		m_nSkip = 0;
	}*/
	else
	{
		type7or8Byte detectedEncoding = CheckUtf8_7bits_8bits(firstChar, numberOfBytes);

		if (detectedEncoding == utf8NoBOM)
		{
			encoding = EncodingStandard::UTF8;
		}
		else if (detectedEncoding == ascii8bits)
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

ot::EncodingGuesser::type7or8Byte ot::EncodingGuesser::CheckUtf8_7bits_8bits(utf8* fileContent, int64_t numberOfBytes)
{
	int rv = 1;
	int ASCII7only = 1;
	utf8* sx = fileContent;
	utf8* endx = sx + numberOfBytes;

	while (sx < endx)
	{
		if (*sx == '\0')
		{											// For detection, we'll say that NUL means not UTF8
			ASCII7only = 0;
			rv = 0;
			break;
		}
		else if ((*sx & 0x80) == 0x0)
		{			// 0nnnnnnn If the byte's first hex code begins with 0-7, it is an ASCII character.
			++sx;
		}
		else if ((*sx & (0x80 + 0x40)) == 0x80)
		{											  // 10nnnnnn 8 through B cannot be first hex codes
			ASCII7only = 0;
			rv = 0;
			break;
		}
		else if ((*sx & (0x80 + 0x40 + 0x20)) == (0x80 + 0x40))
		{					  // 110xxxvv 10nnnnnn, 11 bit character
			ASCII7only = 0;
			if (std::distance(sx, endx) < 2) {
				rv = 0; break;
			}
			if ((sx[1] & (0x80 + 0x40)) != 0x80) {
				rv = 0; break;
			}
			sx += 2;
		}
		else if ((*sx & (0x80 + 0x40 + 0x20 + 0x10)) == (0x80 + 0x40 + 0x20))
		{								// 1110qqqq 10xxxxvv 10nnnnnn, 16 bit character
			ASCII7only = 0;
			if (std::distance(sx, endx) < 3) {
				rv = 0; break;
			}
			if ((sx[1] & (0x80 + 0x40)) != 0x80 || (sx[2] & (0x80 + 0x40)) != 0x80) {
				rv = 0; break;
			}
			sx += 3;
		}
		else if ((*sx & (0x80 + 0x40 + 0x20 + 0x10 + 0x8)) == (0x80 + 0x40 + 0x20 + 0x10))
		{								// 11110qqq 10xxxxvv 10nnnnnn 10mmmmmm, 21 bit character
			ASCII7only = 0;
			if (std::distance(sx, endx) < 4) {
				rv = 0; break;
			}
			if ((sx[1] & (0x80 + 0x40)) != 0x80 || (sx[2] & (0x80 + 0x40)) != 0x80 || (sx[3] & (0x80 + 0x40)) != 0x80) {
				rv = 0; break;
			}
			sx += 4;
		}
		else
		{
			ASCII7only = 0;
			rv = 0;
			break;
		}
	}
	if (ASCII7only)
		return ascii7bits;
	if (rv)
		return utf8NoBOM;
	return ascii8bits;
}


std::string ot::EncodingConverter_ISO88591ToUTF8::operator()(std::vector<char>& fileContent)
{
	//#include <boost/locale.hpp>
//std::string ansi_to_utf8(const std::string& str)
//{
//	return boost::locale::conv::to_utf<char>(str, "Latin1");
//}
	//Source: https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c

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


std::string ot::EncodingConverter_UTF16ToUTF8::operator()(std::vector<char>& fileContent)
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
