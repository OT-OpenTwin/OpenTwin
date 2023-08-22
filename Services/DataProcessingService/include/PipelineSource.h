#pragma once
#include "PipelineItem.h"
#include "OpenTwinCore/Variable.h"

class PipelineSource : public PipelineItem
{
public:
	PipelineSource(std::shared_ptr<EntityBlock> blockEntity);
	genericDataBlock CreateData(genericDataBlock& parameter) const;

private:
	//SourceBlockHandler _handler;
};
