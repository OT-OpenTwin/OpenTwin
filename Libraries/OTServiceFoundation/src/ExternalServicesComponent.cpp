// @otlicense
// File: ExternalServicesComponent.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"

#include "OTCore/JSON.h"
#include "OTCore/DebugHelper.h"
#include "OTCore/ThisService.h"
#include "OTCore/LogDispatcher.h"
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
	const int iniTimeout = (ot::msg::defaultTimeout * 5) / 1000;

	std::this_thread::sleep_for(std::chrono::seconds(iniTimeout));

	if (ot::intern::ExternalServicesComponent::instance().componentState() != ot::intern::ExternalServicesComponent::Ready) {
		OT_LOG_E("The component was not initialized after " + std::to_string(iniTimeout) + " seconds. Shutting down service");
		exit(ot::AppExitCode::InitializationTimeout);
	}
}

ot::intern::ExternalServicesComponent::ExternalServicesComponent()
	: m_application(nullptr), m_componentState(WaitForStartup) 
{
	// Connect action handlers
	m_actionHandler.connectAction(OT_ACTION_CMD_SetLogFlags, this, &ExternalServicesComponent::handleSetLogFlags, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_Init, this, &ExternalServicesComponent::handleInitialize, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_ServiceConnected, this, &ExternalServicesComponent::handleServiceConnected, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_ServiceDisconnected, this, &ExternalServicesComponent::handleServiceDisconnected, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_ShutdownRequestedByService, this, &ExternalServicesComponent::handleShutdownRequestByService, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_ServiceShutdown, this, &ExternalServicesComponent::handleServiceShutdown, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_Run, this, &ExternalServicesComponent::handleRun, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_ServicePreShutdown, this, &ExternalServicesComponent::handlePreShutdown, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_ServiceEmergencyShutdown, this, &ExternalServicesComponent::handleEmergencyShutdown, ot::SECURE_MESSAGE_TYPES);
}

ot::intern::ExternalServicesComponent::~ExternalServicesComponent()
{
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

	// Run the init checker thread to ensure that the service will shutdown if the lds died during the startup of this service
	std::thread t{ initChecker };
	t.detach();

	return 0;
}

