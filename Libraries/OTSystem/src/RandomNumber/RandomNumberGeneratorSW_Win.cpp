// @otlicense

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

uint64_t ot::RandomNumberGeneratorSW::createPositiveRandomInt64(uint64_t maxValue)
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