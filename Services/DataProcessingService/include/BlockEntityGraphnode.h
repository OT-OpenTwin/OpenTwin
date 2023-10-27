#pragma once
#include <list>

#include "EntityBlock.h"

class BlockEntityGraphnode
{
public:
	BlockEntityGraphnode(int nodeID);
	void addSucceedingNode(const BlockEntityGraphnode* node);
	void addPreviousNode(const BlockEntityGraphnode* node);

	const std::list<const BlockEntityGraphnode*> getSucceedingNodes() const;
	const std::list<const BlockEntityGraphnode*> getPreviousNodes() const;

private:
	int _nodeID;
	std::list<const BlockEntityGraphnode*> _succeedingNodes;
	std::list<const BlockEntityGraphnode*> _previousNodes;

	std::shared_ptr<EntityBlock> _embeddedBlockEntity;
};
