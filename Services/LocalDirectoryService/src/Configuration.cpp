// Project header
#include "Configuration.h"
#include "ExitCodes.h"

// OpenTwin header
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCommunication/actionTypes.h"
#include "OpenTwinSystem/Application.h"
#include "OpenTwinSystem/OperatingSystem.h"

// C++ header
#include <iostream>
#include <cassert>

#undef GetObject

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

void Configuration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, LDS_CFG_DefaultMaxCrashRestarts, m_DefaultMaxCrashRestarts);
	ot::rJSON::add(_document, _object, LDS_CFG_DefaultMaxStartupRestarts, m_DefaultMaxStartupRestarts);
	ot::rJSON::add(_document, _object, LDS_CFG_LauncherPath, m_LauncherPath);
	ot::rJSON::add(_document, _object, LDS_CFG_ServicesLibraryPath, m_ServicesLibraryPath);

	OT_rJSON_createValueArray(supportedServicesArray);
	for (auto s : m_SupportedServices) {
		OT_rJSON_createValueObject(serviceObj);
		s.addToJsonObject(_document, serviceObj);
		supportedServicesArray.PushBack(serviceObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, LDS_CFG_SupportedServices, supportedServicesArray);
}

void Configuration::setFromJsonObject(OT_rJSON_val& _object) {
	// Load general configuration
	m_DefaultMaxCrashRestarts = ot::rJSON::getUInt(_object, LDS_CFG_DefaultMaxCrashRestarts);
	OT_LOG_D("[Configuration]: Default max restarts after crash set to " + std::to_string(m_DefaultMaxCrashRestarts));

	m_DefaultMaxStartupRestarts = ot::rJSON::getUInt(_object, LDS_CFG_DefaultMaxStartupRestarts);
	OT_LOG_D("[Configuration]: Default max restarts at startup set to " + std::to_string(m_DefaultMaxStartupRestarts));

	// Load launcher name
	m_LauncherPath = ot::rJSON::getString(_object, LDS_CFG_LauncherPath);
	OT_LOG_D("[Configuration]: Launcher path set to " + m_LauncherPath);

	// Load services library path
	m_ServicesLibraryPath = ot::rJSON::getString(_object, LDS_CFG_ServicesLibraryPath);
	OT_LOG_D("[Configuration]: Services library path set to " + m_ServicesLibraryPath);
	
	// Load supported services

	ot::rJSON::memberCheck(_object, LDS_CFG_SupportedServices);
	
	auto supportedServicesArray = _object[LDS_CFG_SupportedServices].GetArray();
	for (rapidjson::SizeType i = 0; i < supportedServicesArray.Size(); i++) {
		ServiceInformation newEntry;
		if (supportedServicesArray[i].IsString()) {
			std::string name = supportedServicesArray[i].GetString();

			if (name.empty()) continue;
			
			// Name only (no further options provided)
			newEntry.setName(name);
			newEntry.setType(name);
			newEntry.setMaxCrashRestarts(m_DefaultMaxCrashRestarts);
			newEntry.setMaxStartupRestarts(m_DefaultMaxStartupRestarts);
		}
		else if (supportedServicesArray[i].IsObject()) {
			OT_rJSON_val serviceObj = supportedServicesArray[i].GetObject();

			// Name
			std::string name = ot::rJSON::getString(serviceObj, LDS_CFG_ServiceName);
			if (name.empty()) continue;
			newEntry.setName(name);

			// Type
			if (serviceObj.HasMember(LDS_CFG_ServiceType)) {
				std::string type = ot::rJSON::getString(serviceObj, LDS_CFG_ServiceType);
				if (type.empty()) {
					otAssert(0, "Service type is empty");
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
				newEntry.setMaxCrashRestarts(ot::rJSON::getUInt(serviceObj, LDS_CFG_ServiceMaxCrashRestarts));
			}
			else {
				newEntry.setMaxCrashRestarts(m_DefaultMaxCrashRestarts);
			}

			if (serviceObj.HasMember(LDS_CFG_ServiceMaxStartupRestarts)) {
				newEntry.setMaxStartupRestarts(ot::rJSON::getUInt(serviceObj, LDS_CFG_ServiceMaxStartupRestarts));
			}
			else {
				newEntry.setMaxStartupRestarts(m_DefaultMaxStartupRestarts);
			}
		}
		else {
			otAssert(0, "Supported service entry is neither a string nor an object");
			OT_LOG_E("Supported service entry is neither a string nor an object");
			return;
		}

		// Check for duplicates
		for (auto check : m_SupportedServices) {
			if (check.getName() == newEntry.getName()) {
				otAssert(0, "Supported service duplicate");
				OT_LOG_E("A service with the name \"" + check.getName() + "\" was already provided");
				return;
			}
		}

		m_SupportedServices.push_back(newEntry);
		OT_LOG_D("[Configuration]: New supported service added (name = \"" +
			newEntry.getName() + "\"; type = \"" + newEntry.getType() + "\"; maxCrashRestarts = " + std::to_string(newEntry.getMaxCrashRestarts()) + "\"; maxStartupRestarts = \"" + std::to_string(newEntry.getMaxStartupRestarts()) + "\")");
	}
}

int Configuration::importFromEnvironment(void) {
	if (m_configurationImported) return LDS_EXIT_Ok;

	// Clean up current data
	m_SupportedServices.clear();

	// Read configuration from env
	const char * configurationEnv = ot::os::getEnvironmentVariable(LDS_CFG_ENV);
	if (configurationEnv == nullptr) return LDS_EXIT_CfgNoEnv;

	OT_rJSON_parseDOC(configurationDoc, configurationEnv);
	if (!configurationDoc.IsObject()) return LDS_EXIT_CfgBroken;

	try {
		setFromJsonObject(configurationDoc);
	}
	catch (...) {
		return LDS_EXIT_CfgBroken;
	}

	m_configurationImported = true;
	return LDS_EXIT_Ok;
}

bool Configuration::supportsService(const std::string& _serviceName) const {
	for (auto s : m_SupportedServices) {
		if (s.getName() == _serviceName) return true;
	}
	return false;
}

Configuration::Configuration() 
	: m_configurationImported(false), m_DefaultMaxCrashRestarts(8), m_DefaultMaxStartupRestarts(64)
{}

Configuration::~Configuration() {

}