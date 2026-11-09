// @otlicense
// File: SoftwareRandomNumberGenerator.h
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
#include "OTSystem/RandomNumber/RandomNumberGenerator.h"

namespace ot
{

	class OT_SYS_API_EXPORT RandomNumberGeneratorSW : public RandomNumberGenerator
	{
	public:
		virtual uint64_t createPositiveRandomInt64(uint64_t maxValue = std::numeric_limits<uint64_t>::max()) override;
		virtual std::vector<uint8_t> createRandomBytes(uint64_t _size) override;
	};

}