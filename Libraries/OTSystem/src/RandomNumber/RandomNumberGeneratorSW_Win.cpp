// @otlicense
// File: RandomNumberGeneratorSW_Win.cpp
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

#include "OTSystem/ArchitectureInfo.h"

#ifdef OT_OS_WINDOWS

#include "OTSystem/RandomNumber/RandomNumberGeneratorSW.h"
#include "OTSystem/Exception.h"
#include <exception>
#include <cassert>

#define WIN32_NO_STATUS 

// Windows header
#include <Windows.h>
#include <bcrypt.h>

#undef WIN32_NO_STATUS 
#undef min
#undef max
#include <ntstatus.h>

std::vector<uint8_t> ot::RandomNumberGeneratorSW::createRandomBytes(uint64_t _size)
{
	if (_size == 0)
	{
		throw Exception::OutOfBounds("Size must be > 0");
	}

	if (_size > std::numeric_limits<ULONG>::max())
	{
		throw Exception::OutOfBounds("Requested buffer is too large for the RandomNumberGeneratorSW");
	}

	std::vector<uint8_t> randomBytes(_size);

	NTSTATUS status = BCryptGenRandom(nullptr, randomBytes.data(), static_cast<ULONG>(randomBytes.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	
	if (!BCRYPT_SUCCESS(status))
	{
		throw Exception::Runtime("Random number generation failed");
	}

	return randomBytes;
}

#endif // OT_OS_WINDOWS
