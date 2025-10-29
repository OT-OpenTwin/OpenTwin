// @otlicense

#pragma once
#include <gtest/gtest.h>
#include "BlockEntityGraph.h"

class FixtureEntityBlockGraph : public testing::Test
{
public:
	BlockEntityGraph getGraphWithoutCycle();
};
