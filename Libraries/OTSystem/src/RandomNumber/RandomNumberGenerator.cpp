// @otlicense
// File: RandomNumberGenerator.cpp
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

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberGenerator.h"
#include "OTSystem/ArchitectureInfo.h"
#include "OTSystem/Exception.h"

#include <cassert>
#include <cstring>

ot::RandomNumberGenerator::RandomNumberGenerator() {}

uint64_t ot::RandomNumberGenerator::createPositiveRandomInt64(uint64_t maxValue)
{
	if (maxValue == 0)
	{
		throw Exception::OutOfBounds("maxValue must be > 0");
	}

	auto generateRandomUInt64 = [&]() -> uint64_t
		{
			std::vector<uint8_t> bytes = createRandomBytes(sizeof(uint64_t));

			uint64_t result;
			std::memcpy(&result, bytes.data(), sizeof(result));

			return result;
		};

	// Special case: the entire uint64_t range
	if (maxValue == UINT64_MAX)
	{
		return generateRandomUInt64();
	}

	// Calculate the number of possible outcomes
	const uint64_t range = maxValue + 1;

	// Calculate the rejection threshold to avoid modulo bias
	const uint64_t threshold = (uint64_t(0) - range) % range;

	uint64_t randomNumber;

	do
	{
		randomNumber = generateRandomUInt64();
	}
	while (randomNumber < threshold);

	return randomNumber % range;
}
