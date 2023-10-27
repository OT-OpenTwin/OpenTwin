#pragma once
#include "GraphNode.h"

#include <list>

class Graph
{
public:

	GraphNode* addNode();
	//GraphNode* getNode(int nodeID);
	const std::list<GraphNode>& getContainedNodes() const { return _nodes; };
	const bool hasCycles(const GraphNode& startNode) const; //Depth first search

private:
	int _counter = 0;
	std::list<GraphNode> _nodes;

	bool TraverseNodesUntilCycle(const GraphNode& node, std::vector<bool>& visited, std::vector<bool>& recursiveStack) const;
};
