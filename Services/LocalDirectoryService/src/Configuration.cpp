// @otlicense
// File: Configuration.cpp
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
#include "Configuration.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <cassert>
#include <iostream>

#define LDS_CFG_ENV "OT_LOCALDIRECTORYSERVICE_CONFIGURATION"
#define LDS_CFG_DefaultMaxCrashRestarts "DefaultMaxCrashRestarts"
#define LDS_CFG_DefaultMaxStartupRestarts "DefaultMaxStartupRestarts"
#define LDS_CFG_SupportedServices "SupportedServices"
#define LDS_CFG_ServiceName "Name"
#define LDS_CFG_ServiceType "Type"
#define LDS_CFG_ServiceMaxCrashRestarts "MaxCrashRestarts"
#define LDS_CFG_ServiceMaxStartupRestarts "MaxStartupRestarts"
#define LDS_CFG_LauncherPath "LauncherPath"
#define LDS_CFG_ServicesLibraryPath "ServicesLibraryPath"
#define LDS_CFG_ServiceStartWorkerCount "ServiceStartWorkerCount"
#define LDS_CFG_InitializeWorkerCount "InitializeWorkerCount"

Configuration& Configuration::instance() {
	static Configuration g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void Configuration::getDebugInformation(ot::LDSDebugInfo& _info) {
	ot::LDSDebugInfo::ConfigInfo configInfo;
	configInfo.configImported = m_configurationImported;
	configInfo.defaultMaxCrashRestarts = m_defaultMaxCrashRestarts;
	configInfo.defaultMaxStartupRestarts = m_defaultMaxStartupRestarts;
	configInfo.serviceStartWorkerCount = m_serviceStartWorkerCount;
	configInfo.iniWorkerCount = m_iniWorkerCount;
	configInfo.launcherPath = m_launcherPath;
	configInfo.servicesLibraryPath = m_servicesLibraryPath;

	for (const SupportedService& s : m_supportedServices) {
		ot::LDSDebugInfo::SupportedServiceInfo serviceInfo;
		serviceInfo.name = s.getName();
		serviceInfo.type = s.getType();
		serviceInfo.maxCrashRestarts = s.getMaxCrashRestarts();
		serviceInfo.maxStartupRestarts = s.getMaxStartupRestarts();
		configInfo.supportedServices.push_back(std::move(serviceInfo));
	}

	_info.setConfig(std::move(configInfo));
}

void Configuration::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(LDS_CFG_DefaultMaxCrashRestarts, m_defaultMaxCrashRestarts, _allocator);
	_object.AddMember(LDS_CFG_DefaultMaxStartupRestarts, m_defaultMaxStartupRestarts, _allocator);
	_object.AddMember(LDS_CFG_ServiceStartWorkerCount, m_serviceStartWorkerCount, _allocator);
	_object.AddMember(LDS_CFG_InitializeWorkerCount, m_iniWorkerCount, _allocator);
	_object.AddMember(LDS_CFG_LauncherPath, ot::JsonString(m_launcherPath, _allocator), _allocator);
	_object.AddMember(LDS_CFG_ServicesLibraryPath, ot::JsonString(m_servicesLibraryPath, _allocator), _allocator);

	ot::JsonArray serviceArr;
	for (auto& s : m_supportedServices) {
		ot::JsonObject serviceObj;
		s.addToJsonObject(serviceObj, _allocator);
		serviceArr.PushBack(serviceObj, _allocator);
	}
	_object.AddMember(LDS_CFG_SupportedServices, serviceArr, _allocator);
}

