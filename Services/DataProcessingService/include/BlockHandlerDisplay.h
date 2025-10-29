// @otlicense

#pragma once
#include "BlockHandler.h"
#include "EntityBlockDisplay.h"
#include "OTCore/GenericDataStruct.h"

class BlockHandlerDisplay : public BlockHandler
{
public:
	BlockHandlerDisplay(EntityBlockDisplay* _blockEntity, const HandlerMap& _handlerMap);
	virtual bool executeSpecialized() override;

	std::string getBlockType() const override;
private:
	ot::Connector m_input;
	std::string m_description;

};
