//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Application.h"
#include "Configuration.h"

// OpenTwin header
#include "OTSystem/PortManager.h"
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/String.h"
#include "OTCore/DebugHelper.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <thread>
#include <cstdlib>
#include <iostream>

Application& Application::instance(void) {
	static Application g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public functions

int Application::initialize(const char* _ownURL, const char* _globalDirectoryServiceURL) {
	// Read supported services from environment
	Configuration::instance().importFromEnvironment();

	if (!Configuration::instance().ensureValid()) {
		exit(ot::AppExitCode::ConfigurationBroken);
	}

	this->setServiceURL(_ownURL);

	// Filter ip and port from own url
	size_t ix = this->getServiceURL().find(':');
	if (ix == std::string::npos) {
		exit(ot::AppExitCode::ServiceUrlInvalid);
	}
	std::string ip = this->getServiceURL().substr(0, ix);
	std::string port = this->getServiceURL().substr(ix + 1);
	if (port.find(':') != std::string::npos) {
		exit(ot::AppExitCode::ServiceUrlInvalid);
	}
	bool convertFailed = false;
	ot::port_t portNr = ot::String::toNumber<ot::port_t>(port, convertFailed);
	if (convertFailed) {
		exit(ot::AppExitCode::ServiceUrlInvalid);
	}

	m_serviceManager.setServiceIP(ip);
	m_serviceManager.addPortRange(portNr + 1, ot::maxPortNumber);

	ot::DebugHelper::serviceSetupCompleted(*this);

	m_globalDirectoryService.connect(_globalDirectoryServiceURL);

	return ot::AppExitCode::Success;
}

void Application::globalDirectoryServiceCrashed(void) {
	OT_LOG_E("GDS crash not handled yet");
	exit(ot::AppExitCode::GDSNotRunning);
}

std::list<std::string> Application::getSupportedServices(void) const {
	std::list<std::string> lst;
	for (auto& s : Configuration::instance().getSupportedServices()) {
		lst.push_back(s.getName());
	}
	return lst;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

ot::ReturnMessage Application::handleStartNewService(ot::JsonDocument& _jsonDocument) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_IniData));
	
	if (m_serviceManager.requestStartService(initData) != ServiceManager::RequestResult::Success) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, m_serviceManager.lastError());
	}
	else {
		return ot::ReturnMessage::Ok;
	}
}

ot::ReturnMessage Application::handleStartNewRelayService(ot::JsonDocument& _jsonDocument) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_IniData));
	
	std::string relayServiceURL;
	std::string websocketUrl;
	
	auto supportedService = Configuration::instance().getSupportedService(initData.getServiceName());

	if (!supportedService.has_value()) {
		OT_LOG_E("The service \"" + initData.getServiceName() + "\" is not supported by this Local Directory Service");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "The service \"" + initData.getServiceName() + "\" is not supported by this Local Directory Service");
	}

	const unsigned int maxStartupRestarts = supportedService.value().getMaxStartupRestarts();

	for (unsigned int attempt = 0; attempt < maxStartupRestarts; attempt++) {
		ServiceManager::RequestResult result = m_serviceManager.requestStartRelayService(initData, relayServiceURL, websocketUrl);

		if (result == ServiceManager::RequestResult::Success) {
			OT_LOG_D("Relay service started at \"" + relayServiceURL + "\" with websocket at \"" + websocketUrl + "\"");

			ot::JsonDocument responseDoc;
			responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(relayServiceURL, responseDoc.GetAllocator()), responseDoc.GetAllocator());
			responseDoc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());

			return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDoc.toJson());
		}
		else {
			OT_LOG_W("Relay start failed on attempt " + std::to_string(attempt + 1) + "/" + std::to_string(maxStartupRestarts));
		}
	}

	OT_LOG_E("Failed to start relay service: Maximum number of start attempts reached");
	return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to start relay service: Maximum number of start attempts reached");
}

ot::ReturnMessage Application::handleSessionClosing(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	m_serviceManager.sessionClosing(sessionID);
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::handleSessionClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	m_serviceManager.sessionClosed(sessionID);
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::handleServiceClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	ot::serviceID_t serviceID = static_cast<ot::serviceID_t>(ot::json::getUInt(_jsonDocument, OT_ACTION_PARAM_SERVICE_ID));

	m_serviceManager.serviceDisconnected(sessionID, serviceID);
	return ot::ReturnMessage::Ok;
}

std::string Application::handleGetDebugInformation() {
	ot::LDSDebugInfo info;
	info.setURL(this->getServiceURL());
	info.setId(this->getServiceID());
	info.setGDSURL(m_globalDirectoryService.getServiceURL());
	info.setGDSConnected(m_globalDirectoryService.isConnected());

	m_serviceManager.getDebugInformation(info);
	Configuration::instance().getDebugInformation(info);
	
	return info.toJson();
}

std::string Application::handleGetSystemInformation() {
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
	reply.AddMember(OT_ACTION_PARAM_SUPPORTED_SERVICES, ot::JsonArray(this->getSupportedServices(), reply.GetAllocator()), reply.GetAllocator());

	// Now we add information about the sessions and their services
	ot::JsonArray sessionInfo;
	m_serviceManager.getSessionInformation(sessionInfo, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_Sessions, sessionInfo, reply.GetAllocator());

	return reply.toJson();
}

void Application::handleSetGlobalLogFlags(ot::JsonDocument& _doc) {
	ot::ConstJsonArray flags = ot::json::getArray(_doc, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(flags));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Application::Application() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_LocalDirectoryService, OT_INFO_SERVICE_TYPE_LocalDirectoryService) 
{
	m_systemLoadInformation.initialize();

	connectAction(OT_ACTION_CMD_StartNewService, this, &Application::handleStartNewService);
	connectAction(OT_ACTION_CMD_StartNewRelayService, this, &Application::handleStartNewRelayService);
	connectAction(OT_ACTION_CMD_ShutdownSession, this, &Application::handleSessionClosing);
	connectAction(OT_ACTION_CMD_ShutdownSessionCompleted, this, &Application::handleSessionClosed);
	connectAction(OT_ACTION_CMD_ServiceDisconnected, this, &Application::handleServiceClosed);
	connectAction(OT_ACTION_CMD_GetDebugInformation, this, &Application::handleGetDebugInformation);
	connectAction(OT_ACTION_CMD_GetSystemInformation, this, &Application::handleGetSystemInformation);
	connectAction(OT_ACTION_CMD_SetGlobalLogFlags, this, &Application::handleSetGlobalLogFlags);
}

Application::~Application() {}
