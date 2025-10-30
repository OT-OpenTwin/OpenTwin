// @otlicense
// File: ServiceDebugInformation.cpp
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
#include "OTSystem/OperatingSystem.h"
#include "OTCore/ServiceDebugInformation.h"

ot::ServiceDebugInformation::ServiceDebugInformation()
	: m_id(ot::invalidServiceID), m_processID(0)
{}

ot::ServiceDebugInformation::ServiceDebugInformation(const ot::ServiceBase& _serviceInfo) : m_processID(0) {
	m_id = _serviceInfo.getServiceID();
	m_name = _serviceInfo.getServiceName();
	m_url = _serviceInfo.getServiceURL();
	this->setCurrentProcessID();
}

void ot::ServiceDebugInformation::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("ID", static_cast<uint32_t>(m_id), _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("URL", JsonString(m_url, _allocator), _allocator);
	_object.AddMember("WebsocketURL", JsonString(m_websocketUrl, _allocator), _allocator);
	_object.AddMember("ProcessID", m_processID, _allocator);
}

void ot::ServiceDebugInformation::setFromJsonObject(const ConstJsonObject& _object) {
	m_id = static_cast<ot::serviceID_t>(json::getUInt(_object, "ID"));
	m_name = json::getString(_object, "Name");
	m_url = json::getString(_object, "URL");
	m_websocketUrl = json::getString(_object, "WebsocketURL");
	m_processID = json::getUInt64(_object, "ProcessID");
}

void ot::ServiceDebugInformation::setCurrentProcessID() {
	m_processID = ot::OperatingSystem::getCurrentProcessID();
}

std::string ot::ServiceDebugInformation::getAdditionalInformationJson() const {
	bool success = false;
	ot::JsonDocument doc;
	
	if (!m_websocketUrl.empty()) {
		doc.AddMember("WebsocketURL", JsonString(m_websocketUrl, doc.GetAllocator()), doc.GetAllocator());
		success = true;
	}
	



	if (success) {
		return doc.toJson();
	}
	else {
		return std::string();
	}
}
