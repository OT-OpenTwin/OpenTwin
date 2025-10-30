// @otlicense
// File: Graph.cpp
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

#include <vector>

#include "Graph.h"

std::shared_ptr<GraphNode> Graph::addNode()
{
	_nodes.push_back(std::shared_ptr<GraphNode>(new GraphNode(_counter)));
	_counter++;
	return _nodes.back();
}

std::shared_ptr<GraphNode> Graph::getNode(int nodeID)
{
	for (auto node : _nodes)
	{
		if (node->getNodeID() == nodeID)
		{
			return node;
		}
	}
	return std::shared_ptr<GraphNode>(nullptr);
}

const bool Graph::hasCycles(const std::shared_ptr<GraphNode> startNode) const
{
	//Depth first search algorithm

	std::vector<bool> visited(_nodes.size(),false);
	std::vector<bool> recursiveStack(_nodes.size(),false);
	bool hasCycle = TraverseNodesUntilCycle(startNode, visited, recursiveStack);
	return hasCycle;
}

bool Graph::TraverseNodesUntilCycle(const std::shared_ptr<GraphNode> node, std::vector<bool>& visited, std::vector<bool>& recursiveStack) const
{
	if (visited[node->getNodeID()] == false)
	{
		visited[node->getNodeID()] = true;
		recursiveStack[node->getNodeID()] = true;
		
		const std::list<std::shared_ptr<GraphNode>> adjacentNodes = node->getSucceedingNodes();
		for (auto adjacentNode : adjacentNodes)
		{
			if (!visited[adjacentNode->getNodeID()])
			{
				if (TraverseNodesUntilCycle(adjacentNode, visited, recursiveStack))
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
	recursiveStack[node->getNodeID()] = false;
	return false;
}
