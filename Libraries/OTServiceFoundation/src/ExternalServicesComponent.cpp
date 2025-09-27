/*
 *  ExternalServicesComponent.cpp
 *
 *  Created on: 01/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

// OpenTwin header
#include "OTSystem/AppExitCodes.h"

#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ThisService.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/OwnerServiceGlobal.h"

#include "OTCommunication/Msg.h"								// message sending
#include "OTCommunication/ActionTypes.h"						// action member and types definition
#include "OTCommunication/IpConverter.h"						// extract IP information from sender information
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ServiceLogNotifier.h"				// logging

#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/ExternalServicesComponent.h"	// Corresponding header

#include "OTServiceFoundation/MessageQueueHandler.h"

#include <DataBase.h>

// std header
#include <exception>									// error handling
#include <cassert>										// assert
#include <fstream>										// read from file
#include <thread>
#include <Windows.h>
#include <algorithm>
#include <cctype>

namespace ot {
	namespace intern {

		void sessionServiceHealthChecker(std::string _localSessionServiceURL) {
			OT_LOG_D("Starting Local Session Service health check (URL = \"" + _localSessionServiceURL + "\")");

			// Create ping request
			JsonDocument pingDoc;
			pingDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_Ping, pingDoc.GetAllocator());

			std::string ping = pingDoc.toJson();

			bool alive = true;
			while (alive) {
				// Wait for 20s
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(20s);

				// Try to send message and check the response
				std::string ret;
				try {
					if (!ot::msg::send("", _localSessionServiceURL, ot::EXECUTE, ping, ret)) {
						alive = false;
					}
					else if (ret != OT_ACTION_CMD_Ping) {
						alive = false;
					}
				}
				catch (const std::exception& _e) {
					OT_LOG_E(_e.what());
					alive = false;
				}
			}

			OT_LOG_E("Session service has died unexpectedly. Shutting down...");

			exit(ot::AppExitCode::LSSNotRunning);
		}

		void exitWorker(int _exitCode) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

			OT_LOG_D("Removing external services component and calling exit(" + std::to_string(_exitCode) + ")");

			exit(_exitCode);
		}

	}
}

//! @brief This function ensures that the service will shutdown if no initialization call is received during startup
void initChecker() {
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(30s);
	if (ot::intern::ExternalServicesComponent::instance().componentState() != ot::intern::ExternalServicesComponent::Ready) {
		OT_LOG_E("The component was not initialized after 30 seconds. Shutting down service");
		exit(ot::AppExitCode::InitializationTimeout);
	}
}

ot::intern::ExternalServicesComponent::ExternalServicesComponent() : m_application(nullptr), m_componentState(WaitForStartup) {}

ot::intern::ExternalServicesComponent::~ExternalServicesComponent()
{ 
	if (m_application != nullptr) delete m_application; 
	m_application = nullptr;
	
}

ot::intern::ExternalServicesComponent& ot::intern::ExternalServicesComponent::instance() {
	static ExternalServicesComponent g_component;
	return g_component;
}

// ##########################################################################################################################################

int ot::intern::ExternalServicesComponent::startup(ApplicationBase * _application, const std::string& _ownURL) {
	OT_LOG_D("Foundation startup...");

	if (m_componentState != WaitForStartup) {
		OT_LOG_EA("Component startup already performed");
		return -30;
	}

	m_application = _application;
	if (m_application == nullptr) {
		OT_LOG_EA("nullptr as application instance provided");
		return -1;
	}

	m_application->setServiceURL(_ownURL);
	MessageQueueHandler::instance().setOwnServiceURL(_ownURL);
	m_application->setSiteID("1");

	ThisService::instance().setServiceName(m_application->getServiceName());
	ThisService::instance().setServiceType(m_application->getServiceType());
	ThisService::instance().setServiceURL(m_application->getServiceURL());

	m_componentState = WaitForInit;

	OT_LOG_D("Foundation startup completed { \"ServiceName\": \"" + m_application->getServiceName() + "\", \"ServiceType\": \"" + m_application->getServiceType() + "\", \"ServiceURL\": \"" + m_application->getServiceURL() + "\" }");

	// Run the init checker thread to ensure that the service will shutdown if the lds died during the startup of this service
	std::thread t{ initChecker };
	t.detach();

	return 0;
}

std::string ot::intern::ExternalServicesComponent::init(const ot::ServiceInitData& _initData, bool _explicitDebug) {
	OTAssertNullptr(m_application);

	if (m_componentState != WaitForInit) {
		OT_LOG_EA("Component already initialized");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Component already initialized");
	}

	// First set the log flags
	ot::LogDispatcher::instance().setLogFlags(_initData.getLogFlags());
	m_application->logFlagsChanged(_initData.getLogFlags());

	OT_LOG_D("Starting Foundation initialization");

	// Set service parameters
	m_application->setSessionServiceURL(_initData.getSessionServiceURL());
	m_application->setSessionID(_initData.getSessionID());
	m_application->setServiceID(_initData.getServiceID());

	ot::ThisService::instance().setServiceID(m_application->getServiceID());
	ot::OwnerServiceGlobal::instance().setId(m_application->getServiceID());

	// Setup database parameters
	m_application->setDataBaseURL(_initData.getDatabaseUrl());
	DataBase::GetDataBase()->setDataBaseServerURL(_initData.getDatabaseUrl());
	DataBase::GetDataBase()->setSiteIDString("1");

	OT_LOG_D("Foundation setup completed with { "
		"\"LSS.Url\": \"" + _initData.getSessionServiceURL() + "\", "
		"\"Service.ID\": "+ std::to_string(m_application->getServiceID()) + ", "
		"\"Session.ID\": \"" + m_application->getSessionID() + "\", "
		"\"Database.Url\": \"" + _initData.getDatabaseUrl() + "\", "
		"\"Site.ID\": \"" + DataBase::GetDataBase()->getSiteIDString() + "\""
		" }"
	);
	
	// Register this service as a service in the session service
	{
		JsonDocument newServiceCommandDoc;
		newServiceCommandDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_ConfirmService, newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_application->getSessionID(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->getServiceID(), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_application->getServiceURL(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());

#ifdef _DEBUG
		bool useDebug = true;
#else
		bool useDebug = _explicitDebug;
#endif

		// In debug mode add the process ID to the confirmation request
		if (useDebug) {
			auto handle = GetCurrentProcess();
			if (handle != nullptr) {
				unsigned long handleID = GetProcessId(handle);
				if (handleID == 0) {
					OT_LOG_EA("Failed to get current process id");
				}
				else {
					newServiceCommandDoc.AddMember(OT_ACTION_PARAM_PROCESS_ID, JsonString(std::to_string(handleID), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
				}
			}
			else {
				OT_LOG_EA("Failed to get current process handle");
			}
		}

		// Send request
		std::string responseStr;
		if (!ot::msg::send(m_application->getServiceURL(), m_application->getSessionServiceURL(), ot::EXECUTE, newServiceCommandDoc.toJson(), responseStr)) {
			OT_LOG_E("Failed to send http request to LSS at \"" + m_application->getSessionServiceURL() + "\"");
			return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Failed to send register command to LSS");
		}

		ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

		if (response != ot::ReturnMessage::Ok) {
			OT_LOG_E("Failed to register service at LSS: " + response.getWhat());
			return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Failed to register service at LSS");
		}

		JsonDocument reply;
		reply.fromJson(response.getWhat());

		if (reply.HasMember(OT_ACTION_PARAM_LogFlags)) {
			ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(ot::json::getArray(reply, OT_ACTION_PARAM_LogFlags)));
			m_application->logFlagsChanged(ot::LogDispatcher::instance().getLogFlags());
		}

		if (m_application->startAsRelayService()) {
			m_application->setWebSocketURL(ot::json::getString(reply, OT_ACTION_PARAM_WebsocketURL));

			OT_LOG_D("Websocket URL set to: \"" + m_application->getWebSocketURL() + "\"");
		}

		if (reply.HasMember(OT_ACTION_PARAM_SESSION_SERVICES)) {
			for (auto& serviceObj : json::getObjectList(reply, OT_ACTION_PARAM_SESSION_SERVICES)) {
				ot::serviceID_t id = static_cast<ot::serviceID_t>(ot::json::getUInt(serviceObj, OT_ACTION_PARAM_SERVICE_ID));
				std::string url = json::getString(serviceObj, OT_ACTION_PARAM_SERVICE_URL);
				std::string name = json::getString(serviceObj, OT_ACTION_PARAM_SERVICE_NAME);
				std::string type = json::getString(serviceObj, OT_ACTION_PARAM_SERVICE_TYPE);
				m_application->__serviceConnected(name, type, url, id);
			}
		}
	}

	m_componentState = Ready;

	// Start session service health check
	std::thread t{ ot::intern::sessionServiceHealthChecker, m_application->getSessionServiceURL() };
	t.detach();

	OT_LOG_D("Initialization completed");

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string ot::intern::ExternalServicesComponent::dispatchAction(
	const std::string &					_json,
	const std::string &					_sender,
	ot::MessageType						_messageType
) {
	if (m_application == nullptr) {
		OT_LOG_EA("Not initialized yet");
		return std::string();
	}
	try {
		JsonDocument actionDoc;
		actionDoc.fromJson(_json);

		// Get the requested action (if the member is missing a exception will be thrown)
		std::string action = ot::json::getString(actionDoc, OT_ACTION_MEMBER);

		std::lock_guard<std::mutex> guard(m_actionDispatching);

		bool hasHandler{ false };
		std::string result = ot::ActionDispatcher::instance().dispatch(action, actionDoc, hasHandler, _messageType);

		// Prode the 
		if (!hasHandler && action == OT_ACTION_CMD_GetSystemInformation)
		{
			// Provide the default implementation for getting the service information if the service does not have a more specific handler
			result = handleGetSystemInformation();

			hasHandler = true;
		}

		if (!hasHandler) {
			result = m_application->processActionWithModalCommands(action, actionDoc);
		}
		return result;
	}
	catch (const std::exception & e) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error);
		ex.append(e.what());

		OT_LOG_E(e.what());
		return ex;
	}
}

std::string ot::intern::ExternalServicesComponent::handleGetSystemInformation() 
{
	double globalCpuLoad = 0, globalMemoryLoad = 0, processCpuLoad = 0, processMemoryLoad = 0;
	getCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad, processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	if (m_application != nullptr)
	{
		reply.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(m_application->getServiceType(), reply.GetAllocator()), reply.GetAllocator());
		reply.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_application->getSessionID(), reply.GetAllocator()), reply.GetAllocator());
		reply.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(m_application->getProjectType(), reply.GetAllocator()), reply.GetAllocator());
	}

	return reply.toJson();
}
// #####################################################################################################################################

void ot::intern::ExternalServicesComponent::shutdown(bool _requestedAsCommand) {

	OT_LOG_D("Shutdown requested");

	m_application->__shuttingDown(_requestedAsCommand);
	
	// Notify the session service about the service shutting down
	if (!_requestedAsCommand) {
		// Create a document to notify the session service about the fact that this service is closing
		JsonDocument commandDoc;
		commandDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceClosing, commandDoc.GetAllocator());
		commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->getServiceID(), commandDoc.GetAllocator());
		commandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_application->getSessionID(), commandDoc.GetAllocator()), commandDoc.GetAllocator());
		std::string response;

		OT_LOG_D("Sending shutdown notification to LSS");

		if (!ot::msg::send(m_application->getServiceURL(), m_application->getSessionServiceURL(), ot::EXECUTE, commandDoc.toJson(), response)) {
			assert(0); // Failed to send http request
		}
	}

	std::thread t(ot::intern::exitWorker, ot::AppExitCode::Success);
	t.detach();
}

void ot::intern::ExternalServicesComponent::updateSettingsFromDataBase(PropertyGridCfg& _config) {
	if (_config.isEmpty()) return;

	OTAssertNullptr(m_application);

	PropertyGridCfg oldConfig = m_application->getSettingsFromDataBase(DataBase::GetDataBase()->getDataBaseServerURL(), DataBase::GetDataBase()->getSiteIDString(), DataBase::GetDataBase()->getUserName(), DataBase::GetDataBase()->getUserPassword(), m_application->m_DBuserCollection);

	if (!oldConfig.isEmpty()) {
		_config.mergeWith(oldConfig, ot::PropertyBase::FullMerge);
	}

	m_application->settingsSynchronized(_config);
}

// #####################################################################################################################################

// Private functions

std::string ot::intern::ExternalServicesComponent::handleSetLogFlags(JsonDocument& _document) {
	ConstJsonArray flags = json::getArray(_document, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(logFlagsFromJsonArray(flags));
	m_application->logFlagsChanged(ot::LogDispatcher::instance().getLogFlags());

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleInitialize(JsonDocument& _document) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_Config));
	return this->init(initData, false);
}

std::string ot::intern::ExternalServicesComponent::handleServiceConnected(JsonDocument& _document) {
	serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	m_application->__serviceConnected(senderName, senderType, senderURL, senderID);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleServiceDisconnected(JsonDocument& _document) {
	serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	m_application->__serviceDisconnected(senderID);
	
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleShutdownRequestByService(JsonDocument& _document) {
	shutdown(false);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleMessage(JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

#ifdef _DEBUG
	std::string msg("Message from \"");
	msg.append(std::to_string(senderID)).append("\": ").append(message);
	std::cout << msg << std::endl;
	msg.append("\n");
#endif // _DEBUG

	m_application->__processMessage(message, _document, senderID);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleServiceShutdown(JsonDocument& _document) {
	shutdown(true);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleRun(JsonDocument& _document) {
	m_credentialsUsername = ot::json::getString(_document, OT_PARAM_AUTH_USERNAME);
	m_credentialsPassword = ot::json::getString(_document, OT_PARAM_AUTH_PASSWORD);
	std::string databaseUsername = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string databasePassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	m_application->m_DBuserCollection = ot::json::getString(_document, OT_PARAM_SETTINGS_USERCOLLECTION);

	ot::LogDispatcher::instance().setUserName(m_credentialsUsername);

	m_application->setProjectType(ot::json::getString(_document, OT_ACTION_PARAM_SESSION_TYPE));

	DataBase::GetDataBase()->setUserCredentials(databaseUsername, databasePassword);

	// Get the settings from the database
	PropertyGridCfg settingsConfig = m_application->createSettings();
	this->updateSettingsFromDataBase(settingsConfig);

	// Refresh the current service list and start application
	m_application->run();

	// Initialize the performance counters
	m_systemLoad.initialize();

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string ot::intern::ExternalServicesComponent::handleStartupCompleted(JsonDocument& _document) {
	// Get services in the session
	for (const auto& serviceJSON : ot::json::getObjectList(_document, OT_ACTION_PARAM_SESSION_SERVICES)) {
		std::string senderURL = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_URL);
		std::string senderName = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_NAME);
		std::string senderType = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_TYPE);
		serviceID_t senderID = ot::json::getUInt(serviceJSON, OT_ACTION_PARAM_SERVICE_ID);
		if (senderID != m_application->getServiceID()) {
			m_application->__serviceConnected(senderName, senderType, senderURL, senderID);
		}
	}

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string ot::intern::ExternalServicesComponent::handlePreShutdown(JsonDocument& _document) {
	m_application->preShutdown();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleEmergencyShutdown(JsonDocument& _document) {
	std::thread t(ot::intern::exitWorker, ot::AppExitCode::EmergencyShutdown);
	t.detach();
	return OT_ACTION_RETURN_VALUE_OK;
}

void ot::intern::ExternalServicesComponent::getCPUAndMemoryLoad(double& globalCPULoad, double& globalMemoryLoad, double& processCPULoad, double& processMemoryLoad)
{
	m_systemLoad.getGlobalCPUAndMemoryLoad(globalCPULoad, globalMemoryLoad);

	m_systemLoad.getCurrentProcessCPUAndMemoryLoad(processCPULoad, processMemoryLoad);
}
