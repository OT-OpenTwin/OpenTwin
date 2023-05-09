#include "OpenTwinCommunication/CommunicationTypes.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

ot::LogFlag ot::messageTypeToLogFlag(ot::MessageType _type) {
	switch (_type)
	{
	case ot::QUEUE: return ot::QUEUED_INBOUND_MESSAGE_LOG;
	case ot::EXECUTE: return ot::INBOUND_MESSAGE_LOG;
	case ot::EXECUTE_ONE_WAY_TLS: return ONEWAY_TLS_INBOUND_MESSAGE_LOG;
	default:
		otAssert(0, "Unknown message type");
		return ot::INBOUND_MESSAGE_LOG;
	}
}

std::string ot::toString(ot::MessageType _type) {
	switch (_type)
	{
	case ot::QUEUE: return "Queue";
	case ot::EXECUTE: return "Execute";
	case ot::EXECUTE_ONE_WAY_TLS: return "ExecuteOneWayTls";
	case ot::SECURE_MESSAGE_TYPES:
	case ot::ALL_MESSAGE_TYPES:
		otAssert(0, "Not supported message type");
		OT_LOG_E("Not supported message type");
		break;
	default:
		otAssert(0, "Unknown message type");
		OT_LOG_E("Unknown message type");
		break;
	}
	return "Execute";
}

ot::MessageType ot::stringToMessageTypeFlag(const std::string& _type) {
	if (_type == toString(ot::QUEUE)) return ot::QUEUE;
	else if (_type == toString(ot::EXECUTE)) return ot::EXECUTE;
	else if (_type == toString(ot::EXECUTE_ONE_WAY_TLS)) return ot::EXECUTE_ONE_WAY_TLS;
	else {
		otAssert(0, "Unknown message type");
		OT_LOG_E("Unknown message type");
		return ot::EXECUTE;
	}
}