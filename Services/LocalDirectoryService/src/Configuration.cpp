//! @file Configuration.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Configuration.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logger.h"
#include "OTCore/otAssert.h"
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

Configuration& Configuration::instance(void) {
	static Configuration g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void Configuration::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(LDS_CFG_DefaultMaxCrashRestarts, m_defaultMaxCrashRestarts, _allocator);
	_object.AddMember(LDS_CFG_DefaultMaxStartupRestarts, m_defaultMaxStartupRestarts, _allocator);
	_object.AddMember(LDS_CFG_LauncherPath, ot::JsonString(m_launcherPath, _allocator), _allocator);
	_object.AddMember(LDS_CFG_ServicesLibraryPath, ot::JsonString(m_servicesLibraryPath, _allocator), _allocator);

	ot::JsonArray serviceArr;
	for (auto s : m_supportedServices) {
		ot::JsonObject serviceObj;
		s.addToJsonObject(serviceObj, _allocator);
		serviceArr.PushBack(serviceObj, _allocator);
	}
	_object.AddMember(LDS_CFG_SupportedServices, serviceArr, _allocator);
}

void Configuration::setFromJsonObject(const ot::ConstJsonObject& _object) {
	// Load general configuration
	m_defaultMaxCrashRestarts = ot::json::getUInt(_object, LDS_CFG_DefaultMaxCrashRestarts);
	OT_LOG_D("[Configuration]: Default max restarts after crash set to " + std::to_string(m_defaultMaxCrashRestarts));

	m_defaultMaxStartupRestarts = ot::json::getUInt(_object, LDS_CFG_DefaultMaxStartupRestarts);
	OT_LOG_D("[Configuration]: Default max restarts at startup set to " + std::to_string(m_defaultMaxStartupRestarts));

	// Load launcher name
	m_launcherPath = ot::json::getString(_object, LDS_CFG_LauncherPath);
	OT_LOG_D("[Configuration]: Launcher path set to " + m_launcherPath);

	// Load services library path
	m_servicesLibraryPath = ot::json::getString(_object, LDS_CFG_ServicesLibraryPath);
	OT_LOG_D("[Configuration]: Services library path set to " + m_servicesLibraryPath);
	
	// Load supported services

	ot::ConstJsonArray serviceArr = ot::json::getArray(_object, LDS_CFG_SupportedServices);
		for (rapidjson::SizeType i = 0; i < serviceArr.Size(); i++) {
		ServiceInformation newEntry;
		if (serviceArr[i].IsString()) {
			std::string name = ot::json::getString(serviceArr, i);

			if (name.empty()) continue;
			
			// Name only (no further options provided)
			newEntry.setName(name);
			newEntry.setType(name);
			newEntry.setMaxCrashRestarts(m_defaultMaxCrashRestarts);
			newEntry.setMaxStartupRestarts(m_defaultMaxStartupRestarts);
		}
		else if (serviceArr[i].IsObject()) {
			ot::ConstJsonObject serviceObj = ot::json::getObject(serviceArr, i);

			// Name
			std::string name = ot::json::getString(serviceObj, LDS_CFG_ServiceName);
			if (name.empty()) continue;
			newEntry.setName(name);

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
			else {
				newEntry.setType(newEntry.getName());
			}

			// Is Restartable
			if (serviceObj.HasMember(LDS_CFG_ServiceMaxCrashRestarts)) {
				newEntry.setMaxCrashRestarts(ot::json::getUInt(serviceObj, LDS_CFG_ServiceMaxCrashRestarts));
			}
			else {
				newEntry.setMaxCrashRestarts(m_defaultMaxCrashRestarts);
			}

			if (serviceObj.HasMember(LDS_CFG_ServiceMaxStartupRestarts)) {
				newEntry.setMaxStartupRestarts(ot::json::getUInt(serviceObj, LDS_CFG_ServiceMaxStartupRestarts));
			}
			else {
				newEntry.setMaxStartupRestarts(m_defaultMaxStartupRestarts);
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

void Configuration::importFromEnvironment(void) {
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

	this->setFromJsonObject(configurationDoc.GetConstObject());

	m_configurationImported = true;
}

bool Configuration::supportsService(const std::string& _serviceName) const {
	for (auto s : m_supportedServices) {
		if (s.getName() == _serviceName) {
			return true;
		}
	}
	return false;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Configuration::Configuration() 
	: m_configurationImported(false), m_defaultMaxCrashRestarts(8), m_defaultMaxStartupRestarts(64)
{}

Configuration::~Configuration() {

}