#pragma once
#include "PipelineItem.h"
#include "BlockHandler.h"

class PipelineSink : public PipelineItem
{
public:
	PipelineSink(std::shared_ptr<EntityBlock> blockEntity);
	void ProcessData(genericDataBlock& inputData);

private:
	BlockHandler* _handler;
};
