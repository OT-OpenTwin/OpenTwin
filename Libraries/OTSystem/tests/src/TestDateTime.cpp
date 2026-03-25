// @otlicense
// File: TestDateTime.cpp
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
#include "OTSystem/DateTime.h"

TEST(DateTime, SimpleConvert) {
	int64_t msec = 946684800000; // 2000-01-01 00:00:00 UTC

	std::string timestamp = ot::DateTime::timestampFromMsec(msec, ot::DateTime::SimpleUTC);
	EXPECT_EQ(timestamp, "2000-01-01 00:00:00.000");

	int64_t convertedMsec = ot::DateTime::timestampToMsec(timestamp, ot::DateTime::SimpleUTC);
	EXPECT_EQ(convertedMsec, msec);
}

TEST(DateTime, ISO8601Convert) {
	int64_t msec = 946684800000; // 2000-01-01 00:00:00 UTC

	std::string timestamp = ot::DateTime::timestampFromMsec(msec, ot::DateTime::ISO8601UTC);
	EXPECT_EQ(timestamp, "2000-01-01T00:00:00.000Z");

	int64_t convertedMsec = ot::DateTime::timestampToMsec(timestamp, ot::DateTime::ISO8601UTC);
	EXPECT_EQ(convertedMsec, msec);
}

TEST(DateTime, ValidateTime) {
	EXPECT_TRUE(ot::DateTime(2024, 2, 29, 12, 0, 0, 0).isValid()); // Valid. Leap year
	EXPECT_FALSE(ot::DateTime(2023, 2, 29, 12, 0, 0, 0).isValid()); // Not a leap year

	EXPECT_FALSE(ot::DateTime(2024, 0, 1).isValid()); // Invalid month
	EXPECT_FALSE(ot::DateTime(2024, 13, 1).isValid()); // Invalid month

	EXPECT_FALSE(ot::DateTime(2024, 2, 0).isValid()); // Invalid day
	EXPECT_FALSE(ot::DateTime(2024, 2, 30).isValid()); // Invalid day

	EXPECT_FALSE(ot::DateTime(2024, 2, 28, -1, 0, 0).isValid()); // Invalid hour
	EXPECT_FALSE(ot::DateTime(2024, 2, 28, 24, 0, 0).isValid()); // Invalid hour

	EXPECT_FALSE(ot::DateTime(2024, 2, 28, 23, -1, 0).isValid()); // Invalid minute
	EXPECT_FALSE(ot::DateTime(2024, 2, 28, 23, 60, 0).isValid()); // Invalid minute

	EXPECT_FALSE(ot::DateTime(2024, 2, 28, 23, 59, -1).isValid()); // Invalid second
	EXPECT_FALSE(ot::DateTime(2024, 2, 28, 23, 59, 60).isValid()); // Invalid second

	EXPECT_FALSE(ot::DateTime(2024, 2, 28, 23, 59, 59, -1).isValid()); // Invalid millisecond
	EXPECT_FALSE(ot::DateTime(2024, 2, 28, 23, 59, 59, 1000).isValid()); // Invalid millisecond
}

TEST(DateTime, Increment) {
	ot::DateTime dt(2023, 12, 31, 23, 59, 59, 999);
	dt.addMilliseconds(1);
	EXPECT_EQ(dt.getYear(), 2024);
	EXPECT_EQ(dt.getMonth(), 1);
	EXPECT_EQ(dt.getDay(), 1);
	EXPECT_EQ(dt.getHour(), 0);
	EXPECT_EQ(dt.getMinute(), 0);
	EXPECT_EQ(dt.getSecond(), 0);
	EXPECT_EQ(dt.getMillisecond(), 0);

	dt = ot::DateTime(2020, 2, 28);
	dt.addDays(1);
	EXPECT_EQ(dt.getYear(), 2020);
	EXPECT_EQ(dt.getMonth(), 2);
	EXPECT_EQ(dt.getDay(), 29); // Leap year
	dt.addDays(1);
	EXPECT_EQ(dt.getYear(), 2020);
	EXPECT_EQ(dt.getMonth(), 3);
	EXPECT_EQ(dt.getDay(), 1);
}

