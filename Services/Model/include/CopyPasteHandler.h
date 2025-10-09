#pragma once

#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class CopyPasteHandler : public ot::ActionHandler
{
public:
	CopyPasteHandler();

	std::string selectedEntitiesSerialiseAction(ot::JsonDocument& _document);
	std::string pasteEntitiesAction(ot::JsonDocument& _document);

private:
	//! @brief Stores the pasted entities. For that new, unique names are created by adding a postfix "_" + str(counter). Pasted DataEntities must have their parent set!
	void storeEntities(std::map<ot::UID, EntityBase*>& _newEntitiesByName);
};
