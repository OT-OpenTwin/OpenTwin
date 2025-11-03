// @otlicense
// File: TestFlags.cpp
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

#include <gtest/gtest.h>
#include "OTSystem/Flags.h"

enum class TestEnum : int64_t {
	None        = 0 << 0,
	FlagA       = 1 << 0,
	FlagB       = 1 << 1,
	FlagC       = 1 << 2,
	AllFlags    = FlagA | FlagB | FlagC
};
typedef ot::Flags<TestEnum> TestFlags;
OT_ADD_FLAG_FUNCTIONS(TestEnum, TestFlags)



TEST(FlagsTest, DefaultConstructor) {
	TestFlags flags;
	EXPECT_TRUE(flags.none());
	EXPECT_FALSE(flags.any());
	EXPECT_EQ(flags.underlying(), 0Ui64);
}

TEST(FlagsTest, ConstructFromEnum) {
	TestFlags flags(TestEnum::FlagA);
	EXPECT_TRUE(flags.any());
	EXPECT_TRUE(flags.has(TestEnum::FlagA));
	EXPECT_FALSE(flags.has(TestEnum::FlagB));
}

TEST(FlagsTest, ConstructFromUnderlyingValue) {
	TestFlags flags(static_cast<uint64_t>(TestEnum::FlagA) | static_cast<uint64_t>(TestEnum::FlagC));
	EXPECT_TRUE(flags.has(TestEnum::FlagA));
	EXPECT_TRUE(flags.has(TestEnum::FlagC));
	EXPECT_FALSE(flags.has(TestEnum::FlagB));
}

// -----------------------------------------------------------------------------
// Bitwise Operations
// -----------------------------------------------------------------------------
TEST(FlagsTest, BitwiseOrEnum) {
	TestFlags flags = TestEnum::FlagA | TestEnum::FlagB;
	EXPECT_TRUE(flags.has(TestEnum::FlagA));
	EXPECT_TRUE(flags.has(TestEnum::FlagB));
	EXPECT_FALSE(flags.has(TestEnum::FlagC));
}

TEST(FlagsTest, BitwiseOrFlags) {
	TestFlags a(TestEnum::FlagA);
	TestFlags b(TestEnum::FlagB);
	TestFlags c = a | b;
	EXPECT_TRUE(c.has(TestEnum::FlagA));
	EXPECT_TRUE(c.has(TestEnum::FlagB));
	EXPECT_FALSE(c.has(TestEnum::FlagC));
}

TEST(FlagsTest, BitwiseAnd) {
	TestFlags flags = TestEnum::FlagA | TestEnum::FlagB;
	EXPECT_TRUE((flags & TestEnum::FlagA).any());
	EXPECT_FALSE((flags & TestEnum::FlagC).any());
}

TEST(FlagsTest, BitwiseXor) {
	TestFlags flags = TestEnum::FlagA | TestEnum::FlagB;
	TestFlags xorFlags = flags ^ TestEnum::FlagA;
	EXPECT_FALSE(xorFlags.has(TestEnum::FlagA));
	EXPECT_TRUE(xorFlags.has(TestEnum::FlagB));
}

TEST(FlagsTest, BitwiseNot) {
	TestFlags flags(TestEnum::FlagA);
	TestFlags notFlags = ~flags;
	EXPECT_FALSE(notFlags.has(TestEnum::FlagA));
	EXPECT_TRUE(notFlags.has(TestEnum::FlagB));
	EXPECT_TRUE(notFlags.has(TestEnum::FlagC));
}

// -----------------------------------------------------------------------------
// Modifiers
// -----------------------------------------------------------------------------
TEST(FlagsTest, SetAndClear) {
	TestFlags flags;
	flags.set(TestEnum::FlagA);
	EXPECT_TRUE(flags.has(TestEnum::FlagA));

	flags.set(TestEnum::FlagB);
	EXPECT_TRUE(flags.has(TestEnum::FlagB));

	flags.remove(TestEnum::FlagA);
	EXPECT_FALSE(flags.has(TestEnum::FlagA));
	EXPECT_TRUE(flags.has(TestEnum::FlagB));
}

