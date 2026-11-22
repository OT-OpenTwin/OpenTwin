// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/Handler/ActionHandler.h"

class AppBase : public ot::ServiceBase, public ot::ActionHandler {
	OT_DECL_NOCOPY(AppBase)
public:
	static AppBase& instance();


	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	ot::ReturnMessage handleCheckStartupCompleted(ot::JsonDocument& _doc);


private:
	
	AppBase();
	virtual ~AppBase();
};