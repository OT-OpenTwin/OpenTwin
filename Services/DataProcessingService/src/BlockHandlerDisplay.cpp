#include "BlockHandlerDisplay.h"
#include "StringConverter.h"
#include "OTCore/String.h"


BlockHandlerDisplay::BlockHandlerDisplay(EntityBlockDisplay* _blockEntity, const HandlerMap& _handlerMap)
	:BlockHandler(_blockEntity ,_handlerMap)
{
	m_input = _blockEntity->getConnectorInput();
	m_description = _blockEntity->getDescription();
}

bool BlockHandlerDisplay::executeSpecialized()
{
	_uiComponent->displayMessage("Executing Display Block: " + m_blockName);
	std::string displayMessage = m_description + "\n\n";

	auto incomming = m_dataPerPort.find(m_input.getConnectorName());
	if (incomming->second != nullptr)
	{
		ot::JsonValue& data = incomming->second->getData();
		const std::string dataPretty = ot::json::toPrettyString(data);
		
		displayMessage += "Data: \n" + dataPretty;

		
		ot::JsonValue& metadata =incomming->second->getMetadata();
		const std::string metaDataPretty = ot::json::toPrettyString(metadata);
		displayMessage += "\nMetadata:\n" + metaDataPretty;
		_uiComponent->displayMessage(displayMessage + "\n");
	}
	else
	{
		_uiComponent->displayMessage(getErrorDataPipelineNllptr() + "\n");
	}
	return true;
}
