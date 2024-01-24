#pragma once
#include "BlockHandler.h"
#include "EntityBlockDataDimensionReducer.h"

class BlockHandlerDataDimensionReducer :public BlockHandler
{
public:
	BlockHandlerDataDimensionReducer(EntityBlockDataDimensionReducer* blockEntity, const HandlerMap& handlerMap);

private:
	uint32_t _column;
	uint32_t _row;
	std::string  _inputConnectorName;
	std::string _outputConnectorName;

	std::string _blockTypeName;
	virtual bool executeSpecialized() override;
};
