#pragma once
#include "EntityBlock.h"
#include "OpenTwinCore/Variable.h"
class PipelineItem
{
public:
	using genericDataBlock = std::list<ot::variable_t>;
	PipelineItem(std::shared_ptr<EntityBlock> blockEntity)
		:_blockEntity(blockEntity){}
protected:
	std::shared_ptr<EntityBlock> _blockEntity;
};
