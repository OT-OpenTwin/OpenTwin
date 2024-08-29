#pragma once
#include "BlockHandler.h"
#include "EntityBlockDataDimensionReducer.h"
#include "OTCore/GenericDataStructMatrix.h"

class BlockHandlerDataDimensionReducer :public BlockHandler
{
public:
	BlockHandlerDataDimensionReducer(EntityBlockDataDimensionReducer* _blockEntity, const HandlerMap& _handlerMap);

private:
	ot::MatrixEntryPointer m_matrixEntry;
	
	std::string  m_inputConnectorName;
	std::string m_outputConnectorName;

	std::string m_blockTypeName;
	virtual bool executeSpecialized() override;
};
