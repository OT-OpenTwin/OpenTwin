#include "BlockHandlerDisplay.h"
#include "StringConverter.h"

BlockHandlerDisplay::BlockHandlerDisplay(EntityBlockDisplay* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity ,handlerMap)
{
	m_input = blockEntity->getConnectorInput();
	m_description = blockEntity->getDescription();
}

bool BlockHandlerDisplay::executeSpecialized()
{
	_uiComponent->displayMessage("Executing Display Block: " + _blockName);
	std::string displayMessage = m_description + "\n";

	auto incomming = _dataPerPort.find(m_input.getConnectorName());
	ot::ConstJsonArray data =	incomming->second->getData();
	
	for (int i = 0; i < data.Size(); i++)
	{
		displayMessage += ot::json::toJson(ot::json::getObject(data, i)) + "\n";
	}
		
	_uiComponent->displayMessage(displayMessage + "\n");
	return true;
}
