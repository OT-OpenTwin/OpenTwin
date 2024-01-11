#include <gtest/gtest.h>
#include "PortData.h"
#include <limits>

TEST(PortData, ValueTransformationInt32)
{
	std::string value0 = std::to_string(std::numeric_limits<int32_t>::max());
		
	sp::PortData portData(1);
	portData.AddValue(value0);
	const sp::PortDataSingleEntry& frequency = portData.getFrequency();
	EXPECT_TRUE(std::holds_alternative<int32_t>(frequency));
}

TEST(PortData, ValueTransformationInt64)
{
	std::string value0 = std::to_string(std::numeric_limits<int64_t>::max());

	sp::PortData portData(1);
	portData.AddValue(value0);
	const sp::PortDataSingleEntry& frequency = portData.getFrequency();
	EXPECT_TRUE(std::holds_alternative<int64_t>(frequency));
}

TEST(PortData, ValueTransformationFloat)
{
	const float expected = 4.f;
	const std::string value0 = std::to_string(expected);

	sp::PortData portData(1);
	portData.AddValue(value0);
	const sp::PortDataSingleEntry& frequency = portData.getFrequency();
	EXPECT_TRUE(std::holds_alternative<float>(frequency));
}

TEST(PortData, ValueTransformationDouble)
{
	std::string value0 = std::to_string(std::numeric_limits<double>::max());

	sp::PortData portData(1);
	portData.AddValue(value0);
	const sp::PortDataSingleEntry& frequency = portData.getFrequency();
	EXPECT_TRUE(std::holds_alternative<double>(frequency));
}

TEST(PortData, Example1Port)
{
	std::vector<std::string> values(4);
	values[0] = "0";
	values[1] = "1";
	values[2] = "2";
	values[3] = "3";

	sp::PortData portData(1);
	for (std::string& value : values)
	{
		portData.AddValue(value);
	}
	EXPECT_EQ(std::get<int32_t>(portData.getFrequency()),0);
	auto portDataEntries =	portData.getPortDataEntries();
	EXPECT_EQ(portDataEntries.size(), 1);
	EXPECT_EQ(std::get<int32_t>(std::get<0>(portDataEntries[0])), 1);
	EXPECT_EQ(std::get<int32_t>(std::get<1>(portDataEntries[0])), 2);
}