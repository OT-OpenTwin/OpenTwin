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
		using byte = unsigned char;
		TextEncoding::EncodingStandard operator()(const std::vector<char>& fileContent);
		TextEncoding::EncodingStandard operator()(const char* fileContent, int64_t size);
	private:
		const byte utf8Bom[3] = { 0xEF, 0xBB, 0xBF }; // This is the UTF-16 BOM after being converted to UTF-8
		const byte bigEndianBom[2] = { 0xFE, 0xFF};
		const byte littleEndianBom[2] = { 0xFF, 0xFE};

		bool CheckIfISO(byte* fileContent, int64_t numberOfBytes);
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