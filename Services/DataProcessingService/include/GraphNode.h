#pragma once
#include <list>


class GraphNode
{
public:
	GraphNode(int nodeID);
	
	const int getNodeID() const { return _nodeID; }
	
	void addSucceedingNode(const GraphNode* node);
	void addPreviousNode(const GraphNode* node);

	const std::list<const GraphNode*>& getSucceedingNodes() const;
	const std::list<const GraphNode*>& getPreviousNodes() const;

private:
	int _nodeID;
	std::list<const GraphNode*> _succeedingNodes;
	std::list<const GraphNode*> _previousNodes;
};
