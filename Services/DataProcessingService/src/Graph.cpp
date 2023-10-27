#include <vector>

#include "Graph.h"

GraphNode* Graph::addNode()
{
	_nodes.push_back(GraphNode(_counter));
	_counter++;
	return &_nodes.back();
}

const bool Graph::hasCycles(const GraphNode& startNode) const
{
	//Depth first search algorithm

	std::vector<bool> visited(_nodes.size(),false);
	std::vector<bool> recursiveStack(_nodes.size(),false);
	bool hasCycle = TraverseNodesUntilCycle(startNode, visited, recursiveStack);
	return hasCycle;
}

bool Graph::TraverseNodesUntilCycle(const GraphNode& node, std::vector<bool>& visited, std::vector<bool>& recursiveStack) const
{
	if (visited[node.getNodeID()] == false)
	{
		visited[node.getNodeID()] = true;
		recursiveStack[node.getNodeID()] = true;
		
		const std::list<const GraphNode*> adjacentNodes = node.getSucceedingNodes();
		for (auto adjacentNode : adjacentNodes)
		{
			if (!visited[adjacentNode->getNodeID()])
			{
				if (TraverseNodesUntilCycle(*adjacentNode, visited, recursiveStack))
				{
					return true;
				}
			}
			else if (recursiveStack[adjacentNode->getNodeID()])
			{
				return true;
			}
		}
	}
	recursiveStack[node.getNodeID()] = false;
	return false;
}
