#pragma once
#include "BlockItemDataProcessing.h"
#include "EntityBlock.h"

class BlockItemDatabaseAccess : public BlockItemDataProcessing
{
public:
	enum QueryDimension {d1,d2,d3};
	BlockItemDatabaseAccess(QueryDimension dimension = d1);
	static std::shared_ptr<EntityBlock> CreateBlockEntity();
private:
	QueryDimension _queryDimension;
	void AddConnectors(ot::GraphicsFlowItemCfg* block) override;
};
