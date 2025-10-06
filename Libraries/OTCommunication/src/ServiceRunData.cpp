//! @file ServiceRunData.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/ServiceRunData.h"

ot::ServiceRunData::ServiceRunData() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::ServiceRunData::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	JsonArray serviceArr;
	for (const ServiceBase& service : m_services) {
		JsonObject serviceObj;
		service.addToJsonObject(serviceObj, _allocator);
		serviceArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("Services", serviceArr, _allocator);
}

void ot::ServiceRunData::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_services.clear();
	for (const ConstJsonObject& serviceObj : json::getObjectList(_jsonObject, "Services")) {
		ServiceBase service;
		service.setFromJsonObject(serviceObj);
		m_services.push_back(std::move(service));
	}
}
