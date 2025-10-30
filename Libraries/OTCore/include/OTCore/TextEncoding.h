// @otlicense

#pragma once
#pragma warning(disable:4251)
#include <string>
#include <map>

#include "OTCore/CoreTypes.h"

namespace ot
{
	//! @brief This class holds all types of text encodings that can be detected with the EncodingGuesser class.
	//! MongoDB uses only UTF-8 and some errors occured if the characters with different encoding standard were stored.
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
}