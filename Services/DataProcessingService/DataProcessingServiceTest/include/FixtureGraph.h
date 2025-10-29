// @otlicense

#pragma once
#include <gtest/gtest.h>
#include "Graph.h"
#include "GraphNode.h"

class FixtureGraph : public testing::Test
{
public:
	Graph getGraphWithCycle();
	Graph getGraphWithoutCycle();
	GraphNode* getRootNode();
};
