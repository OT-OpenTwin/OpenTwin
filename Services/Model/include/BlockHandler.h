#pragma once

// OpenTwin header
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class BlockHandler : public BusinessLogicHandler {
public:
	BlockHandler();
	~BlockHandler() {};

private:
	ot::ActionHandler m_actionHandler;
	void handleItemChanged(ot::JsonDocument& _doc);
	ot::ReturnMessage handleItemDoubleClicked(ot::JsonDocument& _doc);
	void handleConnectionChanged(ot::JsonDocument& _doc);
};