void Configuration::setFromJsonObject(const ot::ConstJsonObject& _object) {
	// Load general configuration
	m_defaultMaxCrashRestarts = ot::json::getUInt(_object, LDS_CFG_DefaultMaxCrashRestarts, 3);
	OT_LOG_D("[Configuration]: Default max restarts after crash set to " + std::to_string(m_defaultMaxCrashRestarts));

	m_defaultMaxStartupRestarts = ot::json::getUInt(_object, LDS_CFG_DefaultMaxStartupRestarts, 64);
	OT_LOG_D("[Configuration]: Default max restarts at startup set to " + std::to_string(m_defaultMaxStartupRestarts));

	// Load launcher name
	m_launcherPath = ot::json::getString(_object, LDS_CFG_LauncherPath);
	OT_LOG_D("[Configuration]: Launcher path set to " + m_launcherPath);

	// Load services library path
	m_servicesLibraryPath = ot::json::getString(_object, LDS_CFG_ServicesLibraryPath);
	OT_LOG_D("[Configuration]: Services library path set to " + m_servicesLibraryPath);
	
	m_serviceStartWorkerCount = ot::json::getUInt(_object, LDS_CFG_ServiceStartWorkerCount, 1);
	OT_LOG_D("[Configuration]: Service start worker count set to " + std::to_string(m_serviceStartWorkerCount));

	m_iniWorkerCount = ot::json::getUInt(_object, LDS_CFG_InitializeWorkerCount, 4);
	OT_LOG_D("[Configuration]: Initialize worker count set to " + std::to_string(m_iniWorkerCount));

	// Load supported services

	ot::ConstJsonArray serviceArr = ot::json::getArray(_object, LDS_CFG_SupportedServices);
	for (rapidjson::SizeType i = 0; i < serviceArr.Size(); i++) {

		SupportedService newEntry;
		newEntry.setMaxCrashRestarts(m_defaultMaxCrashRestarts);
		newEntry.setMaxStartupRestarts(m_defaultMaxStartupRestarts);

		if (serviceArr[i].IsString()) {
			std::string name = ot::json::getString(serviceArr, i);

			if (name.empty()) {
				continue;
			}

			// Name only (no further options provided)
			newEntry.setName(name);
			newEntry.setType(name);
		}
		else if (serviceArr[i].IsObject()) {
			ot::ConstJsonObject serviceObj = ot::json::getObject(serviceArr, i);

			// Name
			std::string name = ot::json::getString(serviceObj, LDS_CFG_ServiceName);
			if (name.empty()) {
				OT_LOG_W("Empty service name provided as supported servive");
				continue;
			}
			newEntry.setName(name);
			newEntry.setType(name);

			// Type
			if (serviceObj.HasMember(LDS_CFG_ServiceType)) {
				std::string type = ot::json::getString(serviceObj, LDS_CFG_ServiceType);
				if (type.empty()) {
					OTAssert(0, "Service type is empty");
					OT_LOG_E("Service type is empty");
					return;
				}
				newEntry.setType(type);
			}

			// Is Restartable
			if (serviceObj.HasMember(LDS_CFG_ServiceMaxCrashRestarts)) {
				newEntry.setMaxCrashRestarts(ot::json::getUInt(serviceObj, LDS_CFG_ServiceMaxCrashRestarts));
			}

			if (serviceObj.HasMember(LDS_CFG_ServiceMaxStartupRestarts)) {
				newEntry.setMaxStartupRestarts(ot::json::getUInt(serviceObj, LDS_CFG_ServiceMaxStartupRestarts));
			}
		}
		else {
			OTAssert(0, "Supported service entry is neither a string nor an object");
			OT_LOG_E("Supported service entry is neither a string nor an object");
			return;
		}

		// Check for duplicates
		for (auto check : m_supportedServices) {
			if (check.getName() == newEntry.getName()) {
				OTAssert(0, "Supported service duplicate");
				OT_LOG_E("A service with the name \"" + check.getName() + "\" was already provided");
				return;
			}
		}

		m_supportedServices.push_back(newEntry);
		OT_LOG_D("[Configuration]: New supported service added (name = \"" +
			newEntry.getName() + "\"; type = \"" + newEntry.getType() + "\"; maxCrashRestarts = " + std::to_string(newEntry.getMaxCrashRestarts()) + "\"; maxStartupRestarts = \"" + std::to_string(newEntry.getMaxStartupRestarts()) + "\")");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

void Configuration::importFromEnvironment() {
	if (m_configurationImported) {
		return;
	}

	// Clean up current data
	m_supportedServices.clear();

	// Read configuration from env
	const char * configurationEnv = ot::OperatingSystem::getEnvironmentVariable(LDS_CFG_ENV);
	if (configurationEnv == nullptr) {
		exit(ot::AppExitCode::EnvironmentError);
	}

	ot::JsonDocument configurationDoc;
	configurationDoc.fromJson(configurationEnv);

	if (!configurationDoc.IsObject()) {
		exit(ot::AppExitCode::ConfigurationBroken);
	}

	this->setFromJsonObject(configurationDoc.getConstObject());

	m_configurationImported = true;
}

std::optional<SupportedService> Configuration::getSupportedService(const std::string& _serviceName) const {
	for (const SupportedService& s : m_supportedServices) {
		if (s.getName() == _serviceName) {
			return s;
		}
	}
	return std::optional<SupportedService>();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

bool Configuration::ensureValid() const {
	if (!m_configurationImported) {
		OT_LOG_E("Configuration not imported");
		return false;
	}
	if (m_supportedServices.empty()) {
		OT_LOG_E("No supported services provided");
		return false;
	}
	if (m_launcherPath.empty()) {
		OT_LOG_E("Launcher path is empty");
		return false;
	}
	if (m_servicesLibraryPath.empty()) {
		OT_LOG_E("Services library path is empty");
		return false;
	}
	if (m_serviceStartWorkerCount == 0) {
		OT_LOG_E("Service start worker count is zero");
		return false;
	}
	if (m_iniWorkerCount == 0) {
		OT_LOG_E("Initialize worker count is zero");
		return false;
	}

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Configuration::Configuration() 
	: m_configurationImported(false), m_defaultMaxCrashRestarts(8), m_defaultMaxStartupRestarts(64), m_iniWorkerCount(4)
{}

Configuration::~Configuration() {

}