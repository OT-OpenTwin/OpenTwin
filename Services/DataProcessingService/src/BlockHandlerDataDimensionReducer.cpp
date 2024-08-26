#include "BlockHandlerDataDimensionReducer.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

BlockHandlerDataDimensionReducer::BlockHandlerDataDimensionReducer(EntityBlockDataDimensionReducer* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity, handlerMap)
{
	_column = blockEntity->getInputColumn() -1; //Switching from 1 based displayed index to zero based
	_row = blockEntity->getInputRow() - 1;//Switching from 1 based displayed index to zero based
	_inputConnectorName = blockEntity->getInputConnector()->getConnectorName();
	_outputConnectorName = blockEntity->getOutputConnector()->getConnectorName();
}

bool BlockHandlerDataDimensionReducer::executeSpecialized()
{
	PipelineData& incommingPortData = _dataPerPort[_inputConnectorName];
	auto& genericDataBlocks = incommingPortData.m_data;

	std::shared_ptr<ot::GenericDataStructVector> filteredData (new ot::GenericDataStructVector(static_cast<uint32_t>(genericDataBlocks.size())));
	uint32_t count(0);
	for (const auto& genericData : genericDataBlocks)
	{
		auto matrixData = dynamic_cast<ot::GenericDataStructMatrix*>(genericData.get());
		if (matrixData == nullptr)
		{
			throw std::exception((_errorMessageBase + _blockTypeName + " requires a matrix as input.").c_str());
		}
		
		const ot::Variable& value = matrixData->getValue(_column, _row);
		
		filteredData->setValue(count, value);
		count++;
	}

	PipelineData outputData;
	incommingPortData.copyMetaDataReferences(outputData);
	outputData.m_data = { filteredData };

	_dataPerPort[_outputConnectorName] = outputData;
	return true;
}
