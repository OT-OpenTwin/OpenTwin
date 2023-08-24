#pragma once
#include "BlockItemDataProcessing.h"
#include "EntityBlock.h"

#include <memory>

class BlockItemPlot1D : public BlockItemDataProcessing
{
public:
	BlockItemPlot1D();
	virtual void AddConnectors(ot::GraphicsFlowItemCfg* block);
	static std::shared_ptr<EntityBlock> CreateBlockEntity();

};
