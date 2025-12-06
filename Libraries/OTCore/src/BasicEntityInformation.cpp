// @otlicense
// File: BasicEntityInformation.cpp
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
#include "OTCore/BasicEntityInformation.h"

ot::BasicEntityInformation::BasicEntityInformation()
	: m_entityId(ot::invalidUID), m_entityVersion(ot::invalidUID)
{}

ot::BasicEntityInformation::BasicEntityInformation(UID _entityId, UID _entityVersion)
	: m_entityId(_entityId), m_entityVersion(_entityVersion)
{}

ot::BasicEntityInformation::BasicEntityInformation(const std::string& _entityName, UID _entityId, UID _entityVersion)
	: m_entityName(_entityName), m_entityId(_entityId), m_entityVersion(_entityVersion)
{}

bool ot::BasicEntityInformation::operator==(const BasicEntityInformation& _other) const {
	return (m_entityId == _other.m_entityId) &&
		(m_entityName == _other.m_entityName) &&
		(m_entityVersion == _other.m_entityVersion);
}

bool ot::BasicEntityInformation::operator!=(const BasicEntityInformation& _other) const {
	return !BasicEntityInformation::operator==(_other);
}

void ot::BasicEntityInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("EntityName", JsonString(m_entityName, _allocator), _allocator);
	_object.AddMember("EntityID", m_entityId, _allocator);
	_object.AddMember("EntityVersion", m_entityVersion, _allocator);
}

void ot::BasicEntityInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_entityName = json::getString(_object, "EntityName");
	m_entityId = json::getUInt64(_object, "EntityID");
	m_entityVersion = json::getUInt64(_object, "EntityVersion");
}
