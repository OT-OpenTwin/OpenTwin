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
	//_uiComponent->displayMessage("Executing Display Block: " + _blockName);
	//PipelineData& incommingPortData = _dataPerPort[m_input.getConnectorName()];

	//PipelineDataDocumentList& documentList = incommingPortData.m_data;
	//std::string displayMessage = m_description + "\n";
	//
	//for (const auto& document : documentList)
	//{
	//	displayMessage += ot::toString(document);
	//}

	//_uiComponent->displayMessage(displayMessage + "\n");
	return true;
}
