// @otlicense

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
