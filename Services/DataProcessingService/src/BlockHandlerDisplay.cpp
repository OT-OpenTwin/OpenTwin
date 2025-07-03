#include "BlockHandlerDisplay.h"
#include "StringConverter.h"
#include "OTCore/String.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

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
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);  // PrettyWriter = indented JSON
		data.Accept(writer);
		displayMessage += "Data: \n" + std::string(buffer.GetString());

		buffer.Clear();
		ot::JsonValue& metadata =incomming->second->getMetadata();
		metadata.Accept(writer);
		displayMessage += "\nMetadata:\n" + std::string(buffer.GetString());
		_uiComponent->displayMessage(displayMessage + "\n");
	}
	else
	{
		_uiComponent->displayMessage(getErrorDataPipelineNllptr() + "\n");
	}
	return true;
}
