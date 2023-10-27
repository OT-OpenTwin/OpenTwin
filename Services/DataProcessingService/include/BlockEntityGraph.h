#pragma once
#include "BlockEntityGraphnode.h"

#include <list>

class BlockEntityGraph
{
public:
	BlockEntityGraphnode* addNode(std::shared_ptr<EntityBlock> embeddedEntity);
	BlockEntityGraphnode* getNode(int nodeID);
	const std::list<BlockEntityGraphnode>& getContainedNodes;
	bool hasCycles(const BlockEntityGraphnode& startNode); //Depth first search

private:
	std::list<BlockEntityGraphnode> _nodes;

};
