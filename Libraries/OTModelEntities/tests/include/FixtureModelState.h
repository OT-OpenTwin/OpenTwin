// @otlicense
#pragma once
#include "OTModelEntities/ModelState.h"
#include "gtest/gtest.h"

class FixtureModelState : public testing::Test
{
public:
	std::string getPrecedingVersion(const std::string& _currentVersion);
	std::string getProceedingVersion(const std::string& _currentVersion, const std::string& _targetVersion);
};
