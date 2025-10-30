// @otlicense
// File: GSSRegistrationInfo.cpp
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

// LSS header
#include "GSSRegistrationInfo.h"

GSSRegistrationInfo::GSSRegistrationInfo() :
	m_serviceID(0)
{
	this->reset();
}

void GSSRegistrationInfo::reset(void) {
	m_serviceID = ot::invalidServiceID;
	m_databaseURL.clear();
	m_authURL.clear();
	m_gdsURL.clear();
}

void GSSRegistrationInfo::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) {
	_jsonObject.AddMember("LSS.ID", m_serviceID, _allocator);
	_jsonObject.AddMember("DB.URL", ot::JsonString(m_databaseURL, _allocator), _allocator);
	_jsonObject.AddMember("Auth.URL", ot::JsonString(m_authURL, _allocator), _allocator);
	_jsonObject.AddMember("GDS.URL", ot::JsonString(m_gdsURL, _allocator), _allocator);
}
