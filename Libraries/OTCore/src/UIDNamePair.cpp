// @otlicense
// File: UIDNamePair.cpp
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
#include "OTCore/UIDNamePair.h"

ot::UIDNamePair::UIDNamePair()
	: m_uid(0)
{

}

ot::UIDNamePair::UIDNamePair(UID _uid, const std::string& _name) 
	: m_uid(_uid), m_name(_name)
{

}

ot::UIDNamePair::UIDNamePair(const ConstJsonObject& _jsonObject) 
	: m_uid(0)
{
	this->setFromJsonObject(_jsonObject);
}

ot::UIDNamePair::UIDNamePair(const UIDNamePair& _other) 
	: m_uid(_other.m_uid), m_name(_other.m_name)
{

}

ot::UIDNamePair::~UIDNamePair() {}

ot::UIDNamePair& ot::UIDNamePair::operator=(const UIDNamePair& _other) {
	if (this != &_other) {
		m_uid = _other.m_uid;
		m_name = _other.m_name;
	}
	return *this;
}

bool ot::UIDNamePair::operator==(const UIDNamePair& _other) const {
	return m_uid == _other.m_uid && m_name == _other.m_name;
}

bool ot::UIDNamePair::operator!=(const UIDNamePair& _other) const {
	return m_uid != _other.m_uid || m_name != _other.m_name;
}

void ot::UIDNamePair::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("UID", m_uid, _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
}

void ot::UIDNamePair::setFromJsonObject(const ConstJsonObject& _object) {
	m_uid = json::getUInt64(_object, "UID");
	m_name = json::getString(_object, "Name");
}
