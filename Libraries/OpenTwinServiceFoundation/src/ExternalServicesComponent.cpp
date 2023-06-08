/*
 *  ExternalServicesComponent.cpp
 *
 *  Created on: 01/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

// OpenTwin header
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/rJSON.h"	            						// rapidjson wrapper
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/Owner.h"

#include "OpenTwinCommunication/ActionTypes.h"						// action member and types definition
#include "OpenTwinCommunication/IpConverter.h"						// extract IP information from sender information
#include "OpenTwinCommunication/Msg.h"								// message sending
#include "OpenTwinCommunication/ServiceLogNotifier.h"				// logging

#include "OpenTwinFoundation/ApplicationBase.h"
#include "OpenTwinFoundation/Dispatcher.h"
#include "OpenTwinFoundation/SettingsData.h"
#include "OpenTwinFoundation/UiPluginComponent.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ExternalServicesComponent.h"	// Corresponding header

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
			OT_rJSON_createDOC(pingDoc);
			ot::rJSON::add(pingDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
			std::string ping = ot::rJSON::toJSON(pingDoc);

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
	OT_LOG_D("Starting foundation startup");

	if (m_componentState != WaitForStartup) {
		otAssert(0, "Component startup already performed");
		return -30;
	}

	m_application = _application;
	if (m_application == nullptr) {
		otAssert(0, "nullptr provided");
		return -1;
	}

	m_application->setServiceURL(_ownURL);
	m_application->setDirectoryServiceURL(_localDirectoryServiceURL);
	m_application->setSiteID("1");

	m_componentState = WaitForInit;

	OT_LOG_D("Foundation startup completed (ServiceURL = \"" + m_application->serviceURL() + "\"; LDS = \"" + m_application->directoryServiceURL() + "\")");

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
		otAssert(0, "Component already initialized");
		return OT_ACTION_RETURN_INDICATOR_Error "Component already initialized";
	}

	OT_LOG_D("Starting Foundation initialization");

	// Store information
	m_application->setSessionServiceURL(_sessionServiceURL);
	m_application->setSessionID(_sessionID);
	
	// Get the database information
	{
		OT_rJSON_createDOC(request);

		// todo: maybe merge get database url and register new service into one call? (nothin else is happening before anyway
		ot::rJSON::add(request, OT_ACTION_MEMBER, OT_ACTION_CMD_GetDatabaseUrl);

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(request), response)) {
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
		OT_rJSON_createDOC(newServiceCommandDoc);
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_RegisterNewService);
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_SESSION_ID, m_application->sessionID());
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_SERVICE_NAME, m_application->serviceName());
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_PORT, ot::IpConverter::portFromIp(m_application->serviceURL()));
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_SERVICE_TYPE, m_application->serviceType());
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_START_RELAY, m_application->startAsRelayService());

#ifdef _DEBUG
		auto handle = GetCurrentProcess();
		if (handle != nullptr) {
			unsigned long handleID = GetProcessId(handle);
			assert(handleID != 0); // Failed to get process handle ID
			ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_PROCESS_ID, std::to_string(handleID));
		}
		else {
			assert(0); // Failed to get current process handle
		}
#endif // _DEBUG

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(newServiceCommandDoc), response)) {
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

		OT_rJSON_parseDOC(reply, response.c_str());
		OT_rJSON_docCheck(reply);
		m_application->setServiceID(ot::rJSON::getInt(reply, OT_ACTION_PARAM_SERVICE_ID));
		ot::GlobalOwner::instance().setId(m_application->serviceID());

		OT_LOG_D("Service ID set to: \"" + std::to_string(m_application->serviceID()) + "\"");

		if (m_application->startAsRelayService()) {
			m_application->setWebSocketURL(ot::rJSON::getString(reply, OT_ACTION_PARAM_WebsocketURL));

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
		otAssert(0, "Component already initialized");
		return OT_ACTION_RETURN_INDICATOR_Error "Component already initialized";
	}

	OT_LOG_D("Starting Foundation initialization");

	// Store information
	m_application->setSessionServiceURL(_sessionServiceURL);
	m_application->setSessionID(_sessionID);

	// Get the database information
	{
		OT_rJSON_createDOC(request);

		// todo: maybe merge get database url and register new service into one call? (nothin else is happening before anyway
		ot::rJSON::add(request, OT_ACTION_MEMBER, OT_ACTION_CMD_GetDatabaseUrl);

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(request), response)) {
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
		OT_rJSON_createDOC(newServiceCommandDoc);
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_RegisterNewService);
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_SESSION_ID, m_application->sessionID());
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_SERVICE_NAME, m_application->serviceName());
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_PORT, ot::IpConverter::portFromIp(m_application->serviceURL()));
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_SERVICE_TYPE, m_application->serviceType());
		ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_START_RELAY, m_application->startAsRelayService());


		auto handle = GetCurrentProcess();
		if (handle != nullptr) {
			unsigned long handleID = GetProcessId(handle);
			assert(handleID != 0); // Failed to get process handle ID
			ot::rJSON::add(newServiceCommandDoc, OT_ACTION_PARAM_PROCESS_ID, std::to_string(handleID));
		}
		else {
			assert(0); // Failed to get current process handle
		}


		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(newServiceCommandDoc), response)) {
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

		OT_rJSON_parseDOC(reply, response.c_str());
		OT_rJSON_docCheck(reply);
		m_application->setServiceID(ot::rJSON::getInt(reply, OT_ACTION_PARAM_SERVICE_ID));
		ot::GlobalOwner::instance().setId(m_application->serviceID());

		OT_LOG_D("Service ID set to: \"" + std::to_string(m_application->serviceID()) + "\"");

		if (m_application->startAsRelayService()) {
			m_application->setWebSocketURL(ot::rJSON::getString(reply, OT_ACTION_PARAM_WebsocketURL));

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
		otAssert(0, "Not initialized yet");
		return std::string();
	}
	try {
		// Use a mutex to ensure the requests will be handled one by one
		OT_rJSON_parseDOC(actionDoc, _json.c_str());

		// Get the requested action (if the member is missing a exception will be thrown)
		std::string action = ot::rJSON::getString(actionDoc, OT_ACTION_MEMBER);
		
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
		OT_rJSON_createDOC(commandDoc);
		ot::rJSON::add(commandDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceClosing);
		ot::rJSON::add(commandDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
		ot::rJSON::add(commandDoc, OT_ACTION_PARAM_SESSION_ID, m_application->sessionID());
		std::string response;

		OT_LOG_D("Sending shutdown notification to LSS");

		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(commandDoc), response)) {
			assert(0); // Failed to send http request
		}
	}

	std::thread t(ot::intern::exitWorker, 0);
	t.detach();
}

// #####################################################################################################################################

// Private functions

std::string ot::intern::ExternalServicesComponent::handleInitialize(OT_rJSON_doc& _document) {
	std::string serviceName = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string sessionServiceURL = ot::rJSON::getString(_document, OT_ACTION_PARAM_SESSION_SERVICE_URL);
	std::string sessionID = ot::rJSON::getString(_document, OT_ACTION_PARAM_SESSION_ID);

	return this->init(sessionServiceURL, sessionID);
}

std::string ot::intern::ExternalServicesComponent::handleServiceConnected(OT_rJSON_doc& _document) {
	serviceID_t senderID = ot::rJSON::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	m_application->__serviceConnected(senderName, senderType, senderURL, senderID);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleServiceDisconnected(OT_rJSON_doc& _document) {
	serviceID_t senderID = ot::rJSON::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	m_application->__serviceDisconnected(senderName, senderType, senderURL, senderID);
	
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleShutdownRequestByService(OT_rJSON_doc& _document) {
	shutdown(false);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleMessage(OT_rJSON_doc& _document) {
	std::string message = ot::rJSON::getString(_document, OT_ACTION_PARAM_MESSAGE);
	serviceID_t senderID = ot::rJSON::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

#ifdef _DEBUG
	std::string msg("Message from \"");
	msg.append(std::to_string(senderID)).append("\": ").append(message);
	std::cout << msg << std::endl;
	msg.append("\n");
#endif // _DEBUG

	m_application->__processMessage(message, _document, senderID);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleServiceShutdown(OT_rJSON_doc& _document) {
	shutdown(true);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleRun(OT_rJSON_doc& _document) {
	std::string credentialsUsername = ot::rJSON::getString(_document, OT_PARAM_AUTH_USERNAME);
	std::string credentialsPassword = ot::rJSON::getString(_document, OT_PARAM_AUTH_PASSWORD);
	m_application->m_DBuserCollection = ot::rJSON::getString(_document, OT_PARAM_SETTINGS_USERCOLLECTION);

	DataBase::GetDataBase()->setUserCredentials(credentialsUsername, credentialsPassword);

	// Change the service to visible
	{
		OT_rJSON_createDOC(visibilityCommand);
		ot::rJSON::add(visibilityCommand, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceShow);
		ot::rJSON::add(visibilityCommand, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
		ot::rJSON::add(visibilityCommand, OT_ACTION_PARAM_SESSION_ID, m_application->sessionID());

		std::string response;
		if (!ot::msg::send(m_application->serviceURL(), m_application->sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(visibilityCommand), response)) {
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

		OT_rJSON_parseDOC(responseDoc, response.c_str());
		OT_rJSON_docCheck(responseDoc);

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

		auto serviceList = ot::rJSON::getObjectList(responseDoc, OT_ACTION_PARAM_SESSION_SERVICES);
		for (auto serviceJSON : serviceList) {
			OT_rJSON_parseDOC(serviceDoc, serviceJSON.c_str());
			OT_rJSON_docCheck(serviceDoc);
			std::string senderURL = ot::rJSON::getString(serviceDoc, OT_ACTION_PARAM_SERVICE_URL);
			std::string senderName = ot::rJSON::getString(serviceDoc, OT_ACTION_PARAM_SERVICE_NAME);
			std::string senderType = ot::rJSON::getString(serviceDoc, OT_ACTION_PARAM_SERVICE_TYPE);
			serviceID_t senderID = ot::rJSON::getUInt(serviceDoc, OT_ACTION_PARAM_SERVICE_ID);
			if (senderID != m_application->serviceID()) {
				m_application->__serviceConnected(senderName, senderType, senderURL, senderID);
			}
		}
	}

	// Refresh the current service list and start application
	m_application->run();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handlePreShutdown(OT_rJSON_doc& _document) {
	m_application->preShutdown();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleEmergencyShutdown(OT_rJSON_doc& _document) {
	std::thread t(ot::intern::exitWorker, -100);
	t.detach();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::intern::ExternalServicesComponent::handleUIPluginConnected(OT_rJSON_doc& _document) {
	components::UiComponent * ui = m_application->uiComponent();
	unsigned long long pluginUID = ot::rJSON::getULongLong(_document, OT_ACTION_PARAM_UI_PLUGIN_UID);
	std::string pluginName = ot::rJSON::getString(_document, OT_ACTION_PARAM_UI_PLUGIN_NAME);
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
			otAssert(0, "Component not initialized");
			OT_LOG_E("Component not initialized");

			response = OT_ACTION_RETURN_INDICATOR_Error "Component not initialized";
		}
	}
	catch (const std::exception& _e) {
		otAssert(0, "Error occured");
		OT_LOG_E(_e.what());

		response = OT_ACTION_RETURN_INDICATOR_Error;
		response.append(_e.what());
	}
	catch (...) {
		otAssert(0, "Unknown Error occured");
		OT_LOG_E("[FATAL] Unknown error occured");

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
		OT_rJSON_parseDOC(params, info.c_str());
		assert(params.IsObject());

		OT_LOG_I("Application parameters were overwritten by configuration file: " + deplyomentPath);

		std::string actualServiceURL = ot::rJSON::getString(params, OT_ACTION_PARAM_SERVICE_URL);
		std::string actualSessionServiceURL = ot::rJSON::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
		std::string actualLocalDirectoryServiceURL = ot::rJSON::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);
		std::string actualSessionID = ot::rJSON::getString(params, OT_ACTION_PARAM_SESSION_ID);
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
		OT_rJSON_parseDOC(params, info.c_str());
		assert(params.IsObject());

		OT_LOG_I("Application parameters were overwritten by configuration file: " + deplyomentPath);

		std::string actualServiceURL = ot::rJSON::getString(params, OT_ACTION_PARAM_SERVICE_URL);
		std::string actualSessionServiceURL = ot::rJSON::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
		std::string actualLocalDirectoryServiceURL = ot::rJSON::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);
		std::string actualSessionID = ot::rJSON::getString(params, OT_ACTION_PARAM_SESSION_ID);
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
		std::cout << "[INIT] ERROR: " << e.what() << std::endl;
		return -1;
	}
	catch (...) {
		std::cout << "[INIT] ERROR: Unknown error" << std::endl;
		return -2;
	}
}