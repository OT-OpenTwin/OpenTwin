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
	std::string displayMessage = m_description + "\n";

	auto incomming = m_dataPerPort.find(m_input.getConnectorName());
	if (incomming->second != nullptr)
	{
		ot::JsonValue& data = incomming->second->getData();
		
		if (data.IsArray())
		{
			auto dataArray = data.GetArray();

			const size_t numberOfEntries = dataArray.Size();
			for (uint32_t i = 0; i < numberOfEntries; i++)
			{
				ot::JsonValue& entry = dataArray[i];

				//displayMessage += ot::json::toJson(entry) + "\n";
			}
		}
		else
		{
			//displayMessage += ot::json::toJson(data);
		}
		
		displayMessage += "\nMetadata:\n";
		displayMessage +=  ot::json::toJson(incomming->second->getMetadata());
		_uiComponent->displayMessage(displayMessage + "\n");
	}
	else
	{
		_uiComponent->displayMessage(getErrorDataPipelineNllptr() + "\n");
	}
	return true;
}
