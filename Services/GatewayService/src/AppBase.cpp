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

AppBase::AppBase() 
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GatewayService, OT_INFO_SERVICE_TYPE_GatewayService)
{
	
}

AppBase::~AppBase() 
{
	
}