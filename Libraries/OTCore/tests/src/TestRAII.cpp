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
#include "OTCore/RAII/AtomicDecrementRAII.h"
#include "OTCore/RAII/AtomicIncrementRAII.h"
#include "OTCore/RAII/AtomicResetRAII.h"
#include "OTCore/RAII/AtomicValueRAII.h"
#include "OTCore/RAII/CheckpointRAII.h"
#include "OTCore/RAII/CounterRAII.h"
#include "OTCore/RAII/FlagSetRAII.h"
#include "OTCore/RAII/FunctionRAII.h"
#include "OTCore/RAII/NumericValueRAII.h"
#include "OTCore/RAII/ValueRAII.h"

namespace ot::intern::testing
{
	enum class RAIITestFlag
	{
		Zero = 0 << 0, //! @brief 0.
		One  = 1 << 0, //! @brief 1.
		Two  = 1 << 1, //! @brief 2.
		Four = 1 << 2  //! @brief 4.
	};
	typedef ot::Flags<RAIITestFlag> RAIITestFlags;
}
OT_ADD_FLAG_FUNCTIONS(ot::intern::testing::RAIITestFlag, ot::intern::testing::RAIITestFlags)

// ###########################################################################################################################################################################################################################################################################################################################

// AtomicDecrement

TEST(RAIITests, AtomicDecrementRAII_Int)
{
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

// ###########################################################################################################################################################################################################################################################################################################################

// AtomicIncrement

TEST(RAIITests, AtomicIncrementRAII_Int)
{
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

// ###########################################################################################################################################################################################################################################################################################################################

// AtomicReset




// ###########################################################################################################################################################################################################################################################################################################################

// AtomicValue

TEST(RAIITests, AtomicValueRAII_Bool)
{
	std::atomic<bool> boolValue(false);
	{
		ot::AtomicValueRAII raii(boolValue, true);
		EXPECT_TRUE(boolValue.load());
	}
	EXPECT_FALSE(boolValue.load());
}

TEST(RAIITests, AtomicValueRAII_Int)
{
	std::atomic<int> intValue(0);
	{
		ot::AtomicValueRAII raii(intValue, 1);
		EXPECT_EQ(1, intValue.load());
	}
	EXPECT_EQ(0, intValue.load());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Checkpoint

TEST(RAIITests, CheckpointRAII)
{
	// Create dummy function that increments value and returns moved checkpoint raii
	auto createCheckpointRAII = [](int& _value) {
		ot::CheckpointRAII raii(_value);
		EXPECT_EQ(15, _value);
		_value += 5;
		EXPECT_EQ(20, _value);
		return raii;
		};

	int intValue = 0;
	{
		ot::CheckpointRAII raii1(intValue);
		intValue += 5;
		EXPECT_EQ(5, intValue);

		{
			ot::CheckpointRAII raii2(intValue);
			intValue += 10;
			EXPECT_EQ(15, intValue);

			{
				ot::CheckpointRAII raii3(createCheckpointRAII(intValue));
				EXPECT_EQ(20, intValue);
			}

			EXPECT_EQ(15, intValue);

			raii2.dismiss();
		}

		EXPECT_EQ(15, intValue);

		raii1.execute();

		EXPECT_EQ(0, intValue);

		intValue += 1;
	}

	EXPECT_EQ(1, intValue);
}


// ###########################################################################################################################################################################################################################################################################################################################

// Counter

TEST(RAIITests, CounterRAII_Decrement_Int)
{
	int intValue = 10;
	{
		auto raii = ot::raii::makeDecrementCounterRAII(intValue);
		EXPECT_EQ(9, intValue);
		{
			auto raii2 = ot::raii::makeDecrementCounterRAII(intValue);
			EXPECT_EQ(8, intValue);
		}
		EXPECT_EQ(9, intValue);
		intValue -= 5;
	}
	EXPECT_EQ(5, intValue);
}


TEST(RAIITests, CounterRAII_Increment_Int)
{
	int intValue = 0;
	{
		auto raii = ot::raii::makeIncrementCounterRAII(intValue);
		EXPECT_EQ(1, intValue);

		{
			auto raii2 = ot::raii::makeIncrementCounterRAII(intValue);
			EXPECT_EQ(2, intValue);
			{
				auto raii3(std::move(raii2));
				EXPECT_FALSE(raii2.isValid());
				EXPECT_EQ(2, intValue);
			}
			EXPECT_EQ(1, intValue);
		}
		EXPECT_EQ(1, intValue);
		intValue += 5;
		EXPECT_EQ(6, intValue);
	}
	EXPECT_EQ(5, intValue);
}


// ###########################################################################################################################################################################################################################################################################################################################

// FlagSet

TEST(RAIITests, FlagSetRAII)
{
	using namespace ot::intern::testing;
	RAIITestFlags flags(RAIITestFlag::Zero);
	{
		ot::FlagSetRAII<RAIITestFlag> raii1(flags, RAIITestFlag::One);
		{
			ot::FlagSetRAII<RAIITestFlag> raii2(flags, RAIITestFlag::Four);
			EXPECT_TRUE(flags.has(RAIITestFlag::One));
			EXPECT_FALSE(flags.has(RAIITestFlag::Two));
			EXPECT_TRUE(flags.has(RAIITestFlag::Four));
			{
				ot::FlagSetRAII<RAIITestFlag> raii3(flags, RAIITestFlag::Two);
				EXPECT_TRUE(flags.has(RAIITestFlag::One));
				EXPECT_TRUE(flags.has(RAIITestFlag::Two));
				EXPECT_TRUE(flags.has(RAIITestFlag::Four));

				raii3.execute();

				EXPECT_TRUE(flags.has(RAIITestFlag::One));
				EXPECT_FALSE(flags.has(RAIITestFlag::Two));
				EXPECT_TRUE(flags.has(RAIITestFlag::Four));
			}
			EXPECT_TRUE(flags.has(RAIITestFlag::One));
			EXPECT_FALSE(flags.has(RAIITestFlag::Two));
			EXPECT_TRUE(flags.has(RAIITestFlag::Four));
		}

		EXPECT_EQ(RAIITestFlag::One, flags.toEnum());

		raii1.dismiss();
	}

	EXPECT_EQ(RAIITestFlag::One, flags.toEnum());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Function

TEST(RAIITests, FunctionRAII)
{
	bool flag = false;
	{
		ot::FunctionRAII raii(
			[&flag]() { flag = true; }, // Constructor - set
			[&flag]() { flag = false; } // Destructor  - reset
		);
		EXPECT_TRUE(flag);
	}
	EXPECT_FALSE(flag);
}

// ###########################################################################################################################################################################################################################################################################################################################

// NumericValue

TEST(RAIITests, NumericValueRAII)
{
	int intValue = 0;
	{
		auto raii1 = ot::NumericValueRAII<int>(intValue);
		EXPECT_EQ(0, intValue);
		raii1.add(5);
		EXPECT_EQ(5, intValue);
		{
			auto raii2 = ot::NumericValueRAII<int>(intValue, 10);
			EXPECT_EQ(10, intValue);
			{
				auto raii3 = ot::NumericValueRAII<int>(intValue, 20, 15);
				EXPECT_EQ(20, intValue);
				raii3.decrement();
				EXPECT_EQ(19, intValue);
				raii3.execute();
				EXPECT_EQ(15, intValue);
			}
			EXPECT_EQ(15, intValue);
			raii2.dismiss();
		}
		EXPECT_EQ(15, intValue);
		raii1.subtract(5);
		EXPECT_EQ(10, intValue);
	}
	EXPECT_EQ(0, intValue);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Value

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
