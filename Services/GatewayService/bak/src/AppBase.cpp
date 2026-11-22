// @otlicense

// Gateway header
#include "AppBase.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

AppBase& AppBase::instance() {
	static AppBase g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

ot::ReturnMessage AppBase::handleCheckStartupCompleted(ot::JsonDocument& _doc)
{


	return ot::ReturnMessage::True;
}

AppBase::AppBase()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GatewayService, OT_INFO_SERVICE_TYPE_GatewayService)
{
	connectAction(OT_ACTION_CMD_CheckGatewayReady, this, &AppBase::handleCheckStartupCompleted);
}

AppBase::~AppBase() 
{
	
}