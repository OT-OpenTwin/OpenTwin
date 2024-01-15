#include "BlockHandlerDataDimensionReducer.h"

BlockHandlerDataDimensionReducer::BlockHandlerDataDimensionReducer(EntityBlockDataDimensionReducer* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(handlerMap)
{
	_column = blockEntity->getInputColumn();
	_row = blockEntity->getInputRow();
	_inputConnectorName = blockEntity->getInputConnector()->getConnectorName();
	_outputConnectorName = blockEntity->getOutputConnector()->getConnectorName();
}

bool BlockHandlerDataDimensionReducer::executeSpecialized()
{
	std::list<GenericDataBlock>& genericDataBlocks = _dataPerPort[_inputConnectorName];
	
	std::list<GenericDataBlock> filteredData;
	for (GenericDataBlock& genericDataBlock : genericDataBlocks)
	{
		filteredData.push_back(GenericDataBlock(1, 1));
		const ot::Variable& value =	genericDataBlock.getValue(_column, _row);
		filteredData.back().setValue(0, 0, value);
	}

	_dataPerPort[_outputConnectorName] = filteredData;
	return true;
}
