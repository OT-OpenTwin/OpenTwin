/*****************************************************************//**
 * \file   EncodingConverter_ISO88591ToUTF8.h
 * \brief  Converter that can transform a set of ISO 8859-1 characters into a set of UTF-8 character.
 * 
 * \author Wagner
 * \date   August 2023
 *********************************************************************/
#pragma once
#include <vector>
#include <string>
#include "OTCore/CoreAPIExport.h"

namespace ot
{
	class OT_CORE_API_EXPORT EncodingConverter_ISO88591ToUTF8
	{
	public:
		std::string operator()(const std::vector<char>& fileContent);
		std::string operator()(const std::string& fileContent);
	};
}
