// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberGenerator.h"

// std header
#include <memory>

namespace ot
{

	//! @brief The RandomNumberProvider class provides a static interface for generating random numbers using a specified random number generator.
	class OT_SYS_API_EXPORT RandomNumberProvider
	{
	public:
		//! @brief Sets the random number generator to be used by the RandomNumberProvider.
		//! @param _generator A pointer to the random number generator to be used.
		//! The RandomNumberProvider takes ownership of the generator.
		//! Must not be nullptr.
		//! The default is a software-based random number generator (RandomNumberGeneratorSW).
		static void setRandomNumberGenerator(RandomNumberGenerator* _generator);

		//! @brief Returns a pointer to the currently set random number generator.
		//! The ownership of the generator remains with the RandomNumberProvider.
		static RandomNumberGenerator* getRandomNumberGenerator();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Generation

		//! @brief Creates a positive random integer in the range [0, _maxValue] using the currently set random number generator.
		//! @note Use this method if a single random integer is needed.
		//! For generating multiple random integers, consider using createRandomBytes() for better performance.
		//! @param _maxValue The maximum value of the random integer to be generated. Must be greater than 0.
		static uint64_t createPositiveRandomInt64(uint64_t _maxValue = std::numeric_limits<uint64_t>::max());

		//! @brief Creates a vector of random bytes of the specified size using the currently set random number generator.
		//! @param _size The number of random bytes to generate.
		static std::vector<uint8_t> createRandomBytes(uint64_t _size);

		// ###########################################################################################################################################################################################################################################################################################################################

	public:
		std::unique_ptr<RandomNumberGenerator> m_generator;

		static RandomNumberProvider& getInstance();
		static std::unique_ptr<RandomNumberGenerator>& getGenerator();

		RandomNumberProvider();
		RandomNumberProvider(const RandomNumberProvider&) = delete;
		RandomNumberProvider(RandomNumberProvider&&) = delete;

		RandomNumberProvider& operator=(const RandomNumberProvider&) = delete;
		RandomNumberProvider& operator=(RandomNumberProvider&&) = delete;

	};

}