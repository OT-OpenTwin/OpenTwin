// @otlicense
// File: BasicServiceInformation.cpp
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
#include "OTCore/BasicServiceInformation.h"

ot::BasicServiceInformation::BasicServiceInformation() {}

ot::BasicServiceInformation::BasicServiceInformation(const std::string& _serviceNameAndType) : m_name(_serviceNameAndType), m_type(_serviceNameAndType) {}

ot::BasicServiceInformation::BasicServiceInformation(const std::string& _serviceName, const std::string& _serviceType) : m_name(_serviceName), m_type(_serviceType) {}

ot::BasicServiceInformation::BasicServiceInformation(const BasicServiceInformation& _other) : m_name(_other.m_name), m_type(_other.m_type) {}

ot::BasicServiceInformation::~BasicServiceInformation() {}

ot::BasicServiceInformation& ot::BasicServiceInformation::operator = (const BasicServiceInformation& _other) {
	m_name = _other.m_name;
	m_type = _other.m_type;
	return *this;
}

bool ot::BasicServiceInformation::operator == (const BasicServiceInformation& _other) const {
	return m_name == _other.m_name && m_type == _other.m_type;
}

bool ot::BasicServiceInformation::operator != (const BasicServiceInformation& _other) const {
	return m_name != _other.m_name || m_type != _other.m_type;
}

bool ot::BasicServiceInformation::operator < (const BasicServiceInformation& _other) const {
	return m_name < _other.m_name && m_type < _other.m_type;
}

void ot::BasicServiceInformation::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("BSI.ServiceName", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("BSI.ServiceType", JsonString(m_type, _allocator), _allocator);
}

void ot::BasicServiceInformation::setFromJsonObject(const ConstJsonObject& _object) {
	m_name = json::getString(_object, "BSI.ServiceName");
	m_type = json::getString(_object, "BSI.ServiceType");
}
