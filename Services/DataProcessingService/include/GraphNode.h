// @otlicense

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
