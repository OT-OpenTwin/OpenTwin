/*
 *  ExternalServicesComponent.cpp
 *
 *  Created on: 01/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

// OpenTwin header
#include "OTCore/otAssert.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/OwnerServiceGlobal.h"

#include "OTCommunication/ActionTypes.h"						// action member and types definition
#include "OTCommunication/IpConverter.h"						// extract IP information from sender information
#include "OTCommunication/Msg.h"								// message sending
#include "OTCommunication/ServiceLogNotifier.h"				// logging

#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/Dispatcher.h"
#include "OTServiceFoundation/SettingsData.h"
#include "OTServiceFoundation/UiPluginComponent.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ExternalServicesComponent.h"	// Corresponding header

#include <DataBase.h>

// std header

#include <exception>									// error handling
#include <cassert>										// assert
#include <fstream>										// read from file
#include <thread>
#include <Windows.h>
#include <algorithm>
#include <cctype>

static ot::intern::ExternalServicesComponent * g_component{ nullptr };

namespace ot {
	namespace intern {

		void sessionServiceHealthChecker(std::string _sessionServiceURL) {
			OT_LOG_D("Starting Local Session Service health check (URL = \"" + _sessionServiceURL + "\")");

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
					if (!ot::msg::send("", _sessionServiceURL, ot::EXECUTE, ping, ret)) { alive = false; }
					else OT_ACTION_IF_RESPONSE_ERROR(ret) { alive = false; }
			else OT_ACTION_IF_RESPONSE_WARNING(ret) { alive = false; }
			else if (ret != OT_ACTION_CMD_Ping) { alive = false; }
				}
				catch (...) { alive = false; }
			}

			OT_LOG_E("Session service has died unexpectedly. Shutting down...");

			exit(0);
		}

		void exitWorker(int _exitCode) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

			OT_LOG_D("Removing external services component and calling exit()");

			ot::intern::ExternalServicesComponent::deleteInstance();
			exit(_exitCode);
		}

	}
}

//! @brief This function ensures that the service will shutdown if no initialization call is received during startup
void initChecker(void) {
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(30s);
	if (ot::intern::ExternalServicesComponent::instance()->componentState() != ot::intern::ExternalServicesComponent::Ready) {
		OT_LOG_E("The component was not initialized after 30 seconds. Shutting down service");
		exit(0);
	}
}

ot::intern::ExternalServicesComponent::ExternalServicesComponent() : m_application(nullptr), m_componentState(WaitForStartup) {}

ot::intern::ExternalServicesComponent::~ExternalServicesComponent()
{ 
	if (m_application != nullptr) delete m_application; 
	m_application = nullptr;
	
}

ot::intern::ExternalServicesComponent * ot::intern::ExternalServicesComponent::instance() {
	if (g_component == nullptr) g_component = new ExternalServicesComponent;
	return g_component;
}

bool ot::intern::ExternalServicesComponent::hasInstance(void) {
	return (bool)g_component;
}

void ot::intern::ExternalServicesComponent::deleteInstance(void) {
	if (g_component) delete g_component;
	g_component = nullptr;
}

// ##########################################################################################################################################

int ot::intern::ExternalServicesComponent::startup(ApplicationBase * _application, const std::string& _localDirectoryServiceURL, const std::string& _ownURL) {
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
	m_application->setDirectoryServiceURL(_localDirectoryServiceURL);
	m_application->setSiteID("1");

	m_componentState = WaitForInit;

	OT_LOG_D("Foundation startup completed { \"ServiceName\": \"" + m_application->serviceName() + "\"; \"ServiceType\": \"" + m_application->serviceType() + "\"; \"ServiceURL\": \"" + m_application->serviceURL() + "\"; \"LDS\": \"" + m_application->directoryServiceURL() + "\" }");

	// Run the init checker thread to ensure that the service will shutdown if the lds died during the startup of this service
	std::thread t{ initChecker };
	t.detach();

	return 0;
}

std::string ot::intern::ExternalServicesComponent::init(
	const std::string &					_sessionServiceURL,
	const std::string &					_sessionID
) {
	if (m_componentState != WaitForInit) {
		OT_LOG_EA("Component already initialized");
		return OT_ACTION_RETURN_INDICATOR_Error "Component already initialized";
	}

	OT_LOG_D("Starting Foundation initialization");

	// Store information
	m_application->setSessionServiceURL(_sessionServiceURL);
	m_application->setSessionID(_sessionID);
	
	// Get the database information
	{
		JsonDocument request;
		
		// todo: maybe merge get database url and register new service into one call? (nothin else is happening before anyway
		request.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_GetDatabaseUrl, request.GetAllocator());

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, request.toJson(), response)) {
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send get database URL message";
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			return response;
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			return response;
		}

		m_application->setDataBaseURL(response);
		DataBase::GetDataBase()->setDataBaseServerURL(response);
		DataBase::GetDataBase()->setSiteIDString("1");
	}

	OT_LOG_D("Database setup completed (URL = \"" + DataBase::GetDataBase()->getDataBaseServerURL() + "\"; Site.ID = \"" + DataBase::GetDataBase()->getSiteIDString());

	// Register this service as a service in the session service
	{
		JsonDocument newServiceCommandDoc;
		newServiceCommandDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_RegisterNewService, newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_application->sessionID(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, JsonString(m_application->serviceName(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_PORT, JsonString(ot::IpConverter::portFromIp(m_application->serviceURL()), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, JsonString(m_application->serviceType(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_START_RELAY, m_application->startAsRelayService(), newServiceCommandDoc.GetAllocator());

#ifdef _DEBUG
		auto handle = GetCurrentProcess();
		if (handle != nullptr) {
			unsigned long handleID = GetProcessId(handle);
			assert(handleID != 0); // Failed to get process handle ID
			newServiceCommandDoc.AddMember(OT_ACTION_PARAM_PROCESS_ID, JsonString(std::to_string(handleID), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		}
		else {
			assert(0); // Failed to get current process handle
		}
#endif // _DEBUG

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, newServiceCommandDoc.toJson(), response)) {
			OT_LOG_E("Failed to send http request to LSS at \"" + m_application->sessionServiceURL() + "\"");
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send register command to LSS";
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_E("Error response from LSS: \"" + response + "\"");
			return response;
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_E("Warning response from LSS: \"" + response + "\"");
			return response;
		}

		JsonDocument reply;
		reply.fromJson(response);

		m_application->setServiceID(json::getInt(reply, OT_ACTION_PARAM_SERVICE_ID));
		ot::OwnerServiceGlobal::instance().setId(m_application->serviceID());

		OT_LOG_D("Service ID set to: \"" + std::to_string(m_application->serviceID()) + "\"");

		if (m_application->startAsRelayService()) {
			m_application->setWebSocketURL(ot::json::getString(reply, OT_ACTION_PARAM_WebsocketURL));

			OT_LOG_D("Websocket URL set to: \"" + m_application->webSocketURL() + "\"");
		}

	}

	m_componentState = Ready;

	// Start session service health check
	std::thread t{ ot::intern::sessionServiceHealthChecker, m_application->sessionServiceURL() };
	t.detach();

	OT_LOG_D("Initialization completed");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::initDebugExplicit(const std::string& _sessionServiceURL, const std::string& _sessionID)
{
	if (m_componentState != WaitForInit) {
		OT_LOG_EA("Component already initialized");
		return OT_ACTION_RETURN_INDICATOR_Error "Component already initialized";
	}

	OT_LOG_D("Starting Foundation initialization");

	// Store information
	m_application->setSessionServiceURL(_sessionServiceURL);
	m_application->setSessionID(_sessionID);

	// Get the database information
	{
		JsonDocument request;

		// todo: maybe merge get database url and register new service into one call? (nothin else is happening before anyway
		request.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_GetDatabaseUrl, request.GetAllocator());

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, request.toJson(), response)) {
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send get database URL message";
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			return response;
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			return response;
		}

		m_application->setDataBaseURL(response);
		DataBase::GetDataBase()->setDataBaseServerURL(response);
		DataBase::GetDataBase()->setSiteIDString("1");
	}

	OT_LOG_D("Database setup completed (URL = \"" + DataBase::GetDataBase()->getDataBaseServerURL() + "\"; Site.ID = \"" + DataBase::GetDataBase()->getSiteIDString());

	// Register this service as a service in the session service
	{
		JsonDocument newServiceCommandDoc;
		newServiceCommandDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_RegisterNewService, newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_application->sessionID(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, JsonString(m_application->serviceName(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_PORT, JsonString(ot::IpConverter::portFromIp(m_application->serviceURL()), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, JsonString(m_application->serviceType(), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		newServiceCommandDoc.AddMember(OT_ACTION_PARAM_START_RELAY, m_application->startAsRelayService(), newServiceCommandDoc.GetAllocator());


		auto handle = GetCurrentProcess();
		if (handle != nullptr) {
			unsigned long handleID = GetProcessId(handle);
			assert(handleID != 0); // Failed to get process handle ID
			newServiceCommandDoc.AddMember(OT_ACTION_PARAM_PROCESS_ID, JsonString(std::to_string(handleID), newServiceCommandDoc.GetAllocator()), newServiceCommandDoc.GetAllocator());
		}
		else {
			assert(0); // Failed to get current process handle
		}


		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, newServiceCommandDoc.toJson(), response)) {
			OT_LOG_E("Failed to send http request to LSS at \"" + m_application->sessionServiceURL() + "\"");
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send register command to LSS";
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_E("Error response from LSS: \"" + response + "\"");
			return response;
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_E("Warning response from LSS: \"" + response + "\"");
			return response;
		}

		JsonDocument reply;
		reply.fromJson(response);
		m_application->setServiceID(ot::json::getInt(reply, OT_ACTION_PARAM_SERVICE_ID));
		ot::OwnerServiceGlobal::instance().setId(m_application->serviceID());

		OT_LOG_D("Service ID set to: \"" + std::to_string(m_application->serviceID()) + "\"");

		if (m_application->startAsRelayService()) {
			m_application->setWebSocketURL(ot::json::getString(reply, OT_ACTION_PARAM_WebsocketURL));

			OT_LOG_D("Websocket URL set to: \"" + m_application->webSocketURL() + "\"");
		}

	}

	m_componentState = Ready;

	// Start session service health check
	std::thread t{ ot::intern::sessionServiceHealthChecker, m_application->sessionServiceURL() };
	t.detach();

	OT_LOG_D("Initialization completed");

	return OT_ACTION_RETURN_VALUE_OK;
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
		
		bool hasHandler{ false };
		std::string result = ot::Dispatcher::instance()->dispatch(action, actionDoc, hasHandler, _messageType);

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
	catch (...) {
		OT_LOG_E("[FATAL] Unknown error occured");
		return OT_ACTION_RETURN_UnknownError;
	}
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
		commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), commandDoc.GetAllocator());
		commandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_application->sessionID(), commandDoc.GetAllocator()), commandDoc.GetAllocator());
		std::string response;

		OT_LOG_D("Sending shutdown notification to LSS");

		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, commandDoc.toJson(), response)) {
			assert(0); // Failed to send http request
		}
	}

	std::thread t(ot::intern::exitWorker, 0);
	t.detach();
}

// #####################################################################################################################################

// Private functions

std::string ot::intern::ExternalServicesComponent::handleInitialize(JsonDocument& _document) {
	std::string serviceName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string sessionServiceURL = ot::json::getString(_document, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string sessionID = ot::json::getString(_document, OT_ACTION_PARAM_SESSION_ID);

	return this->init(sessionServiceURL, sessionID);
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
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	m_application->__serviceDisconnected(senderName, senderType, senderURL, senderID);
	
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
	std::string credentialsUsername = ot::json::getString(_document, OT_PARAM_AUTH_USERNAME);
	std::string credentialsPassword = ot::json::getString(_document, OT_PARAM_AUTH_PASSWORD);
	m_application->m_DBuserCollection = ot::json::getString(_document, OT_PARAM_SETTINGS_USERCOLLECTION);

	DataBase::GetDataBase()->setUserCredentials(credentialsUsername, credentialsPassword);

	// Change the service to visible
	{
		JsonDocument visibilityCommand;
		visibilityCommand.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceShow, visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_application->sessionID(), visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, visibilityCommand.toJson(), response)) {
			std::cout << OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request" << std::endl;
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request";
		}
		if (response.rfind(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) {
			std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
			return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
		}
		else if (response.rfind(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
			std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
			return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
		}

		JsonDocument responseDoc;
		responseDoc.fromJson(response);

		{
			SettingsData * serviceSettings = m_application->createSettings();
			if (serviceSettings) {
				if (serviceSettings->refreshValuesFromDatabase(DataBase::GetDataBase()->getDataBaseServerURL(), 
															   DataBase::GetDataBase()->getSiteIDString(), 
															   DataBase::GetDataBase()->getUserName(), 
															   DataBase::GetDataBase()->getUserPassword(), 
															   m_application->m_DBuserCollection)) 
				{
					m_application->settingsSynchronized(serviceSettings);
				}
				delete serviceSettings;
			}
		}

		auto serviceList = ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_SESSION_SERVICES);
		for (auto serviceJSON : serviceList) {
			std::string senderURL = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_URL);
			std::string senderName = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_NAME);
			std::string senderType = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_TYPE);
			serviceID_t senderID = ot::json::getUInt(serviceJSON, OT_ACTION_PARAM_SERVICE_ID);
			if (senderID != m_application->serviceID()) {
				m_application->__serviceConnected(senderName, senderType, senderURL, senderID);
			}
		}
	}

	// Refresh the current service list and start application
	m_application->run();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handlePreShutdown(JsonDocument& _document) {
	m_application->preShutdown();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleEmergencyShutdown(JsonDocument& _document) {
	std::thread t(ot::intern::exitWorker, -100);
	t.detach();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleUIPluginConnected(JsonDocument& _document) {
	components::UiComponent * ui = m_application->uiComponent();
	unsigned long long pluginUID = ot::json::getUInt64(_document, OT_ACTION_PARAM_UI_PLUGIN_UID);
	std::string pluginName = ot::json::getString(_document, OT_ACTION_PARAM_UI_PLUGIN_NAME);
	if (ui == nullptr) {
		return OT_ACTION_RETURN_INDICATOR_Error "UI is not connected";
	}

	components::UiPluginComponent * component = new components::UiPluginComponent(pluginUID, pluginName, m_application, ui);
	m_application->__addUiPlugin(component);
	m_application->uiPluginConnected(component);
	return std::string();
}

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

// Microservice calls

//! @brief This function is used to wrap the code that is required to forward a message to the dispatch in the external services component
const char * dispatchActionWrapper(const std::string& _json, const std::string& _senderUrl, ot::MessageType _messageType) {
	std::string response;
	try {
		if (ot::intern::ExternalServicesComponent::hasInstance()) {
			response = ot::intern::ExternalServicesComponent::instance()->dispatchAction(_json, _senderUrl, _messageType);
		}
		else {
			OT_LOG_EA("Component not initialized");

			response = OT_ACTION_RETURN_INDICATOR_Error "Component not initialized";
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());

		response = OT_ACTION_RETURN_INDICATOR_Error;
		response.append(_e.what());
	}
	catch (...) {
		OT_LOG_EAS("[FATAL] Unknown error occured");

		response = OT_ACTION_RETURN_INDICATOR_Error "[FATAL] Unknown error occured";
	}

	// Copy the return value.The memory of this value will be deallocated in the deallocateData function
	char * retVal = new char[response.length() + 1];
	strcpy_s(retVal, response.length() + 1, response.c_str());

	return retVal;
}

const char * ot::foundation::performAction(const std::string & _json, const std::string & _senderIP)
{
	return dispatchActionWrapper(_json, _senderIP, ot::EXECUTE);
};

const char * ot::foundation::performActionOneWayTLS(const std::string & _json, const std::string & _senderIP)
{
	return dispatchActionWrapper(_json, _senderIP, ot::EXECUTE_ONE_WAY_TLS);
};

const char * ot::foundation::queueAction(const std::string & _json, const std::string & _senderIP)
{
	return dispatchActionWrapper(_json, _senderIP, ot::QUEUE);
};

const char * ot::foundation::getServiceURL(void)
{
	try {
		std::string serviceURL = ot::intern::ExternalServicesComponent::instance()->application()->serviceURL();

		char * retVal = new char[serviceURL.length() + 1];
		strcpy_s(retVal, serviceURL.length() + 1, serviceURL.c_str());
		return retVal;
	}
	catch (const std::exception& _e) {
		std::cout << "[ERROR] [FOUNDATION]: getServiceURL: " << _e.what() << std::endl;
	}
	catch (...) {
		std::cout << "[ERROR] [FOUNDATION]: getServiceURL: Unknown error" << std::endl;
	}
	char * retVal = new char[1]{ 0 };
	return retVal;
}

int ot::foundation::init(
	const std::string &					_localDirectoryServiceURL,
	const std::string &					_ownIP,
	const std::string &					_sessionServiceIP,
	const std::string &					_sessionID,
	ApplicationBase *					_application
) {
	try {
		// Setup logger
#ifdef _DEBUG
		if (_application) ot::ServiceLogNotifier::initialize(_application->serviceName(), "", true);
		else ot::ServiceLogNotifier::initialize("<NO APPLICATION>", "", true);
#else
		if (_application) ot::ServiceLogNotifier::initialize(_application->serviceName(), "", false);
		else ot::ServiceLogNotifier::initialize("<NO APPLICATION>", "", false);
#endif

		// The following code is used to make the service lauchable in debug mode in the editor when the session service is requesting the service to start
		// In addition, if an empty siteID is passed to the service, it also reads its information from the config file. This allows for debugging services
		// which are built in release mode.

#ifdef _DEBUG
		// Get file path
		std::string deplyomentPath = _application->deploymentPath();
		if (deplyomentPath.empty()) return -20;

		std::string data = _application->serviceName();
		std::transform(data.begin(), data.end(), data.begin(),
			[](unsigned char c) { return std::tolower(c); });

		deplyomentPath.append(data + ".cfg");

		// Read file
		std::ifstream stream(deplyomentPath);
		char inBuffer[512];
		stream.getline(inBuffer, 512);
		std::string info(inBuffer);

		if (info.empty()) {
			std::cout << "No configuration found" << std::endl;
			assert(0);
			return -21;
		}
		// Parse doc
		JsonDocument params;
		params.fromJson(info);

		OT_LOG_I("Application parameters were overwritten by configuration file: " + deplyomentPath);

		std::string actualServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SERVICE_URL);
		std::string actualSessionServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
		std::string actualLocalDirectoryServiceURL = ot::json::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);
		std::string actualSessionID = ot::json::getString(params, OT_ACTION_PARAM_SESSION_ID);
		// Initialize the service with the parameters from the file

		int startupResult = intern::ExternalServicesComponent::instance()->startup(_application, actualLocalDirectoryServiceURL, actualServiceURL);
		if (startupResult != 0) {
			return startupResult;
		}

		std::string initResult = intern::ExternalServicesComponent::instance()->init(actualSessionServiceURL, actualSessionID);
		if (initResult != OT_ACTION_RETURN_VALUE_OK) {
			return -22;
		}
		else {
			return 0;
		}
#else
		return intern::ExternalServicesComponent::instance()->startup(_application, _localDirectoryServiceURL, _ownIP);
#endif
	}
	catch (const std::exception & e) {
		std::cout << "[INIT] ERROR: " << e.what() << std::endl;
		return -1;
	}
	catch (...) {
		std::cout << "[INIT] ERROR: Unknown error" << std::endl;
		return -2;
	}
}


int ot::foundation::initDebugExplicit(
	const std::string& _localDirectoryServiceURL,
	const std::string& _ownIP,
	const std::string& _sessionServiceIP,
	const std::string& _sessionID,
	ApplicationBase* _application
) {
	try {
		// Setup logger
		if (_application) ot::ServiceLogNotifier::initialize(_application->serviceName(), "", true);
		else ot::ServiceLogNotifier::initialize("<NO APPLICATION>", "", true);


		// The following code is used to make the service lauchable in debug mode in the editor when the session service is requesting the service to start
		// In addition, if an empty siteID is passed to the service, it also reads its information from the config file. This allows for debugging services
		// which are built in release mode.

		// Get file path
		std::string deplyomentPath = _application->deploymentPath();
		if (deplyomentPath.empty()) return -20;

		std::string data = _application->serviceName();
		std::transform(data.begin(), data.end(), data.begin(),
			[](unsigned char c) { return std::tolower(c); });

		deplyomentPath.append(data + ".cfg");

		// Read file
		std::ifstream stream(deplyomentPath);
		char inBuffer[512];
		stream.getline(inBuffer, 512);
		std::string info(inBuffer);

		if (info.empty()) {
			std::cout << "No configuration found" << std::endl;
			assert(0);
			return -21;
		}
		// Parse doc
		JsonDocument params;
		params.fromJson(info);

		OT_LOG_I("Application parameters were overwritten by configuration file: " + deplyomentPath);

		std::string actualServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SERVICE_URL);
		std::string actualSessionServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
		std::string actualLocalDirectoryServiceURL = ot::json::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);
		std::string actualSessionID = ot::json::getString(params, OT_ACTION_PARAM_SESSION_ID);
		// Initialize the service with the parameters from the file

		int startupResult = intern::ExternalServicesComponent::instance()->startup(_application, actualLocalDirectoryServiceURL, actualServiceURL);
		if (startupResult != 0) {
			return startupResult;
		}

		std::string initResult = intern::ExternalServicesComponent::instance()->initDebugExplicit(actualSessionServiceURL, actualSessionID);
		if (initResult != OT_ACTION_RETURN_VALUE_OK) {
			return -22;
		}
		else {
			return 0;
		}

	}
	catch (const std::exception& e) {
		OT_LOG_EAS(e.what());
		return -1;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error occured");
		return -2;
	}
}