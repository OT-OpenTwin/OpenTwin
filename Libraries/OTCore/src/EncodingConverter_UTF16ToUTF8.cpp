#include "OTCore/EncodingConverter_UTF16ToUTF8.h"


void ot::EncodingConverter_UTF16ToUTF8::TransformNextWORD()
{
	char16_t nextUTF16Char = getNextUFT16();

	if (nextUTF16Char < 0x80) { //ASCII
		setNextUTF8Character(static_cast<byte>(nextUTF16Char));
	}
	else if (nextUTF16Char < 0x800) { // 2 Byte Code Units
		setNextUTF8Character(static_cast<byte>(0xC0 | nextUTF16Char >> 6));
		setNextUTF8Character(static_cast<byte>(0x80 | (nextUTF16Char & 0x3f)));
	}
	else if ((nextUTF16Char >= 0xd800) && (nextUTF16Char < 0xdc00)) { // 4 Byte Code Units
		if ((nextUTF16Char >= 0xDC00) && (nextUTF16Char < 0xE000)) // Only valid combination
		{ // valid surrogate pair
			char16_t secondUTF16Char = getNextUFT16();
			unsigned int code = 0x10000 + ((nextUTF16Char & 0x3ff) << 10) + (secondUTF16Char & 0x3ff);
			setNextUTF8Character(0xf0 | ((code >> 18) & 0x07));
			setNextUTF8Character(0x80 | ((code >> 12) & 0x3f));
			setNextUTF8Character(0x80 | ((code >> 6) & 0x3f));
			setNextUTF8Character(0x80 | (code & 0x3f));
		}
	}
	else { // 3 Byte Code Units
		setNextUTF8Character(static_cast<byte>(0xE0 | (nextUTF16Char >> 12)));
		setNextUTF8Character(static_cast<byte>(0x80 | ((nextUTF16Char >> 6) & 0x3f)));
		setNextUTF8Character(static_cast<byte>(0x80 | (nextUTF16Char & 0x3f)));
	}
}

char16_t ot::EncodingConverter_UTF16ToUTF8::getNextUFT16()
{
	char16_t nextUTF16;
	if (_utf16Flavour == ot::TextEncoding::UTF16_LEBOM)
	{
		nextUTF16 = *_currentByte;
		_currentByte++;
		nextUTF16 |= static_cast<char16_t>(*_currentByte << 8);
	}
	else
	{
		nextUTF16 = static_cast<char16_t>(*_currentByte << 8);
		_currentByte++;
		nextUTF16 |= *_currentByte;
	}
	_currentByte++;
	return nextUTF16;
}

void ot::EncodingConverter_UTF16ToUTF8::setNextUTF8Character(byte&& character)
{
	_out.push_back(character);
}


std::string ot::EncodingConverter_UTF16ToUTF8::operator()(ot::TextEncoding::EncodingStandard utf16Flavour, const std::vector<char>& fileContent)
{
	_currentByte = (unsigned char*)&fileContent[0];
	_utf16Flavour = utf16Flavour;
	byte* lastByte = (unsigned char*)&fileContent[fileContent.size() - 1];

	while (std::distance(_currentByte, lastByte) >= 1)
	{
		TransformNextWORD();
	}

	std::string textWithoutBOM = _out.substr(3, _out.size());
	return textWithoutBOM;
}
