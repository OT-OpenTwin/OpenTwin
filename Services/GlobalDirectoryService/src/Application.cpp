//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "Application.h"
#include "LocalDirectoryService.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// std header
#include <list>
#include <chrono>
#include <iostream>

#define HANDLE_CHECK_MEMBER_TYPE(___doc, ___member, ___type)  if (!___doc[___member].Is##___type()) { OT_LOG_W("JSON member \"" ___member "\" invalid type"); return OT_ACTION_RETURN_INDICATOR_Error "JSON member \"" ___member "\" invalid type"; }
#define HANDLE_CHECK_MEMBER_EXISTS(___doc, ___member)  if (!___doc.HasMember(___member)) { OT_LOG_W("Missing JSON member \"" ___member "\""); return OT_ACTION_RETURN_INDICATOR_Error "JSON member \"" ___member "\" is missing"; }
#define HANDLE_CHECK_MEMBER(___doc, ___member, ___type) HANDLE_CHECK_MEMBER_EXISTS(___doc, ___member) HANDLE_CHECK_MEMBER_TYPE(___doc, ___member, ___type)

// ###########################################################################################################################################################################################################################################################################################################################

// Static methods

Application& Application::instance(void) {
	static Application g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

bool Application::requestToRunService(const ServiceInformation& _serviceInfo) {
	// Determine LDS
	std::lock_guard<std::mutex> lock(m_mutex);
	LocalDirectoryService* lds = leastLoadedDirectoryService(_serviceInfo);

	if (lds) {
		// Run service at lds, if failed erase all startup requests from the session with the given ID
		return lds->requestToRunService(_serviceInfo);
	}
	else {
		return false;
	}
}

int Application::initialize(const char* _siteID, const char* _ownURL, const char* _globalSessionServiceURL) {
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


	// Send request to GSS
	std::string gssResponse;

	// In case of error:
	// Minimum timeout: attempts * thread sleep                  = 30 * 500ms        =   15sec
	// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;
	do {
		gssResponse.clear();

		if (!(ok = ot::msg::send(m_serviceURL, gssURL, ot::EXECUTE, gssDoc.toJson(), gssResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Register at Global Session Service (" + gssURL + ") failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Registration at Global Session Service (" + gssURL + ") failed after " + std::to_string(maxCt) + " attemts. Exiting...");
		exit(ot::AppExitCode::GSSRegistrationFailed);
	}

	return 0;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

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

	std::lock_guard<std::mutex> lock(m_mutex);
	
	// Check if a local directory service under the given url is already registered
	for (const LocalDirectoryService& lds : m_localDirectoryServices) {
		if (lds.getServiceURL() == ServiceURL) {
			OT_LOG_E("LDS connected: A LocalDirectoryService under the given URL is already registered");
			return OT_ACTION_RETURN_INDICATOR_Error "A LocalDirectoryService under the given URL is already registered";
		}
	}

	// Create new LDS entry
	LocalDirectoryService newLds(ServiceURL);
	newLds.setServiceID(m_ldsIdManager.grabNextID());
	newLds.setSupportedServices(supportedServices);

	if (!newLds.updateSystemUsageValues(_jsonDocument)) {
		return OT_ACTION_RETURN_INDICATOR_Error "Invalid system values provided";
	}

	// Create response
	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, newLds.getServiceID(), responseDoc.GetAllocator());

	if (m_logModeManager.getGlobalLogFlagsSet()) {
		ot::JsonArray flagsArr;
		ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), flagsArr, responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_GlobalLogFlags, flagsArr, responseDoc.GetAllocator());
	}

	// Add LDS entry
	m_localDirectoryServices.push_back(std::move(newLds));

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

	m_startupDispatcher.addRequest(ServiceInformation(serviceName, serviceType, sessionID, sessionServiceURL));

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

	std::list<ServiceInformation> requestedServices;

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

		requestedServices.push_back(ServiceInformation(serviceName, serviceType, sessionID, sessionServiceURL));
	}

	// Add the list to the dispatcher queue
	m_startupDispatcher.addRequest(std::move(requestedServices));

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleStartRelayService(ot::JsonDocument& _jsonDocument) {
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_ID, String);
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL, String);

	std::string sessionID = _jsonDocument[OT_ACTION_PARAM_SESSION_ID].GetString();
	std::string sessionServiceURL = _jsonDocument[OT_ACTION_PARAM_SESSION_SERVICE_URL].GetString();

	ServiceInformation info(OT_INFO_SERVICE_TYPE_RelayService, OT_INFO_SERVICE_TYPE_RelayService, sessionID, sessionServiceURL);

	// Determine LDS
	std::lock_guard<std::mutex> lock(m_mutex);
	
	LocalDirectoryService * lds = leastLoadedDirectoryService(info);
	if (lds == nullptr) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	std::string relayServiceURL;
	std::string websocketUrl;
	if (!lds->requestToRunRelayService(info, websocketUrl, relayServiceURL)) {
		OT_LOG_E("Failed to start relay service");
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	
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

	ServiceInformation serviceInfo(name, type, sessionID, lssURL);

	std::lock_guard<std::mutex> lock(m_mutex);

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		lds.serviceClosed(serviceInfo, url);
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleSessionClosing(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssURL = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);

	SessionInformation sessionInfo(sessionID, lssURL);

	std::lock_guard<std::mutex> lock(m_mutex);

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		lds.sessionClosing(sessionInfo);
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleSessionClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	std::string lssURL = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	
	SessionInformation sessionInfo(sessionID, lssURL);
	
	std::lock_guard<std::mutex> lock(m_mutex);

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		lds.sessionClosed(sessionInfo);
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleUpdateSystemLoad(ot::JsonDocument& _jsonDocument) {
	HANDLE_CHECK_MEMBER(_jsonDocument, OT_ACTION_PARAM_SERVICE_ID, Uint);

	ot::serviceID_t id = _jsonDocument[OT_ACTION_PARAM_SERVICE_ID].GetUint();

	std::lock_guard<std::mutex> lock(m_mutex);

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		if (lds.getServiceID() == id) {
			if (lds.updateSystemUsageValues(_jsonDocument)) {
				return OT_ACTION_RETURN_VALUE_OK;
			}
			else {
				return OT_ACTION_RETURN_INDICATOR_Error "Invalid system values provided";
			}
		}
	}

	return OT_ACTION_RETURN_INDICATOR_Error "Unknown Local Directory Service ID";
}

