#include "PipelineSink.h"
#include "EntityBlockPlot1D.h"
#include "BlockHandlerPlot1D.h"


PipelineSink::PipelineSink(std::shared_ptr<EntityBlock> blockEntity)
	:PipelineItem(blockEntity), _handler(nullptr)
{
	EntityBlockPlot1D* plotter = dynamic_cast<EntityBlockPlot1D*>(_blockEntity.get());
	if (plotter != nullptr)
	{
		_handler = new BlockHandlerPlot1D(plotter, "C1", "C0"); //Should come from the BlockPlot1D entity
	}
}

void PipelineSink::ProcessData(genericDataBlock& inputData)
{
	_handler->Execute(inputData);
}

void PipelineSink::setConnectorAssoziation(const std::string& otherCon, const std::string& inCon)
{
	_handler->addConnectorAssoziation(otherCon, inCon);
}
