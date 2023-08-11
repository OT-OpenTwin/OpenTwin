/*****************************************************************//**
 * \file   EncodingGuesser.h
 * \brief  Detects the encoding style. The detection class can only find the next best match and does not guarantee the correct detection of the intended standard. 
 *			Thus, the detected standard should be propagated to the user, to allow a change.
 * 
 * \author Wagner
 * \date   August 2023
 *********************************************************************/
#pragma once
#include <vector>
#include <string>
#include "OpenTwinCore/CoreAPIExport.h"
#include"OpenTwinCore/TextEncoding.h"

namespace ot
{
	class OT_CORE_API_EXPORT EncodingGuesser
	{
	public:
		TextEncoding::EncodingStandard operator()(const std::vector<char>& fileContent);
		TextEncoding::EncodingStandard operator()(const char* fileContent, int64_t size);
	private:
		using byte = unsigned char;
		const byte utf8Bom[3] = { 0xEF, 0xBB, 0xBF }; // This is the UTF-16 BOM after being converted to UTF-8
		const byte bigEndianBom[2] = { 0xFE, 0xFF };
		const byte littleEndianBom[2] = { 0xFF, 0xFE };

		bool CheckIfISO(byte* fileContent, int64_t numberOfBytes);
	};
}