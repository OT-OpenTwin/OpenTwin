#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"

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


