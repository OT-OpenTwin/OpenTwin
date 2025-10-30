// @otlicense
// File: SessionInformation.cpp
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

// LDS header
#include "SessionInformation.h"

SessionInformation::SessionInformation(const ot::ServiceInitData& _serviceInitData)
	: m_id(_serviceInitData.getSessionID()), m_sessionServiceUrl(_serviceInitData.getSessionServiceURL())
{}

bool SessionInformation::operator == (const SessionInformation& _other) const {
	return (m_id == _other.m_id && m_sessionServiceUrl == _other.m_sessionServiceUrl);
}

bool SessionInformation::operator != (const SessionInformation& _other) const {
	return (m_id != _other.m_id || m_sessionServiceUrl != _other.m_sessionServiceUrl);
}

bool SessionInformation::operator < (const SessionInformation& _other) const {
	return m_id < _other.m_id;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void SessionInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ID", ot::JsonString(m_id, _allocator), _allocator);
	_object.AddMember("LSS.Url", ot::JsonString(m_sessionServiceUrl, _allocator), _allocator);
}

