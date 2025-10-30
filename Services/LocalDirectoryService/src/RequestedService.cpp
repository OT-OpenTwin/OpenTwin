// @otlicense
// File: RequestedService.cpp
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
#include "RequestedService.h"

RequestedService::RequestedService(ot::ServiceInitData&& _initData, ServiceStartupData&& _startupData)
	: m_initData(std::move(_initData)), m_startupData(std::move(_startupData)) 
{}

RequestedService::RequestedService(const ot::ServiceInitData& _initData, const SupportedService& _supportedServiceInfo)
	: m_initData(_initData), m_startupData(_supportedServiceInfo)
{}

void RequestedService::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::JsonObject initObj;
	m_initData.addToJsonObject(initObj, _allocator);
	_object.AddMember("InitData", initObj, _allocator);

	ot::JsonObject startupObj;
	m_startupData.addToJsonObject(startupObj, _allocator);
	_object.AddMember("StartupData", startupObj, _allocator);
}