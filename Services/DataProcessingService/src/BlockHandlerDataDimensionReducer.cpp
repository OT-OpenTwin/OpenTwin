#include "BlockHandlerDataDimensionReducer.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

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
	GenericDataList& genericDataBlocks = _dataPerPort[_inputConnectorName];

	ot::GenericDataStructVector* filteredData = new ot::GenericDataStructVector(static_cast<uint32_t>(genericDataBlocks.size()));
	uint32_t count(0);
	for (const auto& genericData : genericDataBlocks)
	{
		auto matrixData = dynamic_cast<ot::GenericDataStructMatrix*>(genericData);
		if (matrixData == nullptr)
		{
			throw std::exception((_errorMessageBase + _blockTypeName + " requires a matrix as input.").c_str());
		}
		
		const ot::Variable& value = matrixData->getValue(_column, _row);
		
		filteredData->setValue(count, value);
	}

	_dataPerPort[_outputConnectorName] = { filteredData };
	return true;
}
