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
