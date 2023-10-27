#include "GraphNode.h"

GraphNode::GraphNode(int nodeID)
	:_nodeID(nodeID)
{}

void GraphNode::addSucceedingNode(const GraphNode* node)
{
	_succeedingNodes.push_back(node);
}

void GraphNode::addPreviousNode(const GraphNode* node)
{
	_previousNodes.push_back(node);
}

const std::list<const GraphNode*>& GraphNode::getSucceedingNodes() const
{
	return _succeedingNodes;
}

const std::list<const GraphNode*>& GraphNode::getPreviousNodes() const
{
	return _previousNodes;
}
