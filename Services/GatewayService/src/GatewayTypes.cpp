// @otlicense

// Gateway header
#include "GatewayTypes.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"

std::string ot::Gateway::toString(GatewayActionType _action)
{
	switch (_action)
	{
	case GatewayActionType::Unknown: return "Unknown";
	case GatewayActionType::Ping: return "Ping";
	case GatewayActionType::Shutdown: return "Shutdown";
	case GatewayActionType::SendToService: return "SendToService";
	case GatewayActionType::QueueToService: return "QueueToService";
	default:
		OT_LOG_ES("Unknown gateway action type (" << static_cast<int>(_action) << ")");
		return "Unknown";
	}
}

ot::Gateway::GatewayActionType ot::Gateway::stringToGatewayActionType(const std::string& _action)
{
	if (_action == toString(GatewayActionType::Unknown)) return GatewayActionType::Unknown;
	else if (_action == toString(GatewayActionType::Ping)) return GatewayActionType::Ping;
	else if (_action == toString(GatewayActionType::Shutdown)) return GatewayActionType::Shutdown;
	else if (_action == toString(GatewayActionType::SendToService)) return GatewayActionType::SendToService;
	else if (_action == toString(GatewayActionType::QueueToService)) return GatewayActionType::QueueToService;
	else
	{
		OT_LOG_ES("Unknown gateway action type \"" << _action << "\"");
		return GatewayActionType::Unknown;
	}
}
