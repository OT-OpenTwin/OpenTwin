//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "Application.h"
#include "LocalDirectoryService.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/DebugHelper.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/GDSDebugInfo.h"
#include "OTCommunication/ServiceInitData.h"
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

bool Application::requestToRunService(const ot::ServiceInitData& _serviceInfo) {
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
	setSiteID(_siteID);
	setServiceURL(_ownURL);
	m_globalSessionServiceURL = _globalSessionServiceURL;

	ot::DebugHelper::serviceSetupCompleted(*this);

	// Register at global session service
	ot::JsonDocument gssDoc;
	gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, ot::JsonString(this->getServiceURL(), gssDoc.GetAllocator()), gssDoc.GetAllocator());

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

		if (!(ok = ot::msg::send(this->getServiceURL(), gssURL, ot::EXECUTE, gssDoc.toJson(), gssResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
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
	newLds.setServiceID(m_ldsIdManager.nextID());
	newLds.setSupportedServices(supportedServices);

	if (!newLds.updateSystemUsageValues(_jsonDocument)) {
		return OT_ACTION_RETURN_INDICATOR_Error "Invalid system values provided";
	}

	// Create response
	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, newLds.getServiceID(), responseDoc.GetAllocator());

	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(ot::LogDispatcher::instance().getLogFlags(), flagsArr, responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_GlobalLogFlags, flagsArr, responseDoc.GetAllocator());

	// Add LDS entry
	m_localDirectoryServices.push_back(std::move(newLds));

	return responseDoc.toJson();
}

std::string Application::handleStartService(ot::JsonDocument& _jsonDocument) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_IniData));

	OT_LOG_D("Service start requested: { \"ID\": " + std::to_string(initData.getServiceID()) + ", \"Name\": " + initData.getServiceName() + 
		"\", \"Type\": \"" + initData.getServiceType() + "\", \"SessionID\": \"" + initData.getSessionID() + "\", \"LssUrl\": \"" + initData.getSessionServiceURL() + "\" }");

	if (!this->canStartService(initData)) {
		OT_LOG_W("Service \"" + initData.getServiceName() + "\" of type \"" + initData.getServiceType() + "\" cannot be started.");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Service \"" + initData.getServiceName() + "\" of type \"" + initData.getServiceType() + "\" is not supported by any LDS");
	}
	
	m_startupDispatcher.addRequest(std::move(initData));
	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleStartServices(ot::JsonDocument& _jsonDocument) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_IniData));

	std::list<ot::ServiceInitData> requestedServices;

	for (const ot::ConstJsonObject& serviceObj : ot::json::getObjectList(_jsonDocument, OT_ACTION_PARAM_SESSION_SERVICES)) {
		ot::ServiceInitData info(initData);
		info.setServiceID(static_cast<ot::serviceID_t>(ot::json::getUInt(serviceObj, OT_ACTION_PARAM_SERVICE_ID)));
		info.setServiceName(ot::json::getString(serviceObj, OT_ACTION_PARAM_SERVICE_NAME));
		info.setServiceType(ot::json::getString(serviceObj, OT_ACTION_PARAM_SERVICE_TYPE));

		OT_LOG_D("Service start requested { \"ServiceID\": " + std::to_string(info.getServiceID()) + ", "
			"\"ServiceName\": \"" + info.getServiceName() + "\", "
			"\"ServiceType\": \"" + info.getServiceType() + "\", "
			"\"SessionID\": \"" + info.getSessionID() + "\", "
			"\"LssUrl\": \"" + info.getSessionServiceURL() +
			" }"
		);

		if (!this->canStartService(info)) {
			OT_LOG_W("Service \"" + info.getServiceName() + "\" of type \"" + info.getServiceType() + "\" cannot be started.");
			return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Service \"" + info.getServiceName() + "\" of type \"" + info.getServiceType() + "\" is not supported by any LDS");
		}

		requestedServices.push_back(std::move(info));
	}

	if (requestedServices.empty()) {
		OT_LOG_W("No services requested to start");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "No services requested to start");
	}

	// Add the list to the dispatcher queue
	m_startupDispatcher.addRequest(std::move(requestedServices));

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleStartRelayService(ot::JsonDocument& _jsonDocument) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_IniData));

	// Determine LDS
	std::lock_guard<std::mutex> lock(m_mutex);
	
	LocalDirectoryService * lds = leastLoadedDirectoryService(initData);
	if (lds == nullptr) {
		OT_LOG_E("No LDS available to start relay service");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "No LDS available to start relay service");
	}
	std::string relayServiceURL;
	std::string websocketUrl;
	if (!lds->requestToRunRelayService(initData, websocketUrl, relayServiceURL)) {
		OT_LOG_E("Failed to start relay service");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Failed to start relay service");
	}
	
	OT_LOG_I("Relay service started at \"" + relayServiceURL + "\" with websocket at \"" + websocketUrl + "\"");

	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(relayServiceURL, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok, responseDoc.toJson());
}

std::string Application::handleServiceStopped(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	ot::serviceID_t serviceID = static_cast<ot::serviceID_t>(ot::json::getUInt(_jsonDocument, OT_ACTION_PARAM_SERVICE_ID));

	std::lock_guard<std::mutex> lock(m_mutex);

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		lds.serviceClosed(sessionID, serviceID);
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleSessionClosing(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	
	std::lock_guard<std::mutex> lock(m_mutex);

	m_startupDispatcher.sessionClosing(sessionID);

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		lds.sessionClosing(sessionID);
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleSessionClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	
	std::lock_guard<std::mutex> lock(m_mutex);

	m_startupDispatcher.sessionClosing(sessionID);

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		lds.sessionClosed(sessionID);
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
	ot::LogFlags flags = ot::logFlagsFromJsonArray(ot::json::getArray(_doc, OT_ACTION_PARAM_Flags));
	ot::LogDispatcher::instance().setLogFlags(flags);

	// Update existing session services
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_SetGlobalLogFlags, doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(flags, flagsArr, doc.GetAllocator());
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

	ot::GDSDebugInfo info;

	info.setURL(this->getServiceURL());
	info.setGSSUrl(m_globalSessionServiceURL);
	m_startupDispatcher.getDebugInformation(info);

	for (const LocalDirectoryService& lds : m_localDirectoryServices) {
		ot::GDSDebugInfo::LDSInfo ldsInfo;
		lds.getDebugInformation(ldsInfo);
		info.addLocalDirectoryService(std::move(ldsInfo));
	}

	return info.toJson();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private methods

LocalDirectoryService* Application::leastLoadedDirectoryService(const ot::ServiceInitData& _info) {
	if (m_localDirectoryServices.empty()) {
		OT_LOG_E("Failed to determine least loaded directory service: No LDS connected");
		return nullptr;
	}

	LoadInformation::load_t load = LoadInformation::maxLoadValue();
	LocalDirectoryService* leastLoaded = nullptr;

	for (LocalDirectoryService& lds : m_localDirectoryServices) {
		if (lds.supportsService(_info.getServiceName())) {
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

bool Application::canStartService(const ot::ServiceInitData& _info) const {
	for (const LocalDirectoryService& lds : m_localDirectoryServices) {
		if (lds.supportsService(_info.getServiceName())) {
			return true;
		}
	}
	
	return false;
}

Application::Application()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService) {
	m_systemLoadInformation.initialize();
}

Application::~Application() {}