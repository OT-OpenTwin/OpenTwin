#pragma once
#include "BlockHandler.h"
#include "EntityBlockDisplay.h"

class BlockHandlerDisplay : public BlockHandler
{
public:
	BlockHandlerDisplay(EntityBlockDisplay* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

private:
	ot::Connector _input;
	std::string _description;


};