std::string Application::handleGetSystemInformation(ot::JsonDocument& _doc) {
	double globalCpuLoad = 0, globalMemoryLoad = 0;
	m_systemLoadInformation.getGlobalCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad);

	double processCpuLoad = 0, processMemoryLoad = 0;
	m_systemLoadInformation.getCurrentProcessCPUAndMemoryLoad(processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	std::lock_guard<std::mutex> lock(m_mutex);

	// Now we add information about the session services
	ot::JsonArray servicesInfo;
	for (const LocalDirectoryService& service : m_localDirectoryServices) {
		ot::JsonObject info;
		info.AddMember(OT_ACTION_PARAM_LDS_URL, ot::JsonString(service.getServiceURL(), reply.GetAllocator()), reply.GetAllocator());
		servicesInfo.PushBack(info, reply.GetAllocator());
	}
	reply.AddMember(OT_ACTION_PARAM_LDS, servicesInfo, reply.GetAllocator());

	return reply.toJson();
}

std::string Application::handleSetGlobalLogFlags(ot::JsonDocument& _doc) {
	ot::ConstJsonArray flags = ot::json::getArray(_doc, OT_ACTION_PARAM_Flags);
	m_logModeManager.setGlobalLogFlags(ot::logFlagsFromJsonArray(flags));

	ot::LogDispatcher::instance().setLogFlags(m_logModeManager.getGlobalLogFlags());

	// Update existing session services
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_SetGlobalLogFlags, doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, flagsArr, doc.GetAllocator());

	std::lock_guard<std::mutex> lock(m_mutex);

	std::string json = doc.toJson();
	for (const LocalDirectoryService& lds : m_localDirectoryServices) {
		std::string response;
		if (!ot::msg::send("", lds.getServiceURL(), ot::EXECUTE, json, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_EAS("Failed to send message to LSS at \"" + lds.getServiceURL() + "\"");
		}
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleGetDebugInformation(ot::JsonDocument& _doc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::JsonDocument doc;

	doc.AddMember("GSS.URL", ot::JsonString(m_globalSessionServiceURL, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonArray ldsArr;
	for (const LocalDirectoryService& lds : m_localDirectoryServices) {
		ot::JsonObject ldsObj;
		lds.addToJsonObject(ldsObj, doc.GetAllocator());
		ldsArr.PushBack(ldsObj, doc.GetAllocator());
	}
	doc.AddMember("LDS.Info", ldsArr, doc.GetAllocator());

	ot::JsonObject startupObj;
	m_startupDispatcher.addToJsonObject(startupObj, doc.GetAllocator());
	doc.AddMember("StartupDispatcher", startupObj, doc.GetAllocator());

	ot::JsonObject logManagerObj;
	m_logModeManager.addToJsonObject(logManagerObj, doc.GetAllocator());
	doc.AddMember("LogModeManager", logManagerObj, doc.GetAllocator());

	return doc.toJson();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private methods

LocalDirectoryService* Application::leastLoadedDirectoryService(const ServiceInformation& _info) {
	if (m_localDirectoryServices.empty()) {
		return nullptr;
	}

	LoadInformation::load_t load = LoadInformation::maxLoadValue();
	LocalDirectoryService* leastLoaded = nullptr;

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		if (lds.supportsService(_info.getName())) {
			if (lds.load() < load) {
				load = lds.load();
				leastLoaded = &lds;
			}
			else if (lds.load() == load && leastLoaded == nullptr) {
				leastLoaded = &lds;
			}
		}
	}
	return leastLoaded;
}

Application::Application()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService) {
	m_systemLoadInformation.initialize();
}

Application::~Application() {}