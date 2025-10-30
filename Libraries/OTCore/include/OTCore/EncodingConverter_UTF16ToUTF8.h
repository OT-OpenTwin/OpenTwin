// @otlicense
// File: EncodingConverter_UTF16ToUTF8.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#include <vector>
#include <string>
#include "OTCore/TextEncoding.h"

namespace ot
{
	//! @brief Converter that can transform a set of UTF-16 LE BOM or UTF16 BE BOM characters into a set of UTF-8 character.
	class OT_CORE_API_EXPORT EncodingConverter_UTF16ToUTF8
	{
	public:
		std::string operator()(ot::TextEncoding::EncodingStandard utf16Flavour, const std::vector<char>& fileContent);
		std::string operator()(ot::TextEncoding::EncodingStandard utf16Flavour, const std::string& fileContent);

	private:
		using byte = unsigned char;
		ot::TextEncoding::EncodingStandard _utf16Flavour = ot::TextEncoding::EncodingStandard::UNKNOWN;
		byte* _currentByte = nullptr;
		std::string _out;

		char16_t getNextUFT16();
		void setNextUTF8Character(byte&& character);
		void TransformNextWORD();
	};
}
