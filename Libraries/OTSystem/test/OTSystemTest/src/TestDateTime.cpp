// @otlicense

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