TEST(FlagsTest, ResetAll) {
	TestFlags flags(TestEnum::AllFlags);
	EXPECT_TRUE(flags.any());
	flags.clear();
	EXPECT_TRUE(flags.none());
	EXPECT_FALSE(flags.any());
}

// -----------------------------------------------------------------------------
// Queries and Comparisons
// -----------------------------------------------------------------------------
TEST(FlagsTest, AnyNone) {
	TestFlags flags;
	EXPECT_TRUE(flags.none());
	flags.set(TestEnum::FlagA);
	EXPECT_TRUE(flags.any());
}

TEST(FlagsTest, EqualityAndInequality) {
	TestFlags a = TestEnum::FlagA | TestEnum::FlagB;
	TestFlags b = TestEnum::FlagA | TestEnum::FlagB;
	TestFlags c = TestEnum::FlagA;

	EXPECT_EQ(a, b);
	EXPECT_NE(a, c);
}

TEST(FlagsTest, ComparisonWithEnum) {
	TestFlags flags(TestEnum::FlagA);
	EXPECT_TRUE(flags == TestEnum::FlagA);
	EXPECT_FALSE(flags == TestEnum::FlagB);
}

// -----------------------------------------------------------------------------
// Conversion and Operator bool()
// -----------------------------------------------------------------------------
TEST(FlagsTest, UnderlyingValueConversion) {
	TestFlags flags = TestEnum::FlagA | TestEnum::FlagC;
	uint64_t value = static_cast<uint64_t>(flags);
	EXPECT_EQ(value, static_cast<uint64_t>(TestEnum::FlagA) | static_cast<uint64_t>(TestEnum::FlagC));
}

TEST(FlagsTest, BoolOperator) {
	TestFlags flags;
	EXPECT_FALSE(static_cast<bool>(flags));

	flags.set(TestEnum::FlagB);
	EXPECT_TRUE(static_cast<bool>(flags));
	EXPECT_TRUE(flags); // implicit bool check
}

// -----------------------------------------------------------------------------
// Compound Assignment Operators
// -----------------------------------------------------------------------------
TEST(FlagsTest, CompoundOr) {
	TestFlags flags;
	flags |= TestEnum::FlagA;
	EXPECT_TRUE(flags.has(TestEnum::FlagA));
}

TEST(FlagsTest, CompoundAnd) {
	TestFlags flags = TestEnum::FlagA | TestEnum::FlagB;
	flags &= TestEnum::FlagA;
	EXPECT_TRUE(flags.has(TestEnum::FlagA));
	EXPECT_FALSE(flags.has(TestEnum::FlagB));
}

TEST(FlagsTest, CompoundXor) {
	TestFlags flags = TestEnum::FlagA | TestEnum::FlagB;
	flags ^= TestEnum::FlagB;
	EXPECT_TRUE(flags.has(TestEnum::FlagA));
	EXPECT_FALSE(flags.has(TestEnum::FlagB));
}

// -----------------------------------------------------------------------------
// Edge Cases
// -----------------------------------------------------------------------------
TEST(FlagsTest, NoneEnumConstant) {
	TestFlags flags(TestEnum::None);
	EXPECT_TRUE(flags.none());
	EXPECT_FALSE(flags.any());
}

TEST(FlagsTest, AllEnumConstant) {
	TestFlags flags(TestEnum::AllFlags);
	EXPECT_TRUE(flags.any());
	EXPECT_FALSE(flags.none());
}

TEST(FlagsTest, CombinationAndComparison) {
	TestFlags flags = TestEnum::FlagA | TestEnum::FlagB;
	EXPECT_TRUE((flags & TestEnum::FlagA));
	EXPECT_TRUE((flags & TestEnum::FlagB));
	EXPECT_FALSE((flags & TestEnum::FlagC));
}