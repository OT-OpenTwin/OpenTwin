// @otlicense
// File: ProjectFilterData.cpp
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
#include "OTCore/ProjectFilterData.h"

ot::ProjectFilterData::ProjectFilterData(const ot::ConstJsonObject& _jsonObject) {
	this->setFromJsonObject(_jsonObject);
}

void ot::ProjectFilterData::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("ProjectGroups", JsonArray(m_projectGroups, _allocator), _allocator);
	_jsonObject.AddMember("ProjectTypes", JsonArray(m_projectTypes, _allocator), _allocator);
	_jsonObject.AddMember("ProjectNames", JsonArray(m_projectNames, _allocator), _allocator);
	_jsonObject.AddMember("Tags", JsonArray(m_tags, _allocator), _allocator);
	_jsonObject.AddMember("Owners", JsonArray(m_owners, _allocator), _allocator);
	_jsonObject.AddMember("UserGroups", JsonArray(m_userGroups, _allocator), _allocator);
}

void ot::ProjectFilterData::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_projectGroups = json::getStringList(_jsonObject, "ProjectGroups");
	m_projectTypes = json::getStringList(_jsonObject, "ProjectTypes");
	m_projectNames = json::getStringList(_jsonObject, "ProjectNames");
	m_tags = json::getStringList(_jsonObject, "Tags");
	m_owners = json::getStringList(_jsonObject, "Owners");
	m_userGroups = json::getStringList(_jsonObject, "UserGroups");
}
