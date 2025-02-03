#include "BlockHandlerDataDimensionReducer.h"
#include "OTCore/GenericDataStructSingle.h"


BlockHandlerDataDimensionReducer::BlockHandlerDataDimensionReducer(EntityBlockDataDimensionReducer* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity, handlerMap)
{
	m_matrixEntry.m_column = blockEntity->getInputColumn() -1; //Switching from 1 based displayed index to zero based
	m_matrixEntry.m_row = blockEntity->getInputRow() - 1;//Switching from 1 based displayed index to zero based
	m_inputConnectorName = blockEntity->getInputConnector()->getConnectorName();
	m_outputConnectorName = blockEntity->getOutputConnector()->getConnectorName();
}

bool BlockHandlerDataDimensionReducer::executeSpecialized()
{
	_uiComponent->displayMessage("Executing Data Dimension Reduction Block: " + _blockName);
	PipelineData& incommingPortData = _dataPerPort[m_inputConnectorName];
	PipelineDataDocumentList& pipelineDataDocuments = incommingPortData.m_data;
	if (incommingPortData.m_quantity == nullptr)
	{
		throw std::exception("Only quantities are possibly stored as matrix.");
	}
	PipelineDataDocumentList filteredDocumentList;
	for (const auto& pipelineDocument : pipelineDataDocuments)
	{
		auto matrixData = dynamic_cast<ot::GenericDataStructMatrix*>(pipelineDocument.m_quantity.get());
		if (matrixData == nullptr)
		{
			throw std::exception((_errorMessageBase + m_blockTypeName + " requires a matrix as input.").c_str());
		}
		
		const ot::Variable& value = matrixData->getValue(m_matrixEntry);
		
		std::shared_ptr<ot::GenericDataStructSingle> filteredData (new ot::GenericDataStructSingle());
		filteredData->setValue(value);
		PipelineDataDocument filteredDocument;
		filteredDocument.m_quantity = filteredData;
		filteredDocument.m_parameter = pipelineDocument.m_parameter;
		filteredDocumentList.push_back(filteredDocument);
	}

	PipelineData outputData;
	incommingPortData.copyMetaDataReferences(outputData);
	outputData.m_data = std::move(filteredDocumentList);

	_dataPerPort[m_outputConnectorName] = outputData;
	return true;
}
