#pragma once

#include "ActionAndFunctionHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class BlockHandler : public BusinessLogicHandler {
public:
	std::string selectedEntitiesSerialiseAction(ot::JsonDocument& _document);
	std::string pasteEntitiesAction(ot::JsonDocument& _document);

private:
	void updateIdentifier(std::list<std::unique_ptr<EntityBase>>& _newEntities);
	void storeEntities(std::list<std::unique_ptr<EntityBase>>& _newEntities);
	ot::ActionHandleConnectorManager<BlockHandler> m_connectionManager{ this, &ot::ActionDispatcher::instance() };
	ot::ActionHandleInlineConnector<BlockHandler> m_blockHandlerSerialise{ this, &BlockHandler::selectedEntitiesSerialiseAction, OT_ACTION_CMD_SelectedEntitiesSerialise, ot::SECURE_MESSAGE_TYPES, m_connectionManager };
	ot::ActionHandleInlineConnector<BlockHandler> m_blockHandlerCopy{ this, &BlockHandler::pasteEntitiesAction, OT_ACTION_CMD_PasteEntities, ot::SECURE_MESSAGE_TYPES, m_connectionManager };

};
