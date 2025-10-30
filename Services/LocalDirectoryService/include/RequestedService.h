// @otlicense
// File: RequestedService.h
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

#pragma once

// LDS header
#include "SupportedService.h"
#include "ServiceStartupData.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ServiceInitData.h"

class RequestedService {
	OT_DECL_DEFCOPY(RequestedService)
	OT_DECL_DEFMOVE(RequestedService)
	OT_DECL_NODEFAULT(RequestedService)
public:
	RequestedService(ot::ServiceInitData&& _initData, ServiceStartupData&& _startupData);
	RequestedService(const ot::ServiceInitData& _initData, const SupportedService& _supportedServiceInfo);
	~RequestedService() = default;

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	ot::ServiceInitData& getInitData() { return m_initData; };
	const ot::ServiceInitData& getInitData() const { return m_initData; };

	ServiceStartupData& getStartupData() { return m_startupData; };
	const ServiceStartupData& getStartupData() const { return m_startupData; };

	unsigned int incrStartCounter() { return m_startupData.incrStartCounter(); };
	unsigned int getMaxCrashRestarts() const { return m_startupData.getMaxCrashRestarts(); };

	unsigned int incrIniAttempt() { return m_startupData.incrIniAttempt(); };
	unsigned int getMaxStartupRestarts() const { return m_startupData.getMaxStartupRestarts(); };

private:
	ot::ServiceInitData m_initData;
	ServiceStartupData  m_startupData;

};