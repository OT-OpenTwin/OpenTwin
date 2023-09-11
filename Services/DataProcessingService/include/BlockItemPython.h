#pragma once
#include "BlockItemDataProcessing.h"
#include "EntityBlockPython.h"

class BlockItemPython : public BlockItemDataProcessing
{
public:
	BlockItemPython();
	void AddConnectors(ot::GraphicsFlowItemCfg* block);
	static std::shared_ptr<EntityBlock> CreateBlockEntity();
};
