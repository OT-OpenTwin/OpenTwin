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
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

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

std::string Application::handleStartNewService(ot::JsonDocument& _jsonDocument) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_Config));
	
	if (m_serviceManager.requestStartService(initData) != ServiceManager::RequestResult::Success) {
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, m_serviceManager.lastError());
	}
	else {
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
	}
}

std::string Application::handleStartNewRelayService(ot::JsonDocument& _jsonDocument) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_Config));
	
	std::string relayServiceURL;
	std::string websocketUrl;
	
	auto supportedService = Configuration::instance().getSupportedService(initData.getServiceName());

	if (!supportedService.has_value()) {
		OT_LOG_E("The service \"" + initData.getServiceName() + "\" is not supported by this Local Directory Service");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "The service \"" + initData.getServiceName() + "\" is not supported by this Local Directory Service");
	}

	const unsigned int maxStartupRestarts = supportedService.value().getMaxStartupRestarts();

	for (unsigned int attempt = 0; attempt < maxStartupRestarts; attempt++) {
		ServiceManager::RequestResult result = m_serviceManager.requestStartRelayService(initData, relayServiceURL, websocketUrl);

		if (result == ServiceManager::RequestResult::Success) {
			OT_LOG_D("Relay service started at \"" + relayServiceURL + "\" with websocket at \"" + websocketUrl + "\"");

			ot::JsonDocument responseDoc;
			responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(relayServiceURL, responseDoc.GetAllocator()), responseDoc.GetAllocator());
			responseDoc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());

			return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok, responseDoc.toJson());
		}
		else {
			OT_LOG_W("Relay start failed on attempt " + std::to_string(attempt + 1) + "/" + std::to_string(maxStartupRestarts));
		}
	}

	OT_LOG_E("Failed to start relay service: Maximum number of start attempts reached");
	return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Failed to start relay service: Maximum number of start attempts reached");
}

std::string Application::handleSessionClosing(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	
	m_serviceManager.sessionClosing(sessionID);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleSessionClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	
	m_serviceManager.sessionClosed(sessionID);
	
	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleServiceClosed(ot::JsonDocument& _jsonDocument) {
	std::string sessionID = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SESSION_ID);
	ot::serviceID_t serviceID = static_cast<ot::serviceID_t>(ot::json::getUInt(_jsonDocument, OT_ACTION_PARAM_SERVICE_ID));

	m_serviceManager.serviceDisconnected(sessionID, serviceID);
	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleGetDebugInformation(ot::JsonDocument& _jsonDocument) {
	ot::JsonDocument doc;
	
	ot::JsonObject configObj;
	Configuration::instance().addToJsonObject(configObj, doc.GetAllocator());
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
	reply.AddMember(OT_ACTION_PARAM_SUPPORTED_SERVICES, ot::JsonArray(this->getSupportedServices(), reply.GetAllocator()), reply.GetAllocator());

	// Now we add information about the sessions and their services
	ot::JsonArray sessionInfo;
	m_serviceManager.getSessionInformation(sessionInfo, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_Sessions, sessionInfo, reply.GetAllocator());

	return reply.toJson();
}

std::string Application::handleSetGlobalLogFlags(ot::JsonDocument& _doc) {
	ot::ConstJsonArray flags = ot::json::getArray(_doc, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(flags));

	return OT_ACTION_RETURN_VALUE_OK;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Application::Application() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_LocalDirectoryService, OT_INFO_SERVICE_TYPE_LocalDirectoryService) {
	m_systemLoadInformation.initialize();
}

Application::~Application() {}
