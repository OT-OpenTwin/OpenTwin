#pragma once
#include <gtest/gtest.h>
#include "OTCore/TextEncoding.h"

class FixtureTextEncoding : public testing::TestWithParam<ot::TextEncoding::EncodingStandard>
{
public:
	std::vector<char> ReadFile(ot::TextEncoding::EncodingStandard encoding);

};
