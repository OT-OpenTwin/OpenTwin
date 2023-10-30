#pragma once
#include <list>
#include <memory>

class GraphNode
{
public:
	GraphNode(int nodeID);
	virtual ~GraphNode() {};
	const int getNodeID() const { return _nodeID; }
	
	void addSucceedingNode(const std::shared_ptr<GraphNode> node);
	void addPreviousNode(const std::shared_ptr<GraphNode> node);

	int getRankIncomming() const;
	int getRankOutgoing() const;

	const std::list<std::shared_ptr<GraphNode>>& getSucceedingNodes() const;
	const std::list<std::shared_ptr<GraphNode>>& getPreviousNodes() const;

private:
	int _nodeID;
	std::list<std::shared_ptr<GraphNode>> _succeedingNodes;
	std::list<std::shared_ptr<GraphNode>> _previousNodes;
};
