// @otlicense
// File: TestRAII.cpp
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

#include "gtest/gtest.h"
#include "OTCore/RAII/AtomicValueRAII.h"
#include "OTCore/RAII/AtomicDecrementRAII.h"
#include "OTCore/RAII/AtomicIncrementRAII.h"
#include "OTCore/RAII/DecrementRAII.h"
#include "OTCore/RAII/FunctionRAII.h"
#include "OTCore/RAII/IncrementRAII.h"
#include "OTCore/RAII/ValueRAII.h"

TEST(RAIITests, ValueRAII_Bool) {
	bool boolValue = false;
	{
		ot::ValueRAII<bool> raii(boolValue, true);
		EXPECT_TRUE(boolValue);
	}
	EXPECT_FALSE(boolValue);
}

TEST(RAIITests, ValueRAII_String) {
	std::string stringValue = "Initial";
	{
		ot::ValueRAII<std::string> raii(stringValue, "Temporary");
		EXPECT_EQ("Temporary", stringValue);
	}
	EXPECT_EQ("Initial", stringValue);
}

TEST(RAIITests, AtomicValueRAII_Bool) {
	std::atomic<bool> boolValue(false);
	{
		ot::AtomicValueRAII raii(boolValue, true);
		EXPECT_TRUE(boolValue.load());
	}
	EXPECT_FALSE(boolValue.load());
}

TEST(RAIITests, AtomicValueRAII_Int) {
	std::atomic<int> intValue(0);
	{
		ot::AtomicValueRAII raii(intValue, 1);
		EXPECT_EQ(1, intValue.load());
	}
	EXPECT_EQ(0, intValue.load());
}

TEST(RAIITests, IncrementRAII_Int) {
	int intValue = 0;
	{
		ot::IncrementRAII<int> raii(intValue);
		EXPECT_EQ(1, intValue);
		{
			ot::IncrementRAII<int> raii2(intValue);
			EXPECT_EQ(2, intValue);
		}
		EXPECT_EQ(1, intValue);
		intValue += 5;
	}
	EXPECT_EQ(5, intValue);
}

TEST(RAIITests, AtomicIncrementRAII_Int) {
	std::atomic_int32_t intValue = 0;
	{
		ot::AtomicIncrementRAII raii(intValue);
		EXPECT_EQ(1, intValue);
		{
			ot::AtomicIncrementRAII raii2(intValue);
			EXPECT_EQ(2, intValue);
		}
		EXPECT_EQ(1, intValue);
		intValue += 5;
	}
	EXPECT_EQ(5, intValue);
}

TEST(RAIITests, DecrementRAII_Int) {
	int intValue = 10;
	{
		ot::DecrementRAII<int> raii(intValue);
		EXPECT_EQ(9, intValue);
		{
			ot::DecrementRAII<int> raii2(intValue);
			EXPECT_EQ(8, intValue);
		}
		EXPECT_EQ(9, intValue);
		intValue -= 5;
	}
	EXPECT_EQ(5, intValue);
}

TEST(RAIITests, AtomicDecrementRAII_Int) {
	std::atomic_int32_t intValue = 10;
	{
		ot::AtomicDecrementRAII raii(intValue);
		EXPECT_EQ(9, intValue);
		{
			ot::AtomicDecrementRAII raii2(intValue);
			EXPECT_EQ(8, intValue);
		}
		EXPECT_EQ(9, intValue);
		intValue -= 5;
	}
	EXPECT_EQ(5, intValue);
}

TEST(RAIITests, FunctionRAII) {
	bool flag = false;
	{
		ot::FunctionRAII raii([&flag]() { flag = true; }, [&flag]() { flag = false; });
		EXPECT_TRUE(flag);
	}
	EXPECT_FALSE(flag);
}

