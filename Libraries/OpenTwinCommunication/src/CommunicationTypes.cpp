#include "OpenTwinCommunication/CommunicationTypes.h"
#include "OpenTwinCore/otAssert.h"

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