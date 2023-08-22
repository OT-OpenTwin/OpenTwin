#pragma once
#include "PipelineItem.h"

class PipelineSink : public PipelineItem
{
public:
	PipelineSink(std::shared_ptr<EntityBlock> blockEntity);
	void ProcessData(genericDataBlock& inputData);
};
