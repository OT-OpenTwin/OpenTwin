// @otlicense

//! @file LogTypes.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogTypes.h"
#include "OTCore/LogDispatcher.h"

void ot::addLogFlagsToJsonArray(const LogFlags& _flags, JsonArray& _flagsArray, JsonAllocator& _allocator) {
	if (_flags & INFORMATION_LOG) _flagsArray.PushBack(JsonString("Information", _allocator), _allocator);
	if (_flags & DETAILED_LOG) _flagsArray.PushBack(JsonString("Detailed", _allocator), _allocator);
	if (_flags & WARNING_LOG) _flagsArray.PushBack(JsonString("Warning", _allocator), _allocator);
	if (_flags & ERROR_LOG) _flagsArray.PushBack(JsonString("Error", _allocator), _allocator);
	if (_flags & INBOUND_MESSAGE_LOG) _flagsArray.PushBack(JsonString("InboundMessage", _allocator), _allocator);
	if (_flags & QUEUED_INBOUND_MESSAGE_LOG) _flagsArray.PushBack(JsonString("QueuedMessage", _allocator), _allocator);
	if (_flags & ONEWAY_TLS_INBOUND_MESSAGE_LOG) _flagsArray.PushBack(JsonString("OneWayTLSMessage", _allocator), _allocator);
	if (_flags & OUTGOING_MESSAGE_LOG) _flagsArray.PushBack(JsonString("OutgoingMessage", _allocator), _allocator);
	if (_flags & TEST_LOG) _flagsArray.PushBack(JsonString("Test", _allocator), _allocator);
}

ot::LogFlags ot::logFlagsFromJsonArray(const ConstJsonArray& _flagsArray) {
	LogFlags result(NO_LOG);

	for (JsonSizeType i = 0; i < _flagsArray.Size(); i++) {
		std::string f = json::getString(_flagsArray, i);

		if (f == "Information") result |= INFORMATION_LOG;
		else if (f == "Detailed") result |= DETAILED_LOG;
		else if (f == "Warning") result |= WARNING_LOG;
		else if (f == "Error") result |= ERROR_LOG;
		else if (f == "InboundMessage") result |= INBOUND_MESSAGE_LOG;
		else if (f == "QueuedMessage") result |= QUEUED_INBOUND_MESSAGE_LOG;
		else if (f == "OneWayTLSMessage") result |= ONEWAY_TLS_INBOUND_MESSAGE_LOG;
		else if (f == "OutgoingMessage") result |= OUTGOING_MESSAGE_LOG;
		else if (f == "Test") result |= TEST_LOG;
		else {
			OT_LOG_EAS("Unknown log flag \"" + f + "\"");
		}
	}

	return result;
}
