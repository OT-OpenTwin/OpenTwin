// @otlicense
// File: ProjectInformation.cpp
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
#include "OTCore/ProjectInformation.h"

ot::ProjectInformation::ProjectInformation() : m_version(0), m_creationTime(0), m_lastAccessTime(0), m_lastModifiedTime(0) {

}

ot::ProjectInformation::ProjectInformation(const ConstJsonObject& _jsonObject) : ProjectInformation() {
	this->setFromJsonObject(_jsonObject);
}

void ot::ProjectInformation::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Version", m_version, _allocator);
	_jsonObject.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_jsonObject.AddMember("Type", JsonString(m_type, _allocator), _allocator);
	_jsonObject.AddMember("UserName", JsonString(m_user, _allocator), _allocator);
	_jsonObject.AddMember("Collection", JsonString(m_collection, _allocator), _allocator);
	_jsonObject.AddMember("CreationTime", m_creationTime, _allocator);
	_jsonObject.AddMember("LastAccessTime", m_lastAccessTime, _allocator);
	_jsonObject.AddMember("LastModifiedTime", m_lastModifiedTime, _allocator);
	_jsonObject.AddMember("Groups", JsonArray(m_userGroups, _allocator), _allocator);
	_jsonObject.AddMember("Tags", JsonArray(m_tags, _allocator), _allocator);
	_jsonObject.AddMember("ProjectGroup", JsonString(m_projectGroup, _allocator), _allocator);
}

void ot::ProjectInformation::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_version = json::getInt(_jsonObject, "Version");
	m_name = json::getString(_jsonObject, "Name");
	m_type = json::getString(_jsonObject, "Type");
	m_user = json::getString(_jsonObject, "UserName");
	m_collection = json::getString(_jsonObject, "Collection");
	m_creationTime = json::getInt64(_jsonObject, "CreationTime");
	m_lastAccessTime = json::getInt64(_jsonObject, "LastAccessTime");
	m_lastModifiedTime = json::getInt64(_jsonObject, "LastModifiedTime");
	m_userGroups = json::getStringList(_jsonObject, "Groups");
	m_tags = json::getStringList(_jsonObject, "Tags");
	m_projectGroup = json::getString(_jsonObject, "ProjectGroup");
}
