#include <gtest/gtest.h>
#include "FixtureGraph.h"

TEST(Graph, AddingOneGraphNode)
{
	Graph graph;
	GraphNode* node = graph.addNode();
	EXPECT_EQ(node->getNodeID(), 0);
}

TEST(Graph, AddingTwoGraphNodes)
{
	Graph graph;
	GraphNode* nodeOne = graph.addNode();
	ASSERT_EQ(nodeOne->getNodeID(), 0);
	GraphNode* nodeTwo = graph.addNode();
	EXPECT_EQ(nodeTwo->getNodeID(), 1);
}

TEST_F(FixtureGraph, CycleDetected)
{
	const Graph graphWithCycle = getGraphWithCycle();
	const std::list<GraphNode> nodes = graphWithCycle.getContainedNodes();
	bool cycleDetectedGlobal = false;
	for (const GraphNode& node : nodes)
	{
		const bool cycleDetected = graphWithCycle.hasCycles(node);
		cycleDetectedGlobal |= cycleDetected;
	}
	EXPECT_TRUE(cycleDetectedGlobal);
}

TEST_F(FixtureGraph, NoCycleDetected)
{
	const Graph graphWithCycle = getGraphWithoutCycle();
	const std::list<GraphNode> nodes = graphWithCycle.getContainedNodes();
	bool cycleDetectedGlobal = false;
	for (const GraphNode& node : nodes)
	{
		const bool cycleDetected = graphWithCycle.hasCycles(node);
		cycleDetectedGlobal |= cycleDetected;
	}
	EXPECT_FALSE(cycleDetectedGlobal);
}