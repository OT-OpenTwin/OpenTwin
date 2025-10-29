// @otlicense

#pragma once
#include "GraphNode.h"

#include <list>
#include <vector>
#include <memory>

class Graph
{
public:

	std::shared_ptr<GraphNode> addNode();
	std::shared_ptr<GraphNode> getNode(int nodeID);
	const std::list<std::shared_ptr<GraphNode>> getContainedNodes() const { return _nodes; };
	const bool hasCycles(const std::shared_ptr<GraphNode> startNode) const; //Depth first search

protected:
	int _counter = 0;
	std::list<std::shared_ptr<GraphNode>> _nodes;

	bool TraverseNodesUntilCycle(const std::shared_ptr<GraphNode> node, std::vector<bool>& visited, std::vector<bool>& recursiveStack) const;
};
