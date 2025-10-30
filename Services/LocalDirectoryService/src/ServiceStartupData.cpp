// @otlicense
// File: ServiceStartupData.cpp
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
#include "ServiceStartupData.h"

ServiceStartupData::ServiceStartupData(const SupportedService & _supportedServiceInfo) 
	: m_maxCrashRestarts(_supportedServiceInfo.getMaxCrashRestarts()), m_maxStartupRestarts(_supportedServiceInfo.getMaxStartupRestarts()),
	m_initializeAttempt(0), m_startCounter(0)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ServiceStartupData::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("InitializeAttempt", m_initializeAttempt, _allocator);
	_object.AddMember("StartCounter", m_startCounter, _allocator);

	_object.AddMember("MaxCrashRestarts", m_maxCrashRestarts, _allocator);
	_object.AddMember("MaxStartupRestarts", m_maxStartupRestarts, _allocator);
}
