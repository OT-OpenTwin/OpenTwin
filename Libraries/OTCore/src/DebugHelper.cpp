// @otlicense

//! @file DebugHelper.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/DebugHelper.h"
#include "OTCore/LogDispatcher.h"

void ot::DebugHelper::serviceSetupCompleted(const ot::ServiceDebugInformation& _serviceInfo) {
	std::string message = getSetupCompletedMessagePrefix();
	ot::JsonDocument infoDoc;
	_serviceInfo.addToJsonObject(infoDoc, infoDoc.GetAllocator());
	message.append(infoDoc.toJson());
	OT_LOG_D(message);
}
