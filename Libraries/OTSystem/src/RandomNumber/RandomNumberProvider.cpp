// @otlicense
// File: RandomNumberProvider.cpp
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