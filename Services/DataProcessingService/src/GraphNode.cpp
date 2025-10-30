// @otlicense
// File: GraphNode.cpp
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

#include "GraphNode.h"

GraphNode::GraphNode(int nodeID)
	:_nodeID(nodeID)
{}

void GraphNode::addSucceedingNode(const std::shared_ptr<GraphNode> node, EdgeInfo edgeInfo)
{
	_succeedingNodes.push_back(node);
	_succeedingNodesWithEdgeInfo.push_back(std::make_pair<>(node, edgeInfo));
}

void GraphNode::addPreviousNode(const std::shared_ptr<GraphNode> node)
{
	_previousNodes.push_back(node);
}

int GraphNode::getRankIncomming() const
{
	return static_cast<int>(_previousNodes.size());
}

int GraphNode::getRankOutgoing() const
{
	return static_cast<int>(_succeedingNodes.size());
}

const std::list<std::shared_ptr<GraphNode>>& GraphNode::getSucceedingNodes() const
{
	return _succeedingNodes;
}

const std::list<std::pair<std::shared_ptr<GraphNode>, EdgeInfo>>& GraphNode::getSucceedingNodesWithEdgeInfo() const
{
	return _succeedingNodesWithEdgeInfo;
}

const std::list<std::shared_ptr<GraphNode>>& GraphNode::getPreviousNodes() const
{
	return _previousNodes;
}
