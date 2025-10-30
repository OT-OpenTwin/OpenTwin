// @otlicense
// File: LocalDirectoryService.h
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

// GDS header
#include "LoadInformation.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/GDSDebugInfo.h"
#include "OTCommunication/ServiceInitData.h"

// std header
#include <string>
#include <list>

//! @brief The LocalDirectoryService holds information about the LDS.
//! It is used to start services, gather information about services started by the LDS, as well as to provide information about the system load of the LDS.
class LocalDirectoryService : public ot::ServiceBase {
	OT_DECL_NOCOPY(LocalDirectoryService)
	OT_DECL_NODEFAULT(LocalDirectoryService)
public:
	LocalDirectoryService(const std::string& _url);
	LocalDirectoryService(LocalDirectoryService&& _other) noexcept;
	virtual ~LocalDirectoryService();

	LocalDirectoryService& operator=(LocalDirectoryService&& _other) noexcept;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Load information

	const LoadInformation& loadInformation(void) const { return m_loadInformation; }
	LoadInformation::load_t load(void) const;

	bool updateSystemUsageValues(ot::JsonDocument& _jsonDocument);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service management

	void setSupportedServices(const std::list<std::string>& _serviesNames) { m_supportedServices = _serviesNames; }
	bool supportsService(const std::string& _serviceName) const;

	bool requestToRunService(const ot::ServiceInitData& _serviceInfo);
	bool requestToRunRelayService(const ot::ServiceInitData& _serviceInfo, std::string& _websocketURL, std::string& _relayServiceURL);

	void sessionClosing(const std::string& _sessionID);
	void sessionClosed(const std::string& _sessionID);
	void serviceClosed(const std::string& _sessionID, ot::serviceID_t _serviceID);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void getDebugInformation(ot::GDSDebugInfo::LDSInfo& _info) const;

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const;

private:
	std::list<std::string>         m_supportedServices; //! @brief List of supported services by the LDS.
	LoadInformation                m_loadInformation; //! @brief Latest system load information of the LDS.
	std::list<ot::ServiceInitData> m_services; //! @brief List of services started by the LDS.
};