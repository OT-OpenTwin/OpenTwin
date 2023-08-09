#pragma once
#include <vector>
#include <string>

#include "OpenTwinCore/CoreAPIExport.h"
namespace ot
{
	enum TextEncoding {	UTF8 = 0, UTF8_BOM = 1 , ANSI= 2, UTF16_LEBOM = 3, UTF16_BEBOM= 4, UNKNOWN = 5	};

	class OT_CORE_API_EXPORT EncodingGuesser
	{
	public:
		using utf8 = unsigned char;
		TextEncoding operator()(std::vector<unsigned char>& fileContent);
	private:
		const utf8 unknwon_Bom[2] = { 0x00, 0x00};
		const utf8 utf8_Bom[3] = { 0xEF, 0xBB, 0xBF };
		const utf8 bigEndian_Bom[2] = { 0xFE, 0xFF};
		const utf8 littleEndian_Bom[2] = { 0xFF, 0xFE};

		enum type7or8Byte { utf8NoBOM = 0, ascii7bits = 1, ascii8bits = 2 };
		type7or8Byte CheckUtf8_7bits_8bits(std::vector<unsigned char>& fileContent);
	};

	class OT_CORE_API_EXPORT EncodingConverter_ISO88591ToUTF8
	{
	public:
		std::string operator()(std::vector<unsigned char>& fileContent);
	};
	class OT_CORE_API_EXPORT EncodingConverter_UTF16ToUTF8
	{
	public:
		std::string operator()(std::vector<unsigned char>& fileContent);
	};
}