#pragma once
#include "BlockHandler.h"
#include "EntityBlockDisplay.h"
#include "OTCore/GenericDataStruct.h"

class BlockHandlerDisplay : public BlockHandler
{
public:
	BlockHandlerDisplay(EntityBlockDisplay* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

private:
	ot::Connector m_input;
	std::string m_description;
};
