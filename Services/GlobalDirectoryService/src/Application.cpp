/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "LocalDirectoryService.h"
#include "ServiceStartupInformation.h"

// Open twin header
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// C++ header
#include <iostream>
#include <list>

#define HANDLE_CHECK_MEMBER_TYPE(___doc, ___member, ___type)  if (!___doc[___member].Is##___type()) { OT_LOG_W("JSON member \"" ___member "\" invalid type"); return OT_ACTION_RETURN_INDICATOR_Error "JSON member \"" ___member "\" invalid type"; }
#define HANDLE_CHECK_MEMBER_EXISTS(___doc, ___member)  if (!___doc.HasMember(___member)) { OT_LOG_W("Missing JSON member \"" ___member "\""); return OT_ACTION_RETURN_INDICATOR_Error "JSON member \"" ___member "\" is missing"; }
#define HANDLE_CHECK_MEMBER(___doc, ___member, ___type) HANDLE_CHECK_MEMBER_EXISTS(___doc, ___member) HANDLE_CHECK_MEMBER_TYPE(___doc, ___member, ___type)

static Application * g_instance{ nullptr };

// ##################################################################################################################################

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService)
{}

Application::~Application() {}

// ##################################################################################################################################

LocalDirectoryService * Application::leastLoadedDirectoryService(const ServiceStartupInformation& _info) {
	if (m_localDirectoryServices.empty()) return nullptr;

	LoadInformation::load_t load = LoadInformation::maxLoadValue();
	LocalDirectoryService * leastLoaded = m_localDirectoryServices[0];

	for (auto lds : m_localDirectoryServices) {
		if (lds->supportsService(_info.serviceInformation().name())) {
			if (lds->load() < load) {
				load = lds->load();
				leastLoaded = lds;
			}
			else if (lds->load() == load && leastLoaded == nullptr) {
				leastLoaded = lds;
			}
		}
	}
	return leastLoaded;
}

std::string Application::handleLocalDirectoryServiceConnected(ot::JsonDocument& _jsonDocument) {
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL, String);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SUPPORTED_SERVICES, Array);

	// Check if the provided URL is already registered
	std::string ServiceURL = _jsonDocument[OT_ACTION_PARAM_SERVICE_URL].GetString();
	std::list<std::string> supportedServices;
	auto supportedServiesArray = _jsonDocument[OT_ACTION_PARAM_SUPPORTED_SERVICES].GetArray();
	for (rapidjson::SizeType i = 0; i < supportedServiesArray.Size(); i++) {
		if (!supportedServiesArray[i].IsString()) {
			OT_LOG_W("LDS connected: JSON array \"" OT_ACTION_PARAM_SUPPORTED_SERVICES "\" contains a non string entry");
			return OT_ACTION_RETURN_INDICATOR_Error ": JSON array \"" OT_ACTION_PARAM_SUPPORTED_SERVICES "\" contains a non string entry";
		}
		supportedServices.push_back(supportedServiesArray[i].GetString());
	}
	if (supportedServices.empty()) {
		OT_LOG_W("LDS Connected: No supported services provided");
		return OT_ACTION_RETURN_INDICATOR_Error ": No supported services provided";
	}

	// ##### MUTEX #####
	m_mutex.lock();

	// Check if a local directory service under the given url is already registered
	for (auto lds : m_localDirectoryServices) {
		if (lds->serviceURL() == ServiceURL) {
			// ##### MUTEX #####
			m_mutex.unlock();
			OT_LOG_E("LDS connected: A LocalDirectoryService under the given URL is already registered");
			return OT_ACTION_RETURN_INDICATOR_Error "A LocalDirectoryService under the given URL is already registered";
		}
	}

	// Store information
	LocalDirectoryService * newLds = new LocalDirectoryService(ServiceURL);
	newLds->setServiceID(m_ldsIdManager.grabNextID());
	if (!newLds->updateSystemUsageValues(_jsonDocument)) {
		// ##### MUTEX #####
		m_mutex.unlock();
		return OT_ACTION_RETURN_INDICATOR_Error "Invalid system values provided";
	}
	m_localDirectoryServices.push_back(newLds);

	// Create response
	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, newLds->serviceID(), responseDoc.GetAllocator());

	// ##### MUTEX #####
	m_mutex.unlock();

	return responseDoc.toJson();
}

