// @otlicense

//! @file ThisService.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/ThisService.h"

ot::ThisService& ot::ThisService::instance(void) {
	static ThisService g_instance;
	return g_instance;
}

void ot::ThisService::addIdToJsonDocument(JsonDocument& _document) {
	_document.AddMember("Service.ID", ThisService::instance().getServiceID(), _document.GetAllocator());
}

ot::serviceID_t ot::ThisService::getIdFromJsonDocument(const JsonDocument& _document) {
	serviceID_t id = invalidServiceID;
	id = (serviceID_t)json::getUInt(_document, "Service.ID");
	return id;
}