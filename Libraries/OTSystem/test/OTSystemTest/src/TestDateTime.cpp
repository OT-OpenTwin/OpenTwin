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