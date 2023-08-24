#pragma once
#include "BlockItemDataProcessing.h"
#include "EntityBlock.h"

class BlockItemDatabaseAccess : public BlockItemDataProcessing
{
public:
	BlockItemDatabaseAccess();
	static std::shared_ptr<EntityBlock> CreateBlockEntity();
private:
	void AddConnectors(ot::GraphicsFlowItemCfg* block) override;
};