std::string Application::handleStartService(ot::JsonDocument& _jsonDocument) {
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SERVICE_NAME, String);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SERVICE_TYPE, String);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_ID, String);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL, String);

	std::string serviceName = _jsonDocument[OT_ACTION_PARAM_SERVICE_NAME].GetString();
	std::string serviceType = _jsonDocument[OT_ACTION_PARAM_SERVICE_TYPE].GetString();
	std::string sessionID = _jsonDocument[OT_ACTION_PARAM_SESSION_ID].GetString();
	std::string sessionServiceURL = _jsonDocument[OT_ACTION_PARAM_SESSION_SERVICE_URL].GetString();
	
	OT_LOG_I("Service start requested: { Name: " + serviceName + "; Type: " + serviceType + "; SessionID: " + sessionID + "; LSS-URL: " + sessionServiceURL + " }");

	m_startupDispatcher.addRequest(ServiceStartupInformation(serviceName, serviceType, sessionServiceURL, sessionID));

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleStartServices(ot::JsonDocument& _jsonDocument) {
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICES, Array);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_ID, String);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL, String);

	std::string sessionID = _jsonDocument[OT_ACTION_PARAM_SESSION_ID].GetString();
	std::string sessionServiceURL = _jsonDocument[OT_ACTION_PARAM_SESSION_SERVICE_URL].GetString();

	// Iterate trough the list of services to start
	rapidjson::Value::Array services = _jsonDocument[OT_ACTION_PARAM_SESSION_SERVICES].GetArray();

	if (services.Empty()) {
		OT_LOG_W("Service array is empty");
	}

	std::list<ServiceStartupInformation> requestedServices;

	for (rapidjson::SizeType i = 0; i < services.Size(); i++) {
		if (!services[i].IsObject()) {
			OT_LOG_W("JSON array entry is not an object");
			return OT_ACTION_RETURN_INDICATOR_Error "JSON array entry is not an object";
		}

		auto service = services[i].GetObject();

		HANDLE_CHECK_MEMBER(service, OT_ACTION_PARAM_SERVICE_NAME, String);
		HANDLE_CHECK_MEMBER(service, OT_ACTION_PARAM_SERVICE_TYPE, String);

		std::string serviceName = service[OT_ACTION_PARAM_SERVICE_NAME].GetString();
		std::string serviceType = service[OT_ACTION_PARAM_SERVICE_TYPE].GetString();

		OT_LOG_I("Service start requested (Name = \"" + serviceName + "\"; Type = \"" + serviceType + "\"; SessionID = \"" + sessionID + "\"; LSS.URL = \"" + sessionServiceURL + "\")");

		requestedServices.push_back(ServiceStartupInformation(serviceName, serviceType, sessionServiceURL, sessionID));
	}

	// Add the list to the dispatcher queue
	m_startupDispatcher.addRequest(requestedServices);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleStartRelayService(ot::JsonDocument& _jsonDocument) {
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_ID, String);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL, String);

	std::string sessionID = _jsonDocument[OT_ACTION_PARAM_SESSION_ID].GetString();
	std::string sessionServiceURL = _jsonDocument[OT_ACTION_PARAM_SESSION_SERVICE_URL].GetString();

	ServiceStartupInformation info(OT_INFO_SERVICE_TYPE_RelayService, OT_INFO_SERVICE_TYPE_RelayService, sessionServiceURL, sessionID);

	// Lock application mutex and determine lds
	m_mutex.lock();
	LocalDirectoryService * lds = leastLoadedDirectoryService(info);
	if (lds == nullptr) {
		m_mutex.unlock();
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	std::string relayServiceURL;
	std::string websocketUrl;
	if (!lds->requestToRunRelayService(info, websocketUrl, relayServiceURL)) {
		m_mutex.unlock();
		OT_LOG_E("Failed to start relay service");
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	m_mutex.unlock();

	OT_LOG_I("Relay service started at \"" + relayServiceURL + "\" with websocket at \"" + websocketUrl + "\"");

	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(relayServiceURL, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());

	return responseDoc.toJson();
}

std::string Application::handleServiceStopped(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssURL = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string name = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_NAME);
	std::string type = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string url = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);

	SessionInformation sessionInfo(sessionID, lssURL);
	ServiceInformation serviceInfo(name, type);

	m_mutex.lock();
	for (auto lds : m_localDirectoryServices) {
		lds->serviceClosed(sessionInfo, serviceInfo, url);
	}
	m_mutex.unlock();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleSessionClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssURL = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	
	SessionInformation sessionInfo(sessionID, lssURL);
	
	m_mutex.lock();
	for (auto lds : m_localDirectoryServices) {
		lds->sessionClosed(sessionInfo);
	}
	m_mutex.unlock();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleUpdateSystemLoad(ot::JsonDocument& _jsonDocument) {
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SERVICE_ID, Uint);

	ot::serviceID_t id = _jsonDocument[OT_ACTION_PARAM_SERVICE_ID].GetUint();

	LocalDirectoryService * lds = nullptr;
	for (LocalDirectoryService * l : m_localDirectoryServices) {
		if (l->serviceID() == id) { lds = l; break; }
	}

	if (lds == nullptr) return OT_ACTION_RETURN_INDICATOR_Error "Unknown Local Directory Service ID";
	if (!lds->updateSystemUsageValues(_jsonDocument)) return OT_ACTION_RETURN_INDICATOR_Error "Invalid system values provided";

	return OT_ACTION_RETURN_VALUE_OK;
}

bool Application::requestToRunService(const ServiceStartupInformation& _info) {
	// Lock application mutex and determine lds
	m_mutex.lock();
	LocalDirectoryService * lds = leastLoadedDirectoryService(_info);
	if (lds == nullptr) {
		m_mutex.unlock();
		return false;
	}

	// Run service at lds, if failed erase all startup requests from the session with the given ID
	bool requestResult = lds->requestToRunService(_info);
	m_mutex.unlock();
	return requestResult;
}

int Application::initialize(const char * _siteID, const char * _ownURL, const char * _globalSessionServiceURL)
{
	setSiteId(_siteID);
	setServiceURL(_ownURL);
	m_globalSessionServiceURL = _globalSessionServiceURL;

	// Register at global session service
	ot::JsonDocument gssDoc;
	gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, ot::JsonString(m_serviceURL, gssDoc.GetAllocator()), gssDoc.GetAllocator());

	OT_LOG_I("Sending registration document to the Global Session Service");

	// Send message
	std::string gssURL(_globalSessionServiceURL);
	std::string gssResponse;
	if (!ot::msg::send(m_serviceURL, gssURL, ot::EXECUTE, gssDoc.toJson(), gssResponse)) {
		OT_LOG_E("Failed to send message to Global Session Service (url = " + gssURL + ")");
		return 1;
	}
	if (gssResponse != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invalid Global Session Service (url = " + gssURL + ") response: " + gssResponse);
		return 1;
	}
	else {
		OT_LOG_I("Registration at Global Session Service successful");
	}

	return 0;
}
