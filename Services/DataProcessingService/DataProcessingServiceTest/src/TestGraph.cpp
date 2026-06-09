// @otlicense
// File: TestGraph.cpp
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
#include "FixtureGraph.h"

TEST(Graph, AddingOneGraphNode)
{
	Graph graph;
	std::shared_ptr<GraphNode> node = graph.addNode();
	EXPECT_EQ(node->getNodeID(), 0);
}

TEST(Graph, AddingTwoGraphNodes)
{
	Graph graph;
	std::shared_ptr<GraphNode> nodeOne = graph.addNode();
	ASSERT_EQ(nodeOne->getNodeID(), 0);
	std::shared_ptr<GraphNode> nodeTwo = graph.addNode();
	EXPECT_EQ(nodeTwo->getNodeID(), 1);
}

TEST_F(FixtureGraph, CycleDetected)
{
	const Graph graphWithCycle = getGraphWithCycle();
	const std::list<std::shared_ptr<GraphNode>> nodes = graphWithCycle.getContainedNodes();
	bool cycleDetectedGlobal = false;
	for (const std::shared_ptr<GraphNode>& node : nodes)
	{
		const bool cycleDetected = graphWithCycle.hasCycles(node);
		cycleDetectedGlobal |= cycleDetected;
	}
	EXPECT_TRUE(cycleDetectedGlobal);
}

TEST_F(FixtureGraph, NoCycleDetected)
{
	const Graph graphWithCycle = getGraphWithoutCycle();
	const std::list<std::shared_ptr<GraphNode>> nodes = graphWithCycle.getContainedNodes();
	bool cycleDetectedGlobal = false;
	for (const std::shared_ptr<GraphNode> node : nodes)
	{
		const bool cycleDetected = graphWithCycle.hasCycles(node);
		cycleDetectedGlobal |= cycleDetected;
	}
	EXPECT_FALSE(cycleDetectedGlobal);
}