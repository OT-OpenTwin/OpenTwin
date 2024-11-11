#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"
#include <assert.h>
#include <cmath>

std::string ot::EncodingConverter_ISO88591ToUTF8::operator()(const std::vector<char>& fileContent)
{
	std::string out;
	const std::string fileContentString(fileContent.begin(), fileContent.end());
	return this->operator()(fileContentString);
}

std::string ot::EncodingConverter_ISO88591ToUTF8::operator()(const std::string& fileContent)
{
	//Value ranges from:  https://www.utf8-zeichentabelle.de/ and https://de.wikipedia.org/wiki/Steuerzeichen
	std::vector<char> utfCharList;
	utfCharList.reserve(std::llround(fileContent.size() * 1.5)); // In the worst case, each ANSI character requires two byte in UTF-8. Very unlikely, thus the reserved memory is the middle between best-case and worst-case
	for (const unsigned char& ch : fileContent)
	{
		if (ch < 0x80) {
			utfCharList.push_back(ch);
		}
		else if (ch >= 0x80 && ch <= 0x9F) //QT has issues to display the extended control character set c1.
		{
			//skip
		}
		else if (ch > 0x9F && ch <= 0xBF)
		{
			utfCharList.push_back(static_cast<char>(0xc2));
			utfCharList.push_back(ch);
		}
		else {
			assert(ch >= 0xC0);
			
			utfCharList.push_back(static_cast<char>(0xc3));
			const unsigned char transformed = ch & 0xBF;
			utfCharList.push_back(transformed);
		}
	}
	if (utfCharList.back() != '\0')
	{
		utfCharList.push_back('\0');
	}
	utfCharList.shrink_to_fit();
	std::string utf8_string(utfCharList.data());
	return utf8_string;
}

