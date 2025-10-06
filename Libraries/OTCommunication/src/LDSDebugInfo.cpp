//! @file LDSDebugInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/LDSDebugInfo.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::LDSDebugInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("URL", JsonString(m_url, _allocator), _allocator);
	_jsonObject.AddMember("ID", m_id, _allocator);

	_jsonObject.AddMember("GDSURL", JsonString(m_gdsUrl, _allocator), _allocator);
	_jsonObject.AddMember("GDSConnected", m_gdsConnected, _allocator);

	_jsonObject.AddMember("ServicesIPAddress", JsonString(m_servicesIpAddress, _allocator), _allocator);
	_jsonObject.AddMember("LastError", JsonString(m_lastError, _allocator), _allocator);

	JsonObject configObj;
	addConfigToJson(m_config, configObj, _allocator);
	_jsonObject.AddMember("Config", configObj, _allocator);

	_jsonObject.AddMember("UsedPorts", JsonArray(m_usedPorts, _allocator), _allocator);
	_jsonObject.AddMember("WorkerRunning", m_workerRunning, _allocator);
	_jsonObject.AddMember("ServiceCheckAliveFrequency", m_serviceCheckAliveFrequency, _allocator);

	JsonArray aliveSessionsArr;
	for (const auto& sessionPair : m_aliveSessions) {
		JsonObject sessionObj;
		JsonObject sessionInfoObj;
		sessionInfoObj.AddMember("SessionID", JsonString(sessionPair.first.sessionID, _allocator), _allocator);
		sessionInfoObj.AddMember("LSSURL", JsonString(sessionPair.first.lssUrl, _allocator), _allocator);
		sessionObj.AddMember("SessionInfo", sessionInfoObj, _allocator);

		JsonArray servicesArr;
		for (const ServiceInfo& service : sessionPair.second) {
			JsonObject serviceObj;
			addServiceToJson(service, serviceObj, _allocator);
			servicesArr.PushBack(serviceObj, _allocator);
		}
		sessionObj.AddMember("Services", servicesArr, _allocator);
		aliveSessionsArr.PushBack(sessionObj, _allocator);
	}
	_jsonObject.AddMember("AliveSessions", aliveSessionsArr, _allocator);

	JsonArray requestedServicesArr;
	for (const ServiceInfo& service : m_requestedServices) {
		JsonObject serviceObj;
		addServiceToJson(service, serviceObj, _allocator);
		requestedServicesArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("RequestedServices", requestedServicesArr, _allocator);

	JsonArray iniArr;
	for (const ServiceInfo& service : m_initializingServices) {
		JsonObject serviceObj;
		addServiceToJson(service, serviceObj, _allocator);
		iniArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("InitializingServices", iniArr, _allocator);

	JsonArray failedArr;
	for (const ServiceInfo& service : m_failedServices) {
		JsonObject serviceObj;
		addServiceToJson(service, serviceObj, _allocator);
		failedArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("FailedServices", failedArr, _allocator);

	JsonArray newStoppingArr;
	for (const ServiceInfo& service : m_newStoppingServices) {
		JsonObject serviceObj;
		addServiceToJson(service, serviceObj, _allocator);
		newStoppingArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("NewStoppingServices", newStoppingArr, _allocator);

	JsonArray stoppingArr;
	for (const ServiceInfo& service : m_stoppingServices) {
		JsonObject serviceObj;
		addServiceToJson(service, serviceObj, _allocator);
		stoppingArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("StoppingServices", stoppingArr, _allocator);
}

void ot::LDSDebugInfo::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_url = json::getString(_jsonObject, "URL");
	m_id = json::getUInt(_jsonObject, "ID");

	m_gdsUrl = json::getString(_jsonObject, "GDSURL");
	m_gdsConnected = json::getBool(_jsonObject, "GDSConnected");

	m_servicesIpAddress = json::getString(_jsonObject, "ServicesIPAddress");
	m_lastError = json::getString(_jsonObject, "LastError");

	m_config = setConfigFromJson(json::getObject(_jsonObject, "Config"));

	m_usedPorts.clear();
	for (uint32_t port : json::getUIntList(_jsonObject, "UsedPorts")) {
		m_usedPorts.push_back(static_cast<ot::port_t>(port));
	}

	m_workerRunning = json::getBool(_jsonObject, "WorkerRunning");
	m_serviceCheckAliveFrequency = json::getUInt(_jsonObject, "ServiceCheckAliveFrequency");

	m_aliveSessions.clear();
	for (const ConstJsonObject& sessionObj : json::getObjectList(_jsonObject, "AliveSessions")) {
		SessionInfo sessionInfo;
		const ConstJsonObject& sessionInfoObj = json::getObject(sessionObj, "SessionInfo");
		sessionInfo.sessionID = json::getString(sessionInfoObj, "SessionID");
		sessionInfo.lssUrl = json::getString(sessionInfoObj, "LSSURL");
		std::list<ServiceInfo> services;
		for (const ConstJsonObject& serviceObj : json::getObjectList(sessionObj, "Services")) {
			services.push_back(setServiceFromJson(serviceObj));
		}
		m_aliveSessions.push_back(std::make_pair(std::move(sessionInfo), std::move(services)));
	}

	m_requestedServices.clear();
	for (const ConstJsonObject& serviceObj : json::getObjectList(_jsonObject, "RequestedServices")) {
		m_requestedServices.push_back(setServiceFromJson(serviceObj));
	}

	m_initializingServices.clear();
	for (const ConstJsonObject& serviceObj : json::getObjectList(_jsonObject, "InitializingServices")) {
		m_initializingServices.push_back(setServiceFromJson(serviceObj));
	}

	m_failedServices.clear();
	for (const ConstJsonObject& serviceObj : json::getObjectList(_jsonObject, "FailedServices")) {
		m_failedServices.push_back(setServiceFromJson(serviceObj));
	}

	m_newStoppingServices.clear();
	for (const ConstJsonObject& serviceObj : json::getObjectList(_jsonObject, "NewStoppingServices")) {
		m_newStoppingServices.push_back(setServiceFromJson(serviceObj));
	}

	m_stoppingServices.clear();
	for (const ConstJsonObject& serviceObj : json::getObjectList(_jsonObject, "StoppingServices")) {
		m_stoppingServices.push_back(setServiceFromJson(serviceObj));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private methods

void ot::LDSDebugInfo::addConfigToJson(const ConfigInfo& _config, ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("ConfigImported", _config.configImported, _allocator);
	_jsonObject.AddMember("LauncherPath", JsonString(_config.launcherPath, _allocator), _allocator);
	_jsonObject.AddMember("ServicesLibraryPath", JsonString(_config.servicesLibraryPath, _allocator), _allocator);
	_jsonObject.AddMember("DefaultMaxCrashRestarts", _config.defaultMaxCrashRestarts, _allocator);
	_jsonObject.AddMember("DefaultMaxStartupRestarts", _config.defaultMaxStartupRestarts, _allocator);

	JsonArray supportedArr;
	for (const SupportedServiceInfo& service : _config.supportedServices) {
		JsonObject serviceObj;
		serviceObj.AddMember("Name", JsonString(service.name, _allocator), _allocator);
		serviceObj.AddMember("Type", JsonString(service.type, _allocator), _allocator);
		serviceObj.AddMember("MaxCrashRestarts", service.maxCrashRestarts, _allocator);
		serviceObj.AddMember("MaxStartupRestarts", service.maxStartupRestarts, _allocator);
		supportedArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("SupportedServices", supportedArr, _allocator);
}

ot::LDSDebugInfo::ConfigInfo ot::LDSDebugInfo::setConfigFromJson(const ot::ConstJsonObject& _jsonObject) {
	ConfigInfo info;
	info.configImported = json::getBool(_jsonObject, "ConfigImported");
	info.launcherPath = json::getString(_jsonObject, "LauncherPath");
	info.servicesLibraryPath = json::getString(_jsonObject, "ServicesLibraryPath");
	info.defaultMaxCrashRestarts = json::getUInt(_jsonObject, "DefaultMaxCrashRestarts");
	info.defaultMaxStartupRestarts = json::getUInt(_jsonObject, "DefaultMaxStartupRestarts");

	for (const ConstJsonObject& serviceObj : json::getObjectList(_jsonObject, "SupportedServices")) {
		SupportedServiceInfo service;
		service.name = json::getString(serviceObj, "Name");
		service.type = json::getString(serviceObj, "Type");
		service.maxCrashRestarts = json::getUInt(serviceObj, "MaxCrashRestarts");
		service.maxStartupRestarts = json::getUInt(serviceObj, "MaxStartupRestarts");
		info.supportedServices.push_back(std::move(service));
	}

	return info;
}

void ot::LDSDebugInfo::addServiceToJson(const ServiceInfo& _service, ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("ID", _service.id, _allocator);
	_jsonObject.AddMember("Name", JsonString(_service.name, _allocator), _allocator);
	_jsonObject.AddMember("Type", JsonString(_service.type, _allocator), _allocator);
	_jsonObject.AddMember("SessionID", JsonString(_service.sessionID, _allocator), _allocator);
	_jsonObject.AddMember("LSSURL", JsonString(_service.lssUrl, _allocator), _allocator);
	_jsonObject.AddMember("URL", JsonString(_service.url, _allocator), _allocator);
	_jsonObject.AddMember("WebSocketURL", JsonString(_service.websocketUrl, _allocator), _allocator);
	_jsonObject.AddMember("StartCounter", _service.startCounter, _allocator);
	_jsonObject.AddMember("IniAttempt", _service.iniAttempt, _allocator);
	_jsonObject.AddMember("MaxCrashRestarts", _service.maxCrashRestarts, _allocator);
	_jsonObject.AddMember("MaxStartupRestarts", _service.maxStartupRestarts, _allocator);
	_jsonObject.AddMember("IsShuttingDown", _service.isShuttingDown, _allocator);
}

ot::LDSDebugInfo::ServiceInfo ot::LDSDebugInfo::setServiceFromJson(const ot::ConstJsonObject& _jsonObject) {
	ServiceInfo info;
	
	info.id = json::getUInt(_jsonObject, "ID");
	info.name = json::getString(_jsonObject, "Name");
	info.type = json::getString(_jsonObject, "Type");
	info.sessionID = json::getString(_jsonObject, "SessionID");
	info.lssUrl = json::getString(_jsonObject, "LSSURL");
	info.url = json::getString(_jsonObject, "URL");
	info.websocketUrl = json::getString(_jsonObject, "WebSocketURL");
	info.startCounter = json::getUInt(_jsonObject, "StartCounter");
	info.iniAttempt = json::getUInt(_jsonObject, "IniAttempt");
	info.maxCrashRestarts = json::getUInt(_jsonObject, "MaxCrashRestarts");
	info.maxStartupRestarts = json::getUInt(_jsonObject, "MaxStartupRestarts");
	info.isShuttingDown = json::getBool(_jsonObject, "IsShuttingDown");

	return info;
}