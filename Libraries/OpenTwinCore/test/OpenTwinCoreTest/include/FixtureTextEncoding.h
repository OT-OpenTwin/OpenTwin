#pragma once
#include <gtest/gtest.h>
#include "OpenTwinCore/TextEncoding.h"

class FixtureTextEncoding : public testing::TestWithParam<ot::TextEncoding>
{
public:
	std::vector<unsigned char> ReadFile(ot::TextEncoding encoding);

};
