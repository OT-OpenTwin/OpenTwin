#pragma once

#include "ActionAndFunctionHandler.h"
#include "OTGui/GraphicsCopyInformation.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCommunication/ActionHandleConnector.h"
#include "OTCommunication/ActionTypes.h"

class BlockHandler : public BusinessLogicHandler, public ot::ActionHandler
{
public:
	std::string selectedEntitiesSerialiseAction(ot::JsonDocument& _document);
	std::string pasteEntitiesAction(ot::JsonDocument& _document);

private:
	void copyItem(const ot::GraphicsCopyInformation* _copyInformation);
	void updateIdentifier(std::list<std::unique_ptr<EntityBase>>& _newEntities);

	ot::ActionHandleConnector<BlockHandler> m_blockHandler_Serialise = ot::ActionHandleConnector<BlockHandler>(OT_ACTION_CMD_SelectedEntitiesSerialise, ot::SECURE_MESSAGE_TYPES, this, &BlockHandler::selectedEntitiesSerialiseAction);
	ot::ActionHandleConnector<BlockHandler> m_blockHandler_Copy = ot::ActionHandleConnector<BlockHandler>(OT_ACTION_CMD_PasteEntities, ot::SECURE_MESSAGE_TYPES, this, &BlockHandler::pasteEntitiesAction);
};
