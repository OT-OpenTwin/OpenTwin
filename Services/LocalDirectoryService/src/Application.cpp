/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "Configuration.h"

// OpenTwin header
#include "OTSystem/PortManager.h"
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCommunication/Msg.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// std header
#include <thread>
#include <iostream>
#include <cstdlib>

#define LocalDirectoryService OT_INFO_SERVICE_TYPE_LocalDirectoryService
#define LocalDirectory_SERVICE_TYPE OT_INFO_SERVICE_TYPE_LocalDirectoryService

Application * g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }

	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ServiceBase(LocalDirectoryService, LocalDirectory_SERVICE_TYPE)
{
	m_systemLoadInformation.initialize();
}

Application::~Application()
{
	
}

// ##################################################################################################################################

// Private functions

int Application::initialize(const char * _ownURL, const char * _globalDirectoryServiceURL)
{
	// Read supported services from environment
	LDS_CFG.importFromEnvironment();

	m_serviceURL = _ownURL;
	
	// Filter ip and port from own url
	size_t ix = m_serviceURL.find(':');
	if (ix == std::string::npos) {
		exit(ot::AppExitCode::ServiceUrlInvalid);
	}
	std::string ip = m_serviceURL.substr(0, ix);
	std::string port = m_serviceURL.substr(ix + 1);
	if (port.find(':') != std::string::npos) {
		exit(ot::AppExitCode::ServiceUrlInvalid);
	}
	bool convertFailed = false;
	ot::port_t portNr = ot::String::toNumber<ot::port_t>(port, convertFailed);
	if (convertFailed) {
		exit(ot::AppExitCode::ServiceUrlInvalid);
	}
	
	m_serviceManager.setServiceIP(ip);

	ot::PortManager::instance().addPortRange(portNr + 1, 49151);

	m_globalDirectoryService.connect(_globalDirectoryServiceURL);

	return ot::AppExitCode::Success;
}

std::string Application::handleStartNewService(ot::JsonDocument& _jsonDocument) {
	std::string serviceName = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string sessionServiceURL = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);

	OT_LOG_I("Service start requested: (Name = \"" + serviceName + "\"; Type = \"" + serviceType + "\"; SessionID = \"" + sessionID + "\"; LSS-URL = \"" + sessionServiceURL + "\")");

	SessionInformation sessionInfo(sessionID, sessionServiceURL);
	ServiceInformation serviceInfo(serviceName, serviceType);

	// Get the limits from the configuration
	for (auto s : LDS_CFG.supportedServices()) {
		if (s.name() == serviceInfo.name() && s.type() == serviceInfo.type()) {
			serviceInfo.setMaxCrashRestarts(s.maxCrashRestarts());
			serviceInfo.setMaxStartupRestarts(s.maxStartupRestarts());
			break;
		}
	}
	
	if (m_serviceManager.requestStartService(sessionInfo, serviceInfo) != ServiceManager::RequestResult::Success) {
		return OT_ACTION_RETURN_INDICATOR_Error + m_serviceManager.lastError();
	}
	else {
		return OT_ACTION_RETURN_VALUE_OK;
	}
}

std::string Application::handleStartNewRelayService(ot::JsonDocument& _jsonDocument) {
	std::string sessionServiceURL = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	
	OT_LOG_I("Relay Service start requested: { SessionID: " + sessionID + "; LSS-URL: " + sessionServiceURL + " }");

	SessionInformation sessionInfo(sessionID, sessionServiceURL);

	std::string relayServiceURL;
	std::string websocketUrl;
	
	for (unsigned int attempt = 0; attempt < Configuration::instance().defaultMaxStartupRestarts(); attempt++) {
		ServiceManager::RequestResult result = m_serviceManager.requestStartRelayService(sessionInfo, websocketUrl, relayServiceURL);
		if (result == ServiceManager::RequestResult::Success) {
			OT_LOG_I("Relay service started at \"" + relayServiceURL + "\" with websocket at \"" + websocketUrl + "\"");

			ot::JsonDocument responseDoc;
			responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(relayServiceURL, responseDoc.GetAllocator()), responseDoc.GetAllocator());
			responseDoc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());

			return responseDoc.toJson();
		}
		else {
			OT_LOG_W("Relay start failed on attempt " + std::to_string(attempt + 1) + "/" + std::to_string(Configuration::instance().defaultMaxStartupRestarts()));
		}
	}

	OT_LOG_E("Failed to start relay service: Maximum number of start attempts reached");
	return OT_ACTION_RETURN_INDICATOR_Error "Maximum number of start attempts reached";
}

std::string Application::handleSessionClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssUrl = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	m_serviceManager.sessionClosed(sessionID);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleServiceClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssUrl = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string name = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_NAME);
	std::string type = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string url = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);
	m_serviceManager.serviceDisconnected(sessionID, ServiceInformation(name, type), url);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleGetDebugInformation(ot::JsonDocument& _jsonDocument) {
	ot::JsonDocument doc;
	
	ot::JsonObject configObj;
	LDS_CFG.addToJsonObject(configObj, doc.GetAllocator());
	doc.AddMember("Configuration", configObj, doc.GetAllocator());

	ot::JsonObject serviceManagerObj;
	m_serviceManager.addToJsonObject(serviceManagerObj, doc.GetAllocator());
	doc.AddMember("ServiceManager", serviceManagerObj, doc.GetAllocator());

	return doc.toJson();
}

std::string Application::handleGetSystemInformation(ot::JsonDocument& _doc) {

	double globalCpuLoad = 0, globalMemoryLoad = 0;
	m_systemLoadInformation.getGlobalCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad);

	double processCpuLoad = 0, processMemoryLoad = 0;
	m_systemLoadInformation.getCurrentProcessCPUAndMemoryLoad(processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, "Local Directory Service", reply.GetAllocator());

	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	// Add information about the services which are supported on this node
	reply.AddMember(OT_ACTION_PARAM_SUPPORTED_SERVICES, ot::JsonArray(supportedServices(), reply.GetAllocator()), reply.GetAllocator());

	// Now we add information about the sessions and their services
	ot::JsonArray sessionInfo;
	m_serviceManager.GetSessionInformation(sessionInfo, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_Sessions, sessionInfo, reply.GetAllocator());

	return reply.toJson();
}

std::string Application::handleSetGlobalLogFlags(ot::JsonDocument& _doc) {
	ot::ConstJsonArray flags = ot::json::getArray(_doc, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(flags));
	return OT_ACTION_RETURN_VALUE_OK;
}

void Application::globalDirectoryServiceCrashed(void) {

}

std::list<std::string> Application::supportedServices(void) const {
	std::list<std::string> lst;
	for (auto s : LDS_CFG.supportedServices()) {
		lst.push_back(s.name());
	}
	return lst;
}