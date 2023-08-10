/*****************************************************************//**
 * \file   TextEncoding.h
 * \brief  MongoDB uses only UTF-8 and some errors occured if the characters with different encoding standard were stored.
 *			These clases allow a detection of the encoding style and a conversion to UTF-8. 
 *			The detection class can only find the next best match and does not guarantee the correct detection of the intended standard. Thus, the detected standard should be propagated to the user, to allow a change.
 * 
 * \author Wagner
 * \date   August 2023
 *********************************************************************/
#pragma once
#include <vector>
#include <string>
#include <map>

#include "OpenTwinCore/CoreAPIExport.h"
#pragma warning(disable : 4251)
namespace ot
{
	class OT_CORE_API_EXPORT TextEncoding
	{
	public:
		enum EncodingStandard { UTF8 = 0, UTF8_BOM = 1, ANSI = 2, UTF16_LEBOM = 3, UTF16_BEBOM = 4, UNKNOWN = 5 };
		std::string getString(const EncodingStandard type) const;
		EncodingStandard getType(const std::string& type) const;
	private:

		const std::map<EncodingStandard, std::string> _encodingStandardToString = { {UTF8, "UTF-8"}, {UTF8_BOM, "UTF-8 BOM"}, {ANSI, "ISO 8859-1"}, {UTF16_BEBOM, "UTF-16 BE BOM"}, {UTF16_LEBOM, "UTF-16 LE BOM"}, {UNKNOWN, "Unknown"} };
		const std::map<std::string, EncodingStandard> _stringToEncodingStandard = { {"UTF-8",UTF8}, {"UTF-8 BOM",UTF8_BOM}, {"ISO 8859-1",ANSI}, {"UTF-16 BE BOM",UTF16_BEBOM}, {"UTF-16 LE BOM",UTF16_LEBOM}, {"Unknown",UNKNOWN} };
	};
	
	class OT_CORE_API_EXPORT EncodingGuesser
	{
	public:
		using utf8 = unsigned char;
		TextEncoding::EncodingStandard operator()(const std::vector<char>& fileContent);
		TextEncoding::EncodingStandard operator()(const char* fileContent, int64_t size);
	private:
		const utf8 unknwon_Bom[2] = { 0x00, 0x00};
		const utf8 utf8_Bom[3] = { 0xEF, 0xBB, 0xBF };
		const utf8 bigEndian_Bom[2] = { 0xFE, 0xFF};
		const utf8 littleEndian_Bom[2] = { 0xFF, 0xFE};

		enum type7or8Byte { utf8NoBOM = 0, ascii7bits = 1, ascii8bits = 2 };
		type7or8Byte CheckUtf8_7bits_8bits(utf8* fileContent, int64_t numberOfBytes);
	};

	class OT_CORE_API_EXPORT EncodingConverter_ISO88591ToUTF8
	{
	public:
		std::string operator()(const std::vector<char>& fileContent);
	};
	class OT_CORE_API_EXPORT EncodingConverter_UTF16ToUTF8
	{
	public:
		std::string operator()(const std::vector<char>& fileContent);
	};
}