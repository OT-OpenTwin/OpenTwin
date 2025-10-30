// @otlicense
// File: ServiceRunData.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
