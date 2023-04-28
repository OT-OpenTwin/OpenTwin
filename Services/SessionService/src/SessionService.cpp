/*
 * SessionService.cpp
 *
 *  Created on: November 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// SessionService header
#include "SessionService.h"		// Corresponding header
#include "GlobalSessionService.h"
#include "Session.h"
#include "Exception.h"
#include "Service.h"
#include "RelayService.h"
#include "globalDatatypes.h"
#include "ServiceRunStarter.h"

// Open Twin header
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinSystem/PortManager.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/IpConverter.h"
#include "OpenTwinCommunication/Msg.h"

// C++ header
#include <cassert>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <algorithm>

static SessionService *		g_instance{ nullptr };

void logMessage(const std::string &msg);

size_t writeFunction(void *, size_t, size_t, std::string*);

SessionService::SessionService() 
	: m_globalSessionService(nullptr), m_globalDirectoryService(nullptr), m_id(ot::invalidServiceID)
{
	m_globalDirectoryService = new GlobalDirectoryService(this);

	std::vector<ot::ServiceBase> * DefaultSessionServices = new std::vector<ot::ServiceBase>;
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PHREEC, OT_INFO_SERVICE_TYPE_PHREEC));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_KRIGING, OT_INFO_SERVICE_TYPE_KRIGING));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_FITTD, OT_INFO_SERVICE_TYPE_FITTD));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CartesianMeshService, OT_INFO_SERVICE_TYPE_CartesianMeshService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_GETDP, OT_INFO_SERVICE_TYPE_GETDP));
	m_mandatoryServicesMap.insert_or_assign("Default", DefaultSessionServices);
}

bool SessionService::isServiceInDebugMode(const std::string& _serviceName) {
	return (m_serviceDebugList.count(_serviceName) > 0);
}

SessionService * SessionService::instance(void) {
	if (g_instance == nullptr) {
		g_instance = new SessionService;
	}
	return g_instance;
}

// ######################################################################################

// Setter / Getter

void SessionService::setPort(const std::string& _port) {
	m_port = _port;
	
	ot::port_t portFrom = std::stoi(m_port);;
	portFrom++;
	ot::PortManager::instance().addPortRange(ot::PortRange(portFrom, portFrom + 79));
}

void SessionService::setGlobalDirectoryServiceURL(const std::string& _url) {
	m_globalDirectoryService->connect(_url);
}

// ######################################################################################

// Service management

Session * SessionService::createSession(
	const std::string &						_sessionID,
	const std::string &						_userName,
	const std::string &						_projectName,
	const std::string &						_collectionName,
	const std::string &						_sessionType
) {
	if (_sessionID.length() == 0) { throw ErrorException("Session ID may not be empty"); }

	auto itm = m_sessionMap.find(_sessionID);

	if (itm != m_sessionMap.end()) { return itm->second; }

	Session * newSession = new Session(_sessionID, _userName, _projectName, _collectionName, _sessionType);
	m_sessionMap.insert_or_assign(_sessionID, newSession);

	return newSession;
}

Session * SessionService::getSession(
	const std::string &						_sessionID,
	bool									_throwException
) {
	auto s = m_sessionMap.find(_sessionID);
	if (s == m_sessionMap.end()) {
		if (!_throwException) { return nullptr; }
		std::string ex("A session with the ID \"");
		ex.append(_sessionID);
		ex.append("\" could not be found");
		throw ErrorException(ex.c_str());
	}
	return s->second;
}

bool SessionService::runMandatoryServices(
	const std::string &						_sessionID
) {
	auto s = m_sessionMap.find(_sessionID);
	if (s == m_sessionMap.end()) {
		std::string ex("A session with the ID \"");
		ex.append(_sessionID);
		ex.append("\" could not be found");
		throw ErrorException(ex.c_str());
	}
	return runMandatoryServices(s->second);
}

bool SessionService::runMandatoryServices(
	Session *								_session
) {
	if (_session == nullptr) { throw std::exception("A nullptr was provided as session"); }
	auto it = m_mandatoryServicesMap.find(_session->type());
	if (it == m_mandatoryServicesMap.end()) {
		OT_LOG_E("No mandatory services registered for the session (Type = \"" + _session->type() + "\")");
		std::string ex("No mandatory services registered for the session type \"");
		ex.append(_session->type());
		ex.append("\" found");
		throw WarningException(ex.c_str());
	}

	// Collect all services to run in debug and release mode
	std::list<ot::ServiceBase> debugServices;
	std::list<ot::ServiceBase> releaseServices;

	for (auto s : *it->second) {
		if (isServiceInDebugMode(s.serviceName())) {
			debugServices.push_back(s);
		}
		else {
			releaseServices.push_back(s);
		}
	}

	// Request the user to start all the debug services
	for (auto s : debugServices) {
		std::string serviceUrl;
		if (!runServiceInDebug(s.serviceName(), s.serviceType(), _session, serviceUrl)) {
			OT_LOG_E("Failed to run service in debug mode (Name = \"" + s.serviceName() + "\"; Type = \"" + s.serviceType() + "\")");
			return false;
		}
	}

	// Send release mode services to the GDS
	if (!releaseServices.empty()) {
		for (auto s : releaseServices) {
			_session->addRequestedService(s.serviceName(), s.serviceType());
		}

		OT_LOG_D("Starting services via DirectoryService (Service.Count = \"" + std::to_string(releaseServices.size()) + "\")");
		if (!m_globalDirectoryService->requestToStartServices(releaseServices, _session->id())) {
			assert(0);
			// todo: clean up requested services
			return false;
		}
	}

	return true;
}

void SessionService::serviceClosing(
	Service *								_service,
	bool									_notifyOthers,
	bool									_autoCloseSessionIfMandatory
) {
	assert(_service != nullptr);
	// Get services session
	Session * actualSession = _service->getSession();

	// Remove the service from the session, after this call the pointer is invalid
	actualSession->removeService(_service->id(), _notifyOthers);

	// Check if the session is still alive
	if (actualSession->serviceCount() == 0 && _autoCloseSessionIfMandatory) {

#ifdef OT_USE_GSS
		// Notify GSS
		OT_rJSON_createDOC(gssShutdownDoc);
		ot::rJSON::add(gssShutdownDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_ShutdownSessionCompleted);
		ot::rJSON::add(gssShutdownDoc, OT_ACTION_PARAM_SESSION_ID, actualSession->id());

		std::string response;
		if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, ot::rJSON::toJSON(gssShutdownDoc), response)) {
			OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->serviceURL() + "\"");
		}
		if (response != OT_ACTION_RETURN_VALUE_OK) {
			OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->serviceURL() + "\"");
		}

#endif // OT_USE_GSS

		removeSession(actualSession);
		OT_LOG_D("The session with the ID \"" + actualSession->id() + "\" was closed automatically. Reason: No more services running");
	}
}

void SessionService::removeSession(
	Session *								_session
) {
	ServiceRunStarter::instance().sessionClosing(_session->id());
	m_sessionMap.erase(_session->id());
	delete _session;
}

std::list<const Session *> SessionService::sessions(void) {
	std::list<const Session *> ret;
	for (auto s : m_sessionMap) {
		ret.push_back(s.second);
	}
	return ret;
}

bool SessionService::runServiceInDebug(const std::string& _serviceName, const std::string& _serviceType, Session* _session, std::string& _serviceURL) {
	OT_LOG_D("Starting service in debug mode (Name = \"" + _serviceName + "\"; Type = \"" + _serviceType + "\")");

	char * buffer = nullptr;
	size_t bufferSize = 0;
	if (_dupenv_s(&buffer, &bufferSize, "OPENTWIN_DEV_ROOT") != 0 || buffer == nullptr) {
		OT_LOG_E("Please specify the environment variable \"OPENTWIN_DEV_ROOT\"");
		return false;
	}
	std::string path = buffer;
	delete[] buffer;
	buffer = nullptr;

	std::string serviceNameLower = _serviceName;
	std::transform(serviceNameLower.begin(), serviceNameLower.end(), serviceNameLower.begin(),
		[](unsigned char c) { return std::tolower(c); });

	path.append("\\Deployment\\" + serviceNameLower + ".cfg");

	_serviceURL = m_ip;
	ot::port_t portNumber = ot::PortManager::instance().determineAndBlockAvailablePort();
	_serviceURL.append(":").append(std::to_string(portNumber));

	std::list<std::string> arguments = { "1", _serviceURL, url(),  _session->id() };

	OT_rJSON_createDOC(statupParams);
	ot::rJSON::add(statupParams, OT_ACTION_PARAM_SITE_ID, "1");
	ot::rJSON::add(statupParams, OT_ACTION_PARAM_SERVICE_URL, _serviceURL);
	ot::rJSON::add(statupParams, OT_ACTION_PARAM_SESSION_SERVICE_URL, url());
	ot::rJSON::add(statupParams, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL, "");
	ot::rJSON::add(statupParams, OT_ACTION_PARAM_SESSION_ID, _session->id());

	std::ofstream stream(path);
	stream << ot::rJSON::toJSON(statupParams);
	stream.close();

	OT_LOG_D("Service params written to file (service.name = \"" + _serviceName + "\"; file = \"" + path + "\")");

	_session->addRequestedService(_serviceName, _serviceType);
	_session->addDebugPortInUse(portNumber);

	return true;
}

bool SessionService::runRelayService(Session * _session, std::string & _websocketURL, std::string& _serviceURL) {
	if (isServiceInDebugMode(OT_INFO_SERVICE_TYPE_RelayService)) {
		OT_LOG_D("Starting service \"" OT_INFO_SERVICE_TYPE_RelayService "\" in debug mode");
		char * buffer = nullptr;
		size_t bufferSize = 0;
		if (_dupenv_s(&buffer, &bufferSize, "OPENTWIN_DEV_ROOT") != 0 || buffer == nullptr) {
			OT_LOG_E("Please specify the environment variable \"OPENTWIN_DEV_ROOT\"");
			return false;
		}
		std::string path = buffer;
		delete[] buffer;
		buffer = nullptr;

		path.append("\\Deployment\\").append(OT_INFO_SERVICE_TYPE_RelayService).append(".cfg");

		_serviceURL = m_ip;
		assert(0); // determine new port (service started was removed!)
		//_serviceURL.append(":" + std::to_string(m_serviceStarter.getPortManager()->determineAndBlockAvailablePort()));

		_websocketURL = m_ip;
		assert(0); // determine new port (service started was removed!)
		//_websocketURL.append(":" + std::to_string(m_serviceStarter.getPortManager()->determineAndBlockAvailablePort()));

		OT_rJSON_createDOC(startupParams);
		ot::rJSON::add(startupParams, OT_ACTION_PARAM_SERVICE_URL, _serviceURL);
		ot::rJSON::add(startupParams, OT_ACTION_PARAM_WebsocketURL, _websocketURL);
		ot::rJSON::add(startupParams, OT_ACTION_PARAM_SESSION_SERVICE_URL, url());
		ot::rJSON::add(startupParams, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL, "");

		std::ofstream stream(path);
		stream << ot::rJSON::toJSON(startupParams);
		stream.close();

		std::cout << std::endl << "### Relay service params written to file: " << path << std::endl <<
			"### Please start the Relay service ###" << std::endl << std::endl;
	}
	else
	{
		OT_LOG_D("Starting service \"" OT_INFO_SERVICE_TYPE_RelayService "\" via DirectoryService");
		_serviceURL = "";
		if (!m_globalDirectoryService->requestToStartRelayService(_session->id(), _websocketURL, _serviceURL)) return false;
	}
	return true;

}

Service * SessionService::getServiceFromURL(const std::string& _url) {
	for (auto s : m_sessionMap) {
		try {
			return s.second->getServiceFromURL(_url);
		}
		catch (...) {}
	}
	return nullptr;
}

// #######################################################################################################################################################################################

// Message handling

std::string SessionService::handleGetDBURL(OT_rJSON_doc& _commandDoc) {
	return m_dataBaseURL;
}

std::string SessionService::handleGetAuthURL(OT_rJSON_doc& _commandDoc) {
	return serviceAuthorisationURL();
}

std::string SessionService::handleGetDBAndAuthURL(OT_rJSON_doc& _commandDoc) {
#ifdef OT_USE_GSS
	// The info message is only present if the request is done by the UI frontend upon startup
	OT_rJSON_ifMember(_commandDoc, OT_ACTION_PARAM_MESSAGE) {
		if (ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE) == OT_INFO_MESSAGE_LogIn) {
			return OT_ACTION_RETURN_INDICATOR_Error "Invalid global session service URL";
		}
	}
#endif // OT_USE_GSS
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_DBURL, m_dataBaseURL);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_AUTHURL, serviceAuthorisationURL());

	return OT_rJSON_toJSON(doc);
}

std::string SessionService::handleGetMandatoryServices(OT_rJSON_doc& _commandDoc) {
	OT_rJSON_doc doc;
	doc.SetArray();
	for (auto session : m_mandatoryServicesMap) {
		OT_rJSON_createValueObject(sessionObj);
		ot::rJSON::add(doc, sessionObj, OT_ACTION_PARAM_SESSION_TYPE, session.first);
		OT_rJSON_createValueArray(services);
		for (auto s : *session.second) {
			OT_rJSON_createValueObject(serviceObj);
			ot::rJSON::add(doc, serviceObj, OT_ACTION_PARAM_SERVICE_NAME, s.serviceName());
			ot::rJSON::add(doc, serviceObj, OT_ACTION_PARAM_SERVICE_TYPE, s.serviceType());
			services.PushBack(serviceObj, doc.GetAllocator());
		}
		doc.PushBack(sessionObj, doc.GetAllocator());
	}
	return ot::rJSON::toJSON(doc);
}

std::string SessionService::handleSessionDebugLogOn(OT_rJSON_doc& _commandDoc) {
	std::string sessionID = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	Session * theSession = getSession(sessionID);
	Service * theService = nullptr;
	try { theService = theSession->getService(ot::rJSON::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID)); }
	catch (...) {}

	OT_rJSON_createDOC(debugCommand);
	// todo: rework session debug commands
	//ot::rJSON::add(debugCommand, OT_ACTION_MEMBER, OT_ACTION_CMD_LogOn);
	theSession->setIsDebug(true);
	theSession->broadcast(theService, debugCommand, false);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleSessionDebugLogOff(OT_rJSON_doc& _commandDoc) {
	std::string sessionID = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	Session * theSession = getSession(sessionID);
	Service * theService = nullptr;
	try { theService = theSession->getService(ot::rJSON::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID)); }
	catch (...) {}

	theSession->setIsDebug(false);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleRegisterNewService(OT_rJSON_doc& _commandDoc) {
	static bool deb = false;
	if (deb) {
		int stopHere = 0;
	}
	else {
		deb = true;
	}
	// Example: {...; 'Session.ID':'xxx'; 'Service.Name':'New services name'}

	// Required params
	std::string sessionID = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	std::string serviceName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string servicePort = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_PORT);
	// fixme: critical: instead of providing the service port, the service URL should be passed to the sender
	std::string senderIP(ot::IpConverter::filterIpFromSender("127.0.0.1:xxxx", servicePort));

	// Optional params
	bool shouldRunRelayService = false;
	try { shouldRunRelayService = ot::rJSON::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY); }
	catch (...) {}

	// Get the requested session
	m_masterLock.lock();
	Session * theSession = getSession(sessionID, false);

	if (theSession == nullptr) {
		m_masterLock.unlock();
		OT_LOG_E("A session with the id \"" + sessionID + "\" could not be found");
		throw ErrorException(("A session with the id \"" + sessionID + "\" could not be found").c_str());
	}

	// Create response document
	OT_rJSON_createDOC(response);

	// Create a new service and store its information
	ot::serviceID_t newID(theSession->generateNextServiceId());

	Service * theService;
	if (shouldRunRelayService) {
		std::string relayServiceUrl;
		std::string websocketIp;
		if (!this->runRelayService(theSession, websocketIp, relayServiceUrl)) { throw std::exception("Failed to start Relay service"); }

		Service * theConnectedService = new Service(senderIP, serviceName, newID, serviceType, theSession, false);
		RelayService * rService = new RelayService(relayServiceUrl, serviceName, newID, serviceType, theSession, websocketIp, theConnectedService);
		theSession->registerService(rService);
		theService = rService;

		ot::rJSON::add(response, OT_ACTION_PARAM_WebsocketURL, websocketIp);
	}
	else {
		theService = theSession->registerService(senderIP, serviceName, serviceType, newID);
	}
	
	// Add service to health check (this is done when the service was started in debug mode)
	OT_rJSON_ifMember(_commandDoc, OT_ACTION_PARAM_PROCESS_ID) {
		// todo: add service to local health check (service was started in debug mode)
		assert(0);
		//m_serviceStarter.addServiceToHealthCheck(theService->url(), ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_PROCESS_ID), theSession->id());
	}

	// If the notify connected parameter was provided a connected message will be send to all services in the session
	//theSession->broadcastMessage(theService, OT_ACTION_MESSAGE_ServiceConnected);
	m_masterLock.unlock();

	// Add response information
	ot::rJSON::add(response, OT_ACTION_PARAM_SERVICE_ID, newID);

	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		ot::rJSON::add(response, OT_ACTION_PARAM_UI_ToolBarTabOrder, theSession->toolBarTabOrder());
	}

	// Add service to run starter (will send run command later)
	ServiceRunStarter::instance().addService(theSession, theService);

	return ot::rJSON::toJSON(response);
}

std::string SessionService::handleCreateNewSession(OT_rJSON_doc& _commandDoc)
{
	OT_LOG_I("Creating new session..");

	// Example: {...; 'Session.ID':'The new sessions ID'; 'User.Name':'New user name'; 'Project.Name': 'The projects name'; 'Collection.Name':'The collections name'; 'Session.Type':'The session type'}

	if (!m_globalDirectoryService->isConnected()) return OT_ACTION_RETURN_INDICATOR_Error "No global directory service connected";

	// Required session params
	std::string sessionID = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	std::string userName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_USER_NAME);
	std::string projectName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_PROJECT_NAME);
	std::string collectionName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string sessionType = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_TYPE);

	// User credentials
	std::string credentialsUserName = ot::rJSON::getString(_commandDoc, OT_PARAM_AUTH_USERNAME);
	std::string credentialsUserPassword = ot::rJSON::getString(_commandDoc, OT_PARAM_AUTH_PASSWORD);
	std::string userCollection = ot::rJSON::getString(_commandDoc, OT_PARAM_SETTINGS_USERCOLLECTION);

	// Required service params
	std::string serviceName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string servicePort = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_PORT);
	// fixme: critical: instead of providing the service port, the service URL should be passed to the sender
	std::string senderIP(ot::IpConverter::filterIpFromSender("127.0.0.1:xxxx", servicePort));

	// Optional params
	bool runMandatory = true;
	bool shouldRunRelayService = false;

	try { shouldRunRelayService = ot::rJSON::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY); }
	catch (...) {}

	// Create the session
	m_masterLock.lock();
	Session * theSession = createSession(sessionID, userName, projectName, collectionName, sessionType);

	// Set the user credentials
	theSession->setCredentialsUsername(credentialsUserName);
	theSession->setCredentialsPassword(credentialsUserPassword);
	theSession->setUserCollection(userCollection);

	// Create response document
	OT_rJSON_createDOC(responseDoc);

	// Create a new service and store its information
	ot::serviceID_t newID(theSession->generateNextServiceId());
	Service * theService;
	if (shouldRunRelayService) {
		OT_LOG_D("Relay service requested by session creator");

		std::string serviceURL;
		std::string websocketURL;
		if (!this->runRelayService(theSession, websocketURL, serviceURL)) {
			OT_LOG_E("Failed to start relay service for session (id = \"" + sessionID + "\")");
			delete theSession;
			throw std::exception("Failed to start relay service"); 
		}
		else {
			OT_LOG_D("Relay service started (Service.URL = \"" + serviceURL + "\"; Websocket.URL = \"" + websocketURL + "\")");
		}

		Service * theConnectedService = new Service(senderIP, serviceName, newID, serviceType, theSession, false);
		RelayService * rService = new RelayService(serviceURL, serviceName, newID, serviceType, theSession, websocketURL, theConnectedService);
		theSession->registerService(rService);
		theService = rService;

		ot::rJSON::add(responseDoc, OT_ACTION_PARAM_WebsocketURL, websocketURL);
	}
	else {
		OT_LOG_D("Session created without relay service (for session creator)");
		theService = theSession->registerService(senderIP, serviceName, serviceType, newID);
	}
	theService->setHidden();

	m_masterLock.unlock();

	if (runMandatory) {
		OT_LOG_D("Running mandatory services");
		runMandatoryServices(theSession);
	}
	else {
		OT_LOG_D("Creator does NOT want the mandatory services to be started");
	}

	// Add service to health check (this is done when the service was started in debug mode)
	OT_rJSON_ifMember(_commandDoc, OT_ACTION_PARAM_PROCESS_ID) {
		assert(0); // add service to local health check (service was started in debug mode)

		//m_serviceStarter.addServiceToHealthCheck(theService->url(), ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_PROCESS_ID), theSession->id());
	}
	
	// Add response information
	ot::rJSON::add(responseDoc, OT_ACTION_PARAM_SERVICE_ID, newID);

	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		OT_LOG_D("Adding ToolBar Tab order to the response document");
		ot::rJSON::add(responseDoc, OT_ACTION_PARAM_UI_ToolBarTabOrder, theSession->toolBarTabOrder());
	}

	return ot::rJSON::toJSON(responseDoc);
}

std::string SessionService::handleCheckProjectOpen(OT_rJSON_doc& _commandDoc)
{
	// Required session params
	std::string projectName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_PROJECT_NAME);

	// Check session
	for (auto session : m_sessionMap)
	{
		if (session.second->projectName() == projectName)
		{
			return session.second->userName();
		}
	}

	return "";
}

std::string SessionService::handleSendBroadcastMessage(OT_rJSON_doc& _commandDoc) {
	// Get information
	ot::serviceID_t serviceID(ot::rJSON::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	std::string message(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE));

	m_masterLock.lock();
	Session * theSession = getSession(sessionID, false);
	if (theSession) {
		Service * theService = theSession->getService(serviceID);
		if (theService == nullptr) {
			std::string errorMessage("A service with the ID \"");
			errorMessage.append(std::to_string(serviceID)).append("\" was not registered");
			m_masterLock.unlock();
			throw ErrorException(errorMessage.c_str());
		}
		theSession->broadcastMessage(theService, message);
	}
	m_masterLock.unlock();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleGetServicesInSession(OT_rJSON_doc& _commandDoc) {
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));

	m_masterLock.lock();
	auto s = m_sessionMap.find(sessionID);
	if (s == m_sessionMap.end()) {
		std::string errorMessage("A session with the ID \"");
		errorMessage.append(sessionID).append("\" does not exist");
		m_masterLock.unlock();
		throw ErrorException(errorMessage.c_str());
	}
	m_masterLock.unlock();
	return s->second->getServiceListJSON();
}

std::string SessionService::handleGetSessionExists(OT_rJSON_doc& _commandDoc) {
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	auto itm = m_sessionMap.find(sessionID);
	if (itm == m_sessionMap.end()) return OT_ACTION_RETURN_VALUE_FALSE;
	else return OT_ACTION_RETURN_VALUE_TRUE;
}

std::string SessionService::handleServiceClosing(OT_rJSON_doc& _commandDoc) {
	m_masterLock.lock();
	ot::serviceID_t serviceID(ot::rJSON::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	Session * theSession = getSession(sessionID);
	Service * actualService = theSession->getService(serviceID);
	serviceClosing(actualService, true, true);
	m_masterLock.unlock();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleMessage(OT_rJSON_doc& _commandDoc) {
	std::string senderName(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME));
	OT_LOG_W("[UNEXPECTED] Message from \"" + senderName + "\": \"" +
		ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE) + "\"");
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleShutdownSession(OT_rJSON_doc& _commandDoc)
{
	m_masterLock.lock();
	ot::serviceID_t serviceID(ot::rJSON::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	// Get service info
	Session * theSession = getSession(sessionID);
	Service * theService = theSession->getService(serviceID);
	std::string senderServiceName("<No service name>");
	if (theService) {
		senderServiceName = theService->name();
		theService->setReceiveBroadcastMessages(false);
	}
	// Notify GDS
	m_globalDirectoryService->notifySessionClosed(sessionID);

#ifdef OT_USE_GSS
	// Notify GSS
	OT_rJSON_createDOC(gssShutdownDoc);
	ot::rJSON::add(gssShutdownDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_ShutdownSessionCompleted);
	ot::rJSON::add(gssShutdownDoc, OT_ACTION_PARAM_SESSION_ID, sessionID);
	
	std::string response;
	if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, ot::rJSON::toJSON(gssShutdownDoc), response)) {
		OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->serviceURL() + "\"");
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->serviceURL() + "\"");
	}

#endif
	// Close service
	serviceClosing(theService, false, false);

	// Close session
	theSession->shutdown(nullptr);
	removeSession(theSession);

	// Show info log
	OT_LOG_D("The session with the ID \"" + sessionID + 
		"\" was closed. Reason: Shutdown requested by service \"" + senderServiceName + "\"");
	m_masterLock.unlock();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleServiceFailure(OT_rJSON_doc& _commandDoc) {
	OT_LOG_D("Service failure: Received.. Locking service...");
	
	m_masterLock.lock();

	// Get information of the service that failed
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));

	// Get service info (if it exists)

	Session * actualSession = getSession(sessionID);
	Service * actualService = nullptr;

	std::string serviceName(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME));
	try {
		auto services = actualSession->getServicesByName(serviceName);
		if (!services.empty()) actualService = services.front();
	}
	catch (ErrorException) {}

	OT_LOG_D("Service failure: Cleaning up session (ID = \"" + sessionID + ")");

	// Clean up the session
	actualSession->serviceFailure(actualService);

#ifdef OT_USE_GSS
	OT_LOG_D("Service failure: Notify GSS..");

	// Notify GSS
	OT_rJSON_createDOC(gssShutdownDoc);
	ot::rJSON::add(gssShutdownDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_ShutdownSessionCompleted);
	ot::rJSON::add(gssShutdownDoc, OT_ACTION_PARAM_SESSION_ID, actualSession->id());

	std::string response;
	if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, ot::rJSON::toJSON(gssShutdownDoc), response)) {
		OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->serviceURL() + "\"");
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->serviceURL() + "\"");
	}
#endif // OT_USE_GSS

	// Display log information
	OT_LOG_I("The session with the ID \"" + actualSession->id() + "\" was closed automatically. Reason: Session emergency shutdown");

	// Finally delete the session
	removeSession(actualSession);

	m_masterLock.unlock();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleEnableServiceDebug(OT_rJSON_doc& _commandDoc) {
	std::string serviceName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	// Check if the provided services is contained in the mandatory services list
	bool found = false;
	for (auto sessionType : m_mandatoryServicesMap) {
		for (auto serviceType : *sessionType.second) {
			if (serviceType.serviceName() == serviceName) {
				found = true;
				break;
			}
		}
		if (found) break;
	}
	if (!found) return OT_ACTION_RETURN_INDICATOR_Error "The specified service is not contained in the mandatory services list";

	m_serviceDebugList.insert_or_assign(serviceName, true);
	OT_LOG_D("Enabled the debug mode for the service \"" + serviceName  + "\"");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleDisableServiceDebug(OT_rJSON_doc& _commandDoc) {
	std::string serviceName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	m_serviceDebugList.erase(serviceName);
	OT_LOG_D("Disabled the debug mode for the service \"" + serviceName + "\"");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleReset(OT_rJSON_doc& _commandDoc) {
	m_masterLock.lock();
	for (auto s : m_sessionMap) {
		Session * actualSession = s.second;
		delete actualSession;
	}
	m_sessionMap.clear();

	// todo: check if actually all clear operations were successful

	OT_LOG_I("[RESET] The session service was successfully set back to initial state.");
	OT_LOG_I("[RESET] All session and service informations were removed from the memory");
	m_masterLock.unlock();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleGetDebugInformation(OT_rJSON_doc& _commandDoc) {
	// todo: switch to json export (all classes should be derived from Serializable before)
	OT_LOG_D("***** Debug information [Start] *****");
	if (m_sessionMap.size() == 0) {
		OT_LOG_D("Current sessions: None");
	}
	else {
		OT_LOG_D("Current sessions:");
		for (auto itm : m_sessionMap) {
			OT_LOG_D("\tsession " + itm.second->infoToJSON());
			OT_LOG_D("\t\twith " + itm.second->getServiceListJSON());
		}
	}
	OT_LOG_D("***** Debug information [End] *****");
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleCheckStartupCompleted(OT_rJSON_doc& _commandDoc) {
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	Session * theSession = getSession(sessionID);
	if (theSession) {
		// Check whether the initialization has been completed succesfully
		if (theSession->requestedServices().empty()) return OT_ACTION_RETURN_VALUE_TRUE;
		else return OT_ACTION_RETURN_VALUE_FALSE;
	}
	else {
		return OT_ACTION_RETURN_INDICATOR_Error "Invalid Session ID";
	}
}

std::string SessionService::handleServiceShow(OT_rJSON_doc& _commandDoc)
{
	ot::serviceID_t serviceID(ot::rJSON::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	if (serviceID != 2) {
		int x = 2;
	}
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	Session * theSession = getSession(sessionID);
	if (theSession == nullptr) {
		OT_LOG_E("A session with the id \"" + sessionID + "\" was not found");
		return OT_ACTION_RETURN_INDICATOR_Error "Invalid session id";
	}
	
	Service * theService = theSession->getService(serviceID);
	if (theService == nullptr) {
		OT_LOG_E("A service with the id \"" + std::to_string(serviceID) + "\" was not found in the session with the id \"" + sessionID + "\"");
		return OT_ACTION_RETURN_INDICATOR_Error "Invalid service id";
	}

	theService->setVisible();
	OT_rJSON_createDOC(responseDoc);
	theSession->addServiceListToDocument(responseDoc);
	ot::rJSON::add(responseDoc, OT_ACTION_PARAM_USER_NAME, theSession->userName());
	ot::rJSON::add(responseDoc, OT_ACTION_PARAM_PROJECT_NAME, theSession->projectName());
	return ot::rJSON::toJSON(responseDoc);
}

std::string SessionService::handleServiceHide(OT_rJSON_doc& _commandDoc) {
	ot::serviceID_t serviceID(ot::rJSON::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	Session * theSession = getSession(sessionID);
	Service * theService = theSession->getService(serviceID);
	theService->setHidden();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleAddMandatoryService(OT_rJSON_doc& _commandDoc) {
	std::string sessionType(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_TYPE));
	std::string serviceName(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME));
	std::string serviceType(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE));

	ot::ServiceBase newService(serviceName, serviceType);

	// Locate current data
	auto it = m_mandatoryServicesMap.find(sessionType);
	std::vector<ot::ServiceBase> * data;
	if (it == m_mandatoryServicesMap.end()) {
		data = new std::vector<ot::ServiceBase>;
		m_mandatoryServicesMap.insert_or_assign(sessionType, data);
	}
	else {
		data = it->second;
	}
	// Check for duplicates
	bool found = false;
	for (auto e : *data) {
		if (e == newService) { found = true; break; }
	}
	if (!found) data->push_back(newService);
	OT_LOG_D("Mandatory service for prject type \"" + sessionType + "\": Added service \"" + serviceName + "\"");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleRegisterNewGlobalDirectoryService(OT_rJSON_doc& _commandDoc) {
	std::string serviceURL(ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL));
	m_globalDirectoryService->connect(serviceURL);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleServiceStartupFailed(OT_rJSON_doc& _commandDoc) {
	// Get information of the service that failed
	std::string serviceName = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string sessionID = ot::rJSON::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	
	m_masterLock.lock();

	// Get service info (if it exists)
	Session * actualSession = getSession(sessionID);
	auto services = actualSession->getServicesByName(serviceName);
	Service * actualService = nullptr;
	for (auto s : services) {
		if (!s->isVisible()) {
			actualService = s;
			break;
		}
	}

	assert(actualService);

	// Clean up the session
	actualSession->serviceFailure(actualService);

#ifdef OT_USE_GSS
	// Notify GSS
	OT_rJSON_createDOC(gssShutdownDoc);
	ot::rJSON::add(gssShutdownDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_ShutdownSessionCompleted);
	ot::rJSON::add(gssShutdownDoc, OT_ACTION_PARAM_SESSION_ID, actualSession->id());

	std::string response;
	if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, ot::rJSON::toJSON(gssShutdownDoc), response)) {
		OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->serviceURL() + "\"");
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->serviceURL() + "\"");
	}
#endif // OT_USE_GSS

	// Display log information
	OT_LOG_W("The session with the ID \"" + actualSession->id() + 
		"\" was closed automatically. Reason: Session emergency shutdown after service startup failed");

	// Finally delete the session
	removeSession(actualSession);

	m_masterLock.unlock();

	return OT_ACTION_RETURN_VALUE_OK;
}
