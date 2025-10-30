// @otlicense
// File: EncodingGuesser.h
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
	//! @brief Detects the encoding style. The detection class can only find the next best match and does not guarantee the correct detection of the intended standard. 
	//! Thus, the detected standard should be propagated to the user, to allow a change.
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