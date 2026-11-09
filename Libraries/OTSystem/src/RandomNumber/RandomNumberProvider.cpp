// @otlicense

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTSystem/RandomNumber/RandomNumberGeneratorSW.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Generator setter / getter

void ot::RandomNumberProvider::setRandomNumberGenerator(RandomNumberGenerator* _generator)
{
	if (!_generator)
	{
		throw Exception::InvalidArgument("RandomNumberProvider::setRandomNumberGenerator: _generator must not be nullptr");
	}

	auto& generator = getGenerator();
	generator.reset(_generator);
}

ot::RandomNumberGenerator* ot::RandomNumberProvider::getRandomNumberGenerator()
{
	return getGenerator().get();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Generation

uint64_t ot::RandomNumberProvider::createPositiveRandomInt64(uint64_t _maxValue)
{
	return getGenerator()->createPositiveRandomInt64(_maxValue);
}

std::vector<uint8_t> ot::RandomNumberProvider::createRandomBytes(uint64_t _size)
{
	return getGenerator()->createRandomBytes(_size);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

ot::RandomNumberProvider& ot::RandomNumberProvider::getInstance()
{
	static RandomNumberProvider instance;
	return instance;
}

std::unique_ptr<ot::RandomNumberGenerator>& ot::RandomNumberProvider::getGenerator()
{
	return getInstance().m_generator;
}

ot::RandomNumberProvider::RandomNumberProvider()
	: m_generator(new RandomNumberGeneratorSW())
{}