#include "BlockHandlerDataDimensionReducer.h"
#include "OTCore/GenericDataStructSingle.h"
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
	
	GenericDataList filteredData;
	for (const auto& genericData : genericDataBlocks)
	{
		auto matrixData = dynamic_cast<ot::GenericDataStructMatrix*>(genericData.get());
		if (matrixData == nullptr)
		{
			throw std::exception((_errorMessageBase + _blockTypeName + " requires a matrix as input.").c_str());
		}
		const ot::Variable& value = matrixData->getValue(_column, _row);
		std::shared_ptr<ot::GenericDataStructSingle> singleData(new ot::GenericDataStructSingle());
		singleData->setValue(value);
		filteredData.push_back(singleData);
	}

	_dataPerPort[_outputConnectorName] = filteredData;
	return true;
}
