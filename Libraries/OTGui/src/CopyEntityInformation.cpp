// @otlicense
// File: CopyEntityInformation.cpp
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
#include "OTGui/CopyEntityInformation.h"

ot::CopyEntityInformation::CopyEntityInformation()
	: m_uid(0) {

}

ot::CopyEntityInformation::CopyEntityInformation(const ConstJsonObject& _jsonObject) {
	this->setFromJsonObject(_jsonObject);
}

ot::CopyEntityInformation::CopyEntityInformation(UID _uid, const std::string& _name, const std::string& _rawData)
	: m_uid(_uid), m_name(_name), m_rawData(_rawData) {

}

void ot::CopyEntityInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("UID", m_uid, _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Data", JsonString(m_rawData, _allocator), _allocator);
}

void ot::CopyEntityInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_uid = json::getUInt64(_object, "UID");
	m_name = json::getString(_object, "Name");
	m_rawData = json::getString(_object, "Data");
}