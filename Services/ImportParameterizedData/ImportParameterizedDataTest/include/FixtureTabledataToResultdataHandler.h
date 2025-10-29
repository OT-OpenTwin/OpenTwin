// @otlicense

#pragma once
#include <gtest/gtest.h>

class FixtureTabledataToResultdataHandler : public testing::Test
{
public:
	std::string extractUnitFromName(std::string& name);
};
