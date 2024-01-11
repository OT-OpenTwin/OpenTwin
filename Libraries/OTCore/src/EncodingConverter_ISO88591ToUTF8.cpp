#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"

std::string ot::EncodingConverter_ISO88591ToUTF8::operator()(const std::vector<char>& fileContent)
{
	std::string out;
	const std::string fileContentString(fileContent.begin(), fileContent.end());
	return this->operator()(fileContentString);
}

std::string ot::EncodingConverter_ISO88591ToUTF8::operator()(const std::string& fileContent)
{
	std::string out;
	for (const unsigned char& ch : fileContent)
	{
		if (ch < 0x80) {
			out.push_back(ch);
		}
		else {
			out.push_back(0xc0 | ch >> 6);
			out.push_back(0x80 | (ch & 0x3f));
		}
	}
	return out;
}

