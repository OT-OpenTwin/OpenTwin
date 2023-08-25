#include "PipelineSink.h"
#include "EntityBlockPlot1D.h"
#include "BlockHandlerPlot1D.h"


PipelineSink::PipelineSink(std::shared_ptr<EntityBlock> blockEntity)
	:PipelineItem(blockEntity), _handler(nullptr)
{
	EntityBlockPlot1D* plotter = dynamic_cast<EntityBlockPlot1D*>(_blockEntity.get());
	if (plotter != nullptr)
	{
		_handler = new BlockHandlerPlot1D(plotter);
	}
}

void PipelineSink::ProcessData(genericDataBlock& inputData)
{
	_handler->Execute(inputData);
}
