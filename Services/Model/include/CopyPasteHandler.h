#pragma once

#include "ActionAndFunctionHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class CopyPasteHandler
{
public:
	std::string selectedEntitiesSerialiseAction(ot::JsonDocument& _document);
	std::string pasteEntitiesAction(ot::JsonDocument& _document);

private:
	//! @brief Stores the pasted entities. For that new, unique names are created by adding a postfix "_" + str(counter). Pasted DataEntities must have their parent set!
	void storeEntities(std::map<ot::UID, EntityBase*>& _newEntitiesByName);

	ot::ActionHandleConnectorManager<CopyPasteHandler> m_connectionManager{ this, &ot::ActionDispatcher::instance() };
	ot::ActionHandleInlineConnector<CopyPasteHandler> m_blockHandlerSerialise{ this, &CopyPasteHandler::selectedEntitiesSerialiseAction, OT_ACTION_CMD_SelectedEntitiesSerialise, m_connectionManager, ot::SECURE_MESSAGE_TYPES };
	ot::ActionHandleInlineConnector<CopyPasteHandler> m_blockHandlerCopy{ this, &CopyPasteHandler::pasteEntitiesAction, OT_ACTION_CMD_PasteEntities, m_connectionManager, ot::SECURE_MESSAGE_TYPES };

};
