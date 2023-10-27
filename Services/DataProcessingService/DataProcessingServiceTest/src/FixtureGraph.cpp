#include "FixtureGraph.h"

Graph FixtureGraph::getGraphWithCycle()
{
	Graph graph;
	
	GraphNode* zero = graph.addNode();
	GraphNode* one = graph.addNode();
	GraphNode* two = graph.addNode();
	GraphNode* three = graph.addNode();
	
	zero->addSucceedingNode(one);
	zero->addSucceedingNode(two);
	
	one->addSucceedingNode(two);
	
	two->addSucceedingNode(zero);
	two->addSucceedingNode(three);
	return graph;
}

Graph FixtureGraph::getGraphWithoutCycle()
{
	Graph graph;

	GraphNode* zero = graph.addNode();
	GraphNode* one = graph.addNode();
	GraphNode* two = graph.addNode();
	GraphNode* three = graph.addNode();

	zero->addSucceedingNode(one);
	zero->addSucceedingNode(two);

	one->addSucceedingNode(two);

	two->addSucceedingNode(three);

	return graph;
}

