#include "FixtureGraph.h"

Graph FixtureGraph::getGraphWithCycle()
{
	Graph graph;
	std::shared_ptr<GraphNode> zero = graph.addNode();
	std::shared_ptr<GraphNode> one = graph.addNode();
	std::shared_ptr<GraphNode> two = graph.addNode();
	std::shared_ptr<GraphNode> three = graph.addNode();
	
	zero->addSucceedingNode(one, {"",""});
	zero->addSucceedingNode(two, { "","" });
	
	one->addSucceedingNode(two, { "","" });
	
	two->addSucceedingNode(zero, { "","" });
	two->addSucceedingNode(three, { "","" });
	return graph;
}

Graph FixtureGraph::getGraphWithoutCycle()
{
	Graph graph;

	std::shared_ptr<GraphNode> zero = graph.addNode();
	std::shared_ptr<GraphNode> one = graph.addNode();
	std::shared_ptr<GraphNode> two = graph.addNode();
	std::shared_ptr<GraphNode> three = graph.addNode();

	zero->addSucceedingNode(one, { "","" });
	zero->addSucceedingNode(two, { "","" });

	one->addSucceedingNode(two, { "","" });

	two->addSucceedingNode(three, { "","" });

	return graph;
}

