#pragma once
#include <gtest/gtest.h>

class FixtureNumpyAPI : public testing::Test
{
public:
	FixtureNumpyAPI();
	~FixtureNumpyAPI();

private:
	int initiateNumpy();
};
