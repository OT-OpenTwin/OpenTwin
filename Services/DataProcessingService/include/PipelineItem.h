#pragma once
#include "EntityBlock.h"
#include "OpenTwinCore/Variable.h"
class PipelineItem
{
public:
	using genericDataBlock = std::map<std::string,std::list<ot::Variable>>;
	PipelineItem(std::shared_ptr<EntityBlock> blockEntity)
		:_blockEntity(blockEntity){}
protected:
	std::shared_ptr<EntityBlock> _blockEntity;
};