TEST(DateTime, Decrement) {
	ot::DateTime dt(2024, 1, 1, 0, 0, 0, 0);
	dt.removeMilliseconds(1);
	EXPECT_EQ(dt.getYear(), 2023);
	EXPECT_EQ(dt.getMonth(), 12);
	EXPECT_EQ(dt.getDay(), 31);
	EXPECT_EQ(dt.getHour(), 23);
	EXPECT_EQ(dt.getMinute(), 59);
	EXPECT_EQ(dt.getSecond(), 59);
	EXPECT_EQ(dt.getMillisecond(), 999);

	dt = ot::DateTime(2020, 3, 1);
	dt.removeDays(1);
	EXPECT_EQ(dt.getYear(), 2020);
	EXPECT_EQ(dt.getMonth(), 2);
	EXPECT_EQ(dt.getDay(), 29); // Leap year
	dt.removeDays(1);
	EXPECT_EQ(dt.getYear(), 2020);
	EXPECT_EQ(dt.getMonth(), 2);
	EXPECT_EQ(dt.getDay(), 28);
}

TEST(DateTime, OperatorEqual) {
	ot::DateTime dt1(2024, 6, 15, 10, 30, 45, 500);
	ot::DateTime dt2;
	dt2 = dt1;

	EXPECT_EQ(dt1, dt2);

	EXPECT_EQ(dt2.getYear(), 2024);
	EXPECT_EQ(dt2.getMonth(), 6);
	EXPECT_EQ(dt2.getDay(), 15);
	EXPECT_EQ(dt2.getHour(), 10);
	EXPECT_EQ(dt2.getMinute(), 30);
	EXPECT_EQ(dt2.getSecond(), 45);
	EXPECT_EQ(dt2.getMillisecond(), 500);
}

TEST(DateTime, OperatorLessThan) {
	ot::DateTime dt1(2024, 6, 15, 10, 30, 45, 500);
	ot::DateTime dt2(2024, 6, 15, 10, 30, 45, 600);
	EXPECT_LT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 10, 30, 46, 0);
	EXPECT_LT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 10, 31, 0, 0);
	EXPECT_LT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 11, 0, 0, 0);
	EXPECT_LT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 16, 0, 0, 0, 0);
	EXPECT_LT(dt1, dt2);
	dt2 = ot::DateTime(2024, 7, 1, 0, 0, 0, 0);
	EXPECT_LT(dt1, dt2);
	dt2 = ot::DateTime(2025, 1, 1, 0, 0, 0, 0);
	EXPECT_LT(dt1, dt2);
}

TEST(DateTime, OperatorLessEqual) {
	ot::DateTime dt1(2024, 6, 15, 10, 30, 45, 500);
	ot::DateTime dt2 = dt1;
	EXPECT_LE(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 10, 30, 45, 600);
	EXPECT_LE(dt1, dt2);
}

TEST(DateTime, OperatorGreaterThan) {
	ot::DateTime dt1(2024, 6, 15, 10, 30, 45, 500);
	ot::DateTime dt2(2024, 6, 15, 10, 30, 45, 400);
	EXPECT_GT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 10, 30, 44, 999);
	EXPECT_GT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 10, 29, 59, 999);
	EXPECT_GT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 9, 59, 59, 999);
	EXPECT_GT(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 14, 23, 59, 59, 999);
	EXPECT_GT(dt1, dt2);
	dt2 = ot::DateTime(2024, 5, 31, 23, 59, 59, 999);
	EXPECT_GT(dt1, dt2);
	dt2 = ot::DateTime(2023, 12, 31, 23, 59, 59, 999);
	EXPECT_GT(dt1, dt2);
}

TEST(DateTime, OperatorGreaterEqual) {
	ot::DateTime dt1(2024, 6, 15, 10, 30, 45, 500);
	ot::DateTime dt2 = dt1;
	EXPECT_GE(dt1, dt2);
	dt2 = ot::DateTime(2024, 6, 15, 10, 30, 45, 400);
	EXPECT_GE(dt1, dt2);
}