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
#include "ExitCodes.h"

// Open twin header
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTSystem/PortManager.h"
#include "OTSystem/OperatingSystem.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/TypeConverter.h"

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

}

Application::~Application()
{
	
}

// ##################################################################################################################################

// Private functions

int Application::initialize(const char * _ownURL, const char * _globalDirectoryServiceURL)
{
	// Read supported services from environment
	int cfgResult = LDS_CFG.importFromEnvironment();
	if (cfgResult != LDS_EXIT_Ok) return cfgResult;

	m_serviceURL = _ownURL;
	
	// Filter ip and port from own url
	size_t ix = m_serviceURL.find(':');
	if (ix == std::string::npos) return LDS_EXIT_InvalidURLSyntax;
	std::string ip = m_serviceURL.substr(0, ix);
	std::string port = m_serviceURL.substr(ix + 1);
	if (port.find(':') != std::string::npos) return LDS_EXIT_InvalidURLSyntax;
	bool convertFailed = false;
	ot::port_t portNr = ot::convert::toNumber<ot::port_t>(port, convertFailed);
	if (convertFailed) return LDS_EXIT_InvalidURLSyntax;
	
	m_serviceManager.setServiceIP(ip);

	ot::PortManager::instance().addPortRange(portNr + 1, 49151);

	m_globalDirectoryService.connect(_globalDirectoryServiceURL);
	return LDS_EXIT_Ok;
}

std::string Application::handleStartNewService(OT_rJSON_doc& _jsonDocument) {
	std::string serviceName = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string sessionServiceURL = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string sessionID = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);

	OT_LOG_I("Service start requested: (Name = \"" + serviceName + "\"; Type = \"" + serviceType + "\"; SessionID = \"" + sessionID + "\"; LSS-URL = \"" + sessionServiceURL + "\")");

	SessionInformation sessionInfo(sessionID, sessionServiceURL);
	ServiceInformation serviceInfo(serviceName, serviceType);

	// Get the limits from the configuration
	for (auto s : LDS_CFG.getSupportedServices()) {
		if (s.getName() == serviceInfo.getName() && s.getType() == serviceInfo.getType()) {
			serviceInfo.setMaxCrashRestarts(s.getMaxCrashRestarts());
			serviceInfo.setMaxStartupRestarts(s.getMaxStartupRestarts());
			break;
		}
	}
	
	if (!m_serviceManager.requestStartService(sessionInfo, serviceInfo)) {
		return OT_ACTION_RETURN_INDICATOR_Error + m_serviceManager.lastError();
	}
	else {
		return OT_ACTION_RETURN_VALUE_OK;
	}
}

std::string Application::handleStartNewRelayService(OT_rJSON_doc& _jsonDocument) {
	std::string sessionServiceURL = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string sessionID = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	
	OT_LOG_I("Relay Service start requested: { SessionID: " + sessionID + "; LSS-URL: " + sessionServiceURL + " }");

	SessionInformation sessionInfo(sessionID, sessionServiceURL);

	std::string relayServiceURL;
	std::string websocketUrl;
	if (!m_serviceManager.requestStartRelayService(sessionInfo, websocketUrl, relayServiceURL)) {
		OT_LOG_E("Failed to start relay service: " + m_serviceManager.lastError());
		return OT_ACTION_RETURN_INDICATOR_Error + m_serviceManager.lastError();
	}
	else {
		OT_LOG_I("Relay service started at \"" + relayServiceURL + "\" with websocket at \"" + websocketUrl + "\"");

		OT_rJSON_createDOC(responseDoc);
		ot::rJSON::add(responseDoc, OT_ACTION_PARAM_SERVICE_URL, relayServiceURL);
		ot::rJSON::add(responseDoc, OT_ACTION_PARAM_WebsocketURL, websocketUrl);
		return ot::rJSON::toJSON(responseDoc);
	}
}

std::string Application::handleSessionClosed(OT_rJSON_doc& _jsonDocument) {
	std::string sessionID = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssUrl = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	m_serviceManager.sessionClosed(sessionID);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleServiceClosed(OT_rJSON_doc& _jsonDocument) {
	std::string sessionID = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssUrl = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string name = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_NAME);
	std::string type = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string url = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);
	m_serviceManager.serviceDisconnected(sessionID, ServiceInformation(name, type), url);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleGetDebugInformation(OT_rJSON_doc& _jsonDocument) {
	OT_rJSON_createDOC(doc);
	
	OT_rJSON_createValueObject(configObj);
	LDS_CFG.addToJsonObject(doc, configObj);
	ot::rJSON::add(doc, "Configuration", configObj);

	OT_rJSON_createValueObject(serviceManagerObj);
	m_serviceManager.addToJsonObject(doc, serviceManagerObj);
	ot::rJSON::add(doc, "ServiceManager", serviceManagerObj);

	return ot::rJSON::toJSON(doc);
}

void Application::globalDirectoryServiceCrashed(void) {

}

std::list<std::string> Application::supportedServices(void) const {
	std::list<std::string> lst;
	for (auto s : LDS_CFG.getSupportedServices()) {
		lst.push_back(s.getName());
	}
	return lst;
}