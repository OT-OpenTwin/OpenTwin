#include "GraphNode.h"

GraphNode::GraphNode(int nodeID)
	:_nodeID(nodeID)
{}

void GraphNode::addSucceedingNode(const std::shared_ptr<GraphNode> node)
{
	_succeedingNodes.push_back(node);
}

void GraphNode::addPreviousNode(const std::shared_ptr<GraphNode> node)
{
	_previousNodes.push_back(node);
}

int GraphNode::getRankIncomming() const
{
	return _previousNodes.size();
}

int GraphNode::getRankOutgoing() const
{
	return _succeedingNodes.size();
}

const std::list<std::shared_ptr<GraphNode>>& GraphNode::getSucceedingNodes() const
{
	return _succeedingNodes;
}

const std::list<std::shared_ptr<GraphNode>>& GraphNode::getPreviousNodes() const
{
	return _previousNodes;
}
