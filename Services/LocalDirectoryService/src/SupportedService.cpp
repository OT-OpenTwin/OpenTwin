// @otlicense
// File: SupportedService.cpp
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
#include "SupportedService.h"

SupportedService::SupportedService() : m_maxCrashRestarts(0), m_maxStartupRestarts(0) {}

SupportedService::SupportedService(const std::string& _name, const std::string& _type) :
	m_name(_name), m_type(_type), m_maxCrashRestarts(0), m_maxStartupRestarts(0)
{}

void SupportedService::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);
	_object.AddMember("MaxCrashRestarts", m_maxCrashRestarts, _allocator);
	_object.AddMember("MaxStartupRestarts", m_maxStartupRestarts, _allocator);
}