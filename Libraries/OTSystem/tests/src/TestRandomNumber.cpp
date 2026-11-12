// @otlicense
// File: TestRandomNumber.cpp
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

#include <cstdint>
#include <set>
#include <gtest/gtest.h>
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTSystem/RandomNumber/RandomNumberGeneratorSW.h"
#include "OTSystem/Exception.h"

TEST(RandomNumberTest, BytesHaveRequestedSize)
{
    ot::RandomNumberGeneratorSW gen;
    EXPECT_EQ(gen.createRandomBytes(1).size(), 1u);
    EXPECT_EQ(gen.createRandomBytes(1000).size(), 1000u);
}

TEST(RandomNumberTest, LargeBufferIsFilled) {
    ot::RandomNumberGeneratorSW gen;
    const uint64_t size = 1 << 20;
    std::vector<uint8_t> bytes = gen.createRandomBytes(size);
    ASSERT_EQ(bytes.size(), size);

    bool nonZero = false;
    for (size_t i = size - 1024; i < size; i++)
    {
        if (bytes[i] != 0)
        {
            nonZero = true;
            break;
        }
    }
    EXPECT_TRUE(nonZero);
}

TEST(RandomNumberTest, BytesDifferBetweenCalls)
{
    ot::RandomNumberGeneratorSW gen;
    EXPECT_NE(gen.createRandomBytes(32), gen.createRandomBytes(32));
}

TEST(RandomNumberTest, ZeroSizeThrows)
{
    ot::RandomNumberGeneratorSW gen;
    EXPECT_THROW(gen.createRandomBytes(0), ot::Exception::OutOfBounds);
}

TEST(RandomNumberTest, IntStaysInRange)
{
    ot::RandomNumberGeneratorSW gen;
    std::set<uint64_t> seen;

    for (int i = 0; i < 1000; i++)
    {
        uint64_t value = gen.createPositiveRandomInt64(10);
        ASSERT_LE(value, 10u);
        seen.insert(value);
    }

    // All 11 values should appear in 1000 draws
    EXPECT_EQ(seen.size(), 11u);
}

TEST(RandomNumberTest, IntMaxValueOne)
{
    ot::RandomNumberGeneratorSW gen;
    std::set<uint64_t> seen;
    for (int i = 0; i < 100; i++)
    {
        uint64_t value = gen.createPositiveRandomInt64(1);
        ASSERT_LE(value, 1u);
        seen.insert(value);
    }

    EXPECT_EQ(seen.size(), 2u);
}

TEST(RandomNumberTest, IntFullRange)
{
    ot::RandomNumberGeneratorSW gen;
    EXPECT_NE(gen.createPositiveRandomInt64(), gen.createPositiveRandomInt64());
}

TEST(RandomNumberTest, IntZeroMaxThrows)
{
    ot::RandomNumberGeneratorSW gen;
    EXPECT_THROW(gen.createPositiveRandomInt64(0), ot::Exception::OutOfBounds);
}

TEST(RandomNumberTest, ProviderUsesDefaultGenerator)
{
    ASSERT_NE(ot::RandomNumberProvider::getRandomNumberGenerator(), nullptr);
    EXPECT_EQ(ot::RandomNumberProvider::createRandomBytes(16).size(), 16u);
    EXPECT_LE(ot::RandomNumberProvider::createPositiveRandomInt64(5), 5u);
}

TEST(RandomNumberTest, ProviderRejectsNullGenerator)
{
    EXPECT_THROW(ot::RandomNumberProvider::setRandomNumberGenerator(nullptr), ot::Exception::InvalidArgument);

    // Set default generator must still be in place after rejected call
    EXPECT_NE(ot::RandomNumberProvider::getRandomNumberGenerator(), nullptr);
}
