#pragma once
// Service header
#include "BlockHandler.h"
#include "ActionAndFunctionHandler.h"


// OpenTwin header
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"



class BlockHandler : public ActionAndFunctionHandler, public BusinessLogicHandler {

public:

	BlockHandler() {};
	~BlockHandler() {};

	void updateBlock(ot::JsonDocument& _doc);

protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) override;
};