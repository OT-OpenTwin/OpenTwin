// @otlicense
// File: ProjectTemplateInformation.cpp
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
#include "OTGui/ProjectTemplateInformation.h"

ot::ProjectTemplateInformation::ProjectTemplateInformation()
	: m_isDefault(false) {

}

ot::ProjectTemplateInformation::ProjectTemplateInformation(const ProjectTemplateInformation& _other) {
	*this = _other;
}

ot::ProjectTemplateInformation::~ProjectTemplateInformation() {

}

ot::ProjectTemplateInformation& ot::ProjectTemplateInformation::operator = (const ProjectTemplateInformation& _other) {
	if (this == &_other) return *this;

	m_name = _other.m_name;
	m_projectType = _other.m_projectType;
	m_briefDescription = _other.m_briefDescription;
	m_description = _other.m_description;
	m_isDefault = _other.m_isDefault;

	return *this;
}

void ot::ProjectTemplateInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Type", JsonString(m_projectType, _allocator), _allocator);
	_object.AddMember("Brief", JsonString(m_briefDescription, _allocator), _allocator);
	JsonObject descriptionObj;
	m_description.addToJsonObject(descriptionObj, _allocator);
	_object.AddMember("Description", descriptionObj, _allocator);
	_object.AddMember("IsDefault", m_isDefault, _allocator);
}

void ot::ProjectTemplateInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_projectType = json::getString(_object, "Type");
	m_briefDescription = json::getString(_object, "Brief");
	m_description.setFromJsonObject(json::getObject(_object, "Description"));
	m_isDefault = json::getBool(_object, "IsDefault");
}