ot::ReturnMessage ot::intern::ExternalServicesComponent::init(const ot::ServiceInitData& _initData) {
	OTAssertNullptr(m_application);

	if (m_componentState != WaitForInit) {
		OT_LOG_EA("Component already initialized");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Component already initialized");
	}

	// First set the log flags
	ot::LogDispatcher::instance().setLogFlags(_initData.getLogFlags());
	ot::LogDispatcher::instance().setUserName(_initData.getUsername());
	m_application->logFlagsChanged(_initData.getLogFlags());

	OT_LOG_D("Starting Foundation initialization");

	// Set service parameters
	m_application->setSessionServiceURL(_initData.getSessionServiceURL());
	m_application->setSessionIDPrivate(_initData.getSessionID());
	m_application->setProjectType(_initData.getSessionType());
	m_application->setServiceID(_initData.getServiceID());
	m_application->m_lmsUrl = _initData.getLMSUrl();

	ot::ThisService::instance().setServiceID(m_application->getServiceID());
	ot::OwnerServiceGlobal::instance().setId(m_application->getServiceID());

	// Setup database parameters
	m_application->setDataBaseURL(_initData.getDatabaseUrl());
	m_application->m_dbUserCollection = _initData.getUserCollection();
	m_application->m_loggedInUserName = _initData.getUsername();
	m_application->m_loggedInUserPassword = _initData.getPassword();
	m_application->m_dataBaseUserName = _initData.getDatabaseUsername();
	m_application->m_dataBaseUserPassword = _initData.getDatabasePassword();

	// Initialize database api (project name will be set by the setSessionIDPrivate call above)
	DataBase& db = DataBase::instance();
	db.setDataBaseServerURL(_initData.getDatabaseUrl());
	db.setSiteIDString("1");
	db.setUserCredentials(_initData.getDatabaseUsername(), _initData.getDatabasePassword());

	ot::DebugHelper::serviceSetupCompleted(*m_application);

	// Register this service as a service in the session service
	JsonDocument newServiceCommandDoc;
	newServiceCommandDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_ConfirmService, newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
	newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_application->getSessionID(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
	newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->getServiceID(), newServiceCommandDoc.GetAllocator());
	newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_application->getServiceURL(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());

	// In debug mode add the process ID to the confirmation request

	unsigned long long handleID = ot::OperatingSystem::getCurrentProcessID();
	if (handleID == 0) {
		OT_LOG_EA("Failed to get current process id");
	}
	else {
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_PROCESS_ID, JsonString(std::to_string(handleID), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
	}

	// Send request
	std::string responseStr;
	if (!ot::msg::send(m_application->getServiceURL(), m_application->getSessionServiceURL(), ot::EXECUTE, newServiceCommandDoc.toJson(), responseStr)) {
		OT_LOG_E("Failed to send http request to LSS at \"" + m_application->getSessionServiceURL() + "\"");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to send register command to LSS");
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (response != ot::ReturnMessage::Ok) {
		OT_LOG_E("Failed to register service at LSS: " + response.getWhat());
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to register service at LSS");
	}

	// Ensure database connection
	if (!m_application->initializeDataBaseConnectionPrivate()) {
		OT_LOG_E("Failed to ensure database connection");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to ensure database connection");
	}

	// Initialize default template
	m_application->initializeDefaultTemplate();

	// Get the settings from the database
	PropertyGridCfg settingsConfig = m_application->createSettings();
	this->updateSettingsFromDataBase(settingsConfig);

	OT_LOG_D("Foundation setup completed with { "
		"\"LSS.Url\": \"" + _initData.getSessionServiceURL() + "\", "
		"\"Service.ID\": " + std::to_string(m_application->getServiceID()) + ", "
		"\"Session.ID\": \"" + m_application->getSessionID() + "\", "
		"\"Database.Url\": \"" + _initData.getDatabaseUrl() + "\", "
		"\"Site.ID\": \"" + DataBase::instance().getSiteIDString() + "\""
		" }"
	);

	JsonDocument reply;
	reply.fromJson(response.getWhat());

	m_componentState = Ready;

	// Start session service health check
	std::thread t{ ot::intern::sessionServiceHealthChecker, m_application->getSessionServiceURL() };
	t.detach();

	OT_LOG_D("Initialization completed");

	// Initialize the application
	m_application->initialize();

	// Notify the application about all services currently connected to the session if provided
	if (reply.HasMember(OT_ACTION_PARAM_RunData)) {
		ot::ServiceRunData runData;
		runData.setFromJsonObject(json::getObject(reply, OT_ACTION_PARAM_RunData));

		for (const ot::ServiceBase& service : runData.getServices()) {
			m_application->serviceConnectedPrivate(service);
		}
	}

	// Initialize the performance counters
	m_systemLoad.initialize();

	return ot::ReturnMessage(ot::ReturnMessage::Ok);
}

std::string ot::intern::ExternalServicesComponent::dispatchAction(
	const std::string &					_json,
	const std::string &					_sender,
	ot::MessageType						_messageType
) {
	if (m_application == nullptr) {
		OT_LOG_EA("Not initialized yet");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Not initialized yet");
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

	m_application->shuttingDownPrivate(_requestedAsCommand);
	
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

	DataBase& db = DataBase::instance();
	PropertyGridCfg oldConfig = m_application->getSettingsFromDataBase(db.getDataBaseServerURL(), db.getSiteIDString(), db.getUserName(), db.getUserPassword(), m_application->m_dbUserCollection);

	if (!oldConfig.isEmpty()) {
		_config.mergeWith(oldConfig, ot::PropertyBase::FullMerge);
	}

	m_application->settingsSynchronized(_config);
}

// #####################################################################################################################################

// Private functions

void ot::intern::ExternalServicesComponent::handleSetLogFlags(JsonDocument& _document) {
	ConstJsonArray flags = json::getArray(_document, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(logFlagsFromJsonArray(flags));
	m_application->logFlagsChanged(ot::LogDispatcher::instance().getLogFlags());
}

std::string ot::intern::ExternalServicesComponent::handleInitialize(JsonDocument& _document) {
	ot::ServiceInitData initData;
	initData.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_IniData));
	return this->init(initData).toJson();
}

void ot::intern::ExternalServicesComponent::handleServiceConnected(JsonDocument& _document) {
	serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	m_application->serviceConnectedPrivate(ot::ServiceBase(senderName, senderType, senderURL, senderID));
}

void ot::intern::ExternalServicesComponent::handleServiceDisconnected(JsonDocument& _document) {
	serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	m_application->serviceDisconnectedPrivate(senderID);
}

void ot::intern::ExternalServicesComponent::handleShutdownRequestByService() {
	shutdown(false);
}

void ot::intern::ExternalServicesComponent::handleServiceShutdown() {
	shutdown(true);
}

void ot::intern::ExternalServicesComponent::handleRun(JsonDocument& _document) {
	ot::ServiceRunData runData;
	runData.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_RunData));

	for (const ot::ServiceBase& service : runData.getServices()) {
		if (service.getServiceID() != m_application->getServiceID()) {
			m_application->serviceConnectedPrivate(service);
		}
	}

	m_application->run();
}

void ot::intern::ExternalServicesComponent::handlePreShutdown() {
	m_application->preShutdown();
}

void ot::intern::ExternalServicesComponent::handleEmergencyShutdown() {
	std::thread t(ot::intern::exitWorker, ot::AppExitCode::EmergencyShutdown);
	t.detach();
}

void ot::intern::ExternalServicesComponent::getCPUAndMemoryLoad(double& globalCPULoad, double& globalMemoryLoad, double& processCPULoad, double& processMemoryLoad)
{
	m_systemLoad.getGlobalCPUAndMemoryLoad(globalCPULoad, globalMemoryLoad);

	m_systemLoad.getCurrentProcessCPUAndMemoryLoad(processCPULoad, processMemoryLoad);
}
