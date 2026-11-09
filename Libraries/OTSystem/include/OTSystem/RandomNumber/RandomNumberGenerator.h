// @otlicense
// File: RandomNumberGenerator.h
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

// OpenTwin header
#include "OTSystem/SystemTypes.h"

// std header
#include <vector>

namespace ot
{

	class OT_SYS_API_EXPORT RandomNumberGenerator
	{
	public:
		RandomNumberGenerator();
		virtual ~RandomNumberGenerator() = default;

		//! @brief Creates a positive random integer in the range [0, _maxValue].
		//! @note Use this method if a single random integer is needed.
		//! For generating multiple random integers, consider using createRandomBytes() for better performance.
		//! @param _maxValue The maximum value of the random integer to be generated. Must be greater than 0.
		virtual uint64_t createPositiveRandomInt64(uint64_t _maxValue) = 0;

		//! @brief Creates a vector of random bytes of the specified size.
		//! @param _size The number of random bytes to generate.
		virtual std::vector<uint8_t> createRandomBytes(uint64_t _size) = 0;
	};

}
