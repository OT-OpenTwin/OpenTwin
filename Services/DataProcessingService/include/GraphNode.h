// @otlicense
// File: GraphNode.h
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

#pragma once
#include <list>
#include <memory>
#include <string>

struct EdgeInfo
{
	std::string thisNodePort;
	std::string succeedingNodePort;
};

class GraphNode
{
public:
	GraphNode(int nodeID);
	virtual ~GraphNode() {};
	const int getNodeID() const { return _nodeID; }
	
	void addSucceedingNode(const std::shared_ptr<GraphNode> node, EdgeInfo edgeInfo);
	void addPreviousNode(const std::shared_ptr<GraphNode> node);

	int getRankIncomming() const;
	int getRankOutgoing() const;

	const std::list<std::shared_ptr<GraphNode>>& getSucceedingNodes() const;
	const std::list<std::pair<std::shared_ptr<GraphNode>, EdgeInfo>>& getSucceedingNodesWithEdgeInfo() const;
	const std::list<std::shared_ptr<GraphNode>>& getPreviousNodes() const;

private:
	int _nodeID;
	std::list<std::pair<std::shared_ptr<GraphNode>,EdgeInfo>> _succeedingNodesWithEdgeInfo;
	std::list<std::shared_ptr<GraphNode>> _succeedingNodes;
	std::list<std::shared_ptr<GraphNode>> _previousNodes;
};
