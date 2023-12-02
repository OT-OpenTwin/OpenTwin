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
#include "OTCore/Logger.h"
#include "OTSystem/PortManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/Msg.h"

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
	//DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_KRIGING, OT_INFO_SERVICE_TYPE_KRIGING));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_FITTD, OT_INFO_SERVICE_TYPE_FITTD));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CartesianMeshService, OT_INFO_SERVICE_TYPE_CartesianMeshService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_GETDP, OT_INFO_SERVICE_TYPE_GETDP));
	//DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_BlockEditorService, OT_INFO_SERVICE_TYPE_BlockEditorService));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE));
	DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService));
	//DefaultSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
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
		else {
			_session->addRequestedService(s.serviceName(), s.serviceType());
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
		ot::JsonDocument gssShutdownDoc;
		gssShutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());
		gssShutdownDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(actualSession->id(), gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());
		
		std::string response;
		if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response)) {
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

	ot::JsonDocument statupParams;
	statupParams.AddMember(OT_ACTION_PARAM_SITE_ID, ot::JsonString("1", statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(_serviceURL, statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(url(), statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL, ot::JsonString("", statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_session->id(), statupParams.GetAllocator()), statupParams.GetAllocator());

	std::ofstream stream(path);
	stream << statupParams.toJson();
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

		ot::JsonDocument startupParams;
		startupParams.AddMember(OT_ACTION_PARAM_SITE_ID, ot::JsonString("1", startupParams.GetAllocator()), startupParams.GetAllocator());
		startupParams.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(_serviceURL, startupParams.GetAllocator()), startupParams.GetAllocator());
		startupParams.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(_websocketURL, startupParams.GetAllocator()), startupParams.GetAllocator());
		startupParams.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(url(), startupParams.GetAllocator()), startupParams.GetAllocator());
		startupParams.AddMember(OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL, ot::JsonString("", startupParams.GetAllocator()), startupParams.GetAllocator());

		std::ofstream stream(path);
		stream << startupParams.toJson();
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

std::string SessionService::handleGetDBURL(ot::JsonDocument& _commandDoc) {
	return m_dataBaseURL;
}

std::string SessionService::handleGetAuthURL(ot::JsonDocument& _commandDoc) {
	return serviceAuthorisationURL();
}

std::string SessionService::handleGetDBAndAuthURL(ot::JsonDocument& _commandDoc) {
#ifdef OT_USE_GSS
	// The info message is only present if the request is done by the UI frontend upon startup
	if (_commandDoc.HasMember(OT_ACTION_PARAM_MESSAGE)) {
		if (ot::json::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE) == OT_INFO_MESSAGE_LogIn) {
			return OT_ACTION_RETURN_INDICATOR_Error "Invalid global session service URL";
		}
	}
#endif // OT_USE_GSS
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_SERVICE_DBURL, ot::JsonString(m_dataBaseURL, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_AUTHURL, ot::JsonString(serviceAuthorisationURL(), doc.GetAllocator()), doc.GetAllocator());

	return doc.toJson();
}

std::string SessionService::handleGetMandatoryServices(ot::JsonDocument& _commandDoc) {
	ot::JsonDocument doc;
	doc.SetArray();
	for (auto session : m_mandatoryServicesMap) {
		ot::JsonObject sessionObj;
		sessionObj.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(session.first, doc.GetAllocator()), doc.GetAllocator());
		
		ot::JsonArray servicesArr;
		for (auto s : *session.second) {
			ot::JsonObject serviceObj;
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(s.serviceName(), doc.GetAllocator()), doc.GetAllocator());
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(s.serviceType(), doc.GetAllocator()), doc.GetAllocator());
			servicesArr.PushBack(serviceObj, doc.GetAllocator());
		}
		doc.PushBack(sessionObj, doc.GetAllocator());
	}
	return doc.toJson();
}

std::string SessionService::handleSessionDebugLogOn(ot::JsonDocument& _commandDoc) {
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	Session * theSession = getSession(sessionID);
	Service * theService = nullptr;
	try { theService = theSession->getService(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID)); }
	catch (...) {}

	ot::JsonDocument debugCmd;
	// todo: rework session debug commands
	//ot::json::add(debugCommand, OT_ACTION_MEMBER, OT_ACTION_CMD_LogOn);
	theSession->setIsDebug(true);
	//theSession->broadcast(theService, debugCommand, false);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleSessionDebugLogOff(ot::JsonDocument& _commandDoc) {
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	Session * theSession = getSession(sessionID);
	Service * theService = nullptr;
	try { theService = theSession->getService(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID)); }
	catch (...) {}

	theSession->setIsDebug(false);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleRegisterNewService(ot::JsonDocument& _commandDoc) {
	static bool deb = false;
	if (deb) {
		int stopHere = 0;
	}
	else {
		deb = true;
	}
	// Example: {...; 'Session.ID':'xxx'; 'Service.Name':'New services name'}

	// Required params
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string servicePort = ot::json::getString(_commandDoc, OT_ACTION_PARAM_PORT);
	// fixme: instead of providing the service port, the service URL should be passed to the sender
	std::string senderIP(ot::IpConverter::filterIpFromSender("127.0.0.1:xxxx", servicePort));

	// Optional params
	bool shouldRunRelayService = false;
	if (_commandDoc.HasMember(OT_ACTION_PARAM_START_RELAY)) {
		shouldRunRelayService = ot::json::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY); 
	}

	// Get the requested session
	m_masterLock.lock();
	Session * theSession = getSession(sessionID, false);

	if (theSession == nullptr) {
		m_masterLock.unlock();
		OT_LOG_E("A session with the id \"" + sessionID + "\" could not be found");
		throw ErrorException(("A session with the id \"" + sessionID + "\" could not be found").c_str());
	}

	// Create response document
	ot::JsonDocument response;

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

		response.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketIp, response.GetAllocator()), response.GetAllocator());
	}
	else {
		theService = theSession->registerService(senderIP, serviceName, serviceType, newID);
	}
	
	// Add service to health check (this is done when the service was started in debug mode)
	
	// If the notify connected parameter was provided a connected message will be send to all services in the session
	//theSession->broadcastMessage(theService, OT_ACTION_MESSAGE_ServiceConnected);
	m_masterLock.unlock();

	// Add response information
	response.AddMember(OT_ACTION_PARAM_SERVICE_ID, newID, response.GetAllocator());

	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		response.AddMember(OT_ACTION_PARAM_UI_ToolBarTabOrder, ot::JsonArray(theSession->toolBarTabOrder(), response.GetAllocator()), response.GetAllocator());
	}

	// Add service to run starter (will send run command later)
	ServiceRunStarter::instance().addService(theSession, theService);

	return response.toJson();
}

std::string SessionService::handleCreateNewSession(ot::JsonDocument& _commandDoc)
{
	OT_LOG_I("Creating new session..");

	// Example: {...; 'Session.ID':'The new sessions ID'; 'User.Name':'New user name'; 'Project.Name': 'The projects name'; 'Collection.Name':'The collections name'; 'Session.Type':'The session type'}

	if (!m_globalDirectoryService->isConnected()) return OT_ACTION_RETURN_INDICATOR_Error "No global directory service connected";

	// Required session params
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	std::string userName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_USER_NAME);
	std::string projectName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_PROJECT_NAME);
	std::string collectionName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string sessionType = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_TYPE);

	// User credentials
	std::string credentialsUserName = ot::json::getString(_commandDoc, OT_PARAM_AUTH_USERNAME);
	std::string credentialsUserPassword = ot::json::getString(_commandDoc, OT_PARAM_AUTH_PASSWORD);
	std::string userCollection = ot::json::getString(_commandDoc, OT_PARAM_SETTINGS_USERCOLLECTION);

	// Required service params
	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string servicePort = ot::json::getString(_commandDoc, OT_ACTION_PARAM_PORT);
	// fixme: critical: instead of providing the service port, the service URL should be passed to the sender
	std::string senderIP(ot::IpConverter::filterIpFromSender("127.0.0.1:xxxx", servicePort));

	// Optional params
	bool runMandatory = true;
	bool shouldRunRelayService = false;

	try { shouldRunRelayService = ot::json::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY); }
	catch (...) {}

	// Create the session
	m_masterLock.lock();
	Session * theSession = createSession(sessionID, userName, projectName, collectionName, sessionType);

	// Set the user credentials
	theSession->setCredentialsUsername(credentialsUserName);
	theSession->setCredentialsPassword(credentialsUserPassword);
	theSession->setUserCollection(userCollection);

	// Create response document
	ot::JsonDocument responseDoc;

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

		responseDoc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketURL, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}
	else {
		OT_LOG_D("Session created without relay service (for session creator)");
		theService = theSession->registerService(senderIP, serviceName, serviceType, newID);
	}

	m_masterLock.unlock();

	if (runMandatory) {
		OT_LOG_D("Running mandatory services");
		runMandatoryServices(theSession);
	}
	else {
		OT_LOG_D("Creator does NOT want the mandatory services to be started");
	}
	
	// Add response information
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, newID, responseDoc.GetAllocator());

	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		OT_LOG_D("Adding ToolBar Tab order to the response document");
		responseDoc.AddMember(OT_ACTION_PARAM_UI_ToolBarTabOrder, ot::JsonArray(theSession->toolBarTabOrder(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}

	return responseDoc.toJson();
}

std::string SessionService::handleCheckProjectOpen(ot::JsonDocument& _commandDoc)
{
	// Required session params
	std::string projectName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_PROJECT_NAME);

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

std::string SessionService::handleSendBroadcastMessage(ot::JsonDocument& _commandDoc) {
	// Get information
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	std::string message(ot::json::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE));

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

std::string SessionService::handleGetServicesInSession(ot::JsonDocument& _commandDoc) {
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));

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

std::string SessionService::handleGetSessionExists(ot::JsonDocument& _commandDoc) {
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	auto itm = m_sessionMap.find(sessionID);
	if (itm == m_sessionMap.end()) return OT_ACTION_RETURN_VALUE_FALSE;
	else return OT_ACTION_RETURN_VALUE_TRUE;
}

std::string SessionService::handleServiceClosing(ot::JsonDocument& _commandDoc) {
	m_masterLock.lock();
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	Session * theSession = getSession(sessionID);
	Service * actualService = theSession->getService(serviceID);
	serviceClosing(actualService, true, true);
	m_masterLock.unlock();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleMessage(ot::JsonDocument& _commandDoc) {
	std::string senderName(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME));
	OT_LOG_W("[UNEXPECTED] Message from \"" + senderName + "\": \"" +
		ot::json::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE) + "\"");
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleShutdownSession(ot::JsonDocument& _commandDoc)
{
	m_masterLock.lock();
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
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
	ot::JsonDocument gssShutdownDoc;
	gssShutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());
	gssShutdownDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(sessionID, gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response)) {
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

std::string SessionService::handleServiceFailure(ot::JsonDocument& _commandDoc) {
	OT_LOG_D("Service failure: Received.. Locking service...");
	
	m_masterLock.lock();

	// Get information of the service that failed
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));

	// Get service info (if it exists)

	Session * actualSession = getSession(sessionID);
	Service * actualService = nullptr;

	std::string serviceName(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME));
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
	ot::JsonDocument gssShutdownDoc;
	gssShutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());
	gssShutdownDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(actualSession->id(), gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response)) {
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

std::string SessionService::handleEnableServiceDebug(ot::JsonDocument& _commandDoc) {
	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
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

std::string SessionService::handleDisableServiceDebug(ot::JsonDocument& _commandDoc) {
	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	m_serviceDebugList.erase(serviceName);
	OT_LOG_D("Disabled the debug mode for the service \"" + serviceName + "\"");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleReset(ot::JsonDocument& _commandDoc) {
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

std::string SessionService::handleGetDebugInformation(ot::JsonDocument& _commandDoc) {
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

std::string SessionService::handleCheckStartupCompleted(ot::JsonDocument& _commandDoc) {
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
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

std::string SessionService::handleServiceShow(ot::JsonDocument& _commandDoc)
{
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	if (serviceID != 2) {
		int x = 2;
	}
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
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

	theSession->removeRequestedService(theService->name(), theService->type());

	theService->setVisible();

	ot::JsonDocument responseDoc;
	theSession->addServiceListToDocument(responseDoc);
	responseDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(theSession->userName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(theSession->projectName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	return responseDoc.toJson();
}

std::string SessionService::handleServiceHide(ot::JsonDocument& _commandDoc) {
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	Session * theSession = getSession(sessionID);
	Service * theService = theSession->getService(serviceID);
	theService->setHidden();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleAddMandatoryService(ot::JsonDocument& _commandDoc) {
	std::string sessionType(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_TYPE));
	std::string serviceName(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME));
	std::string serviceType(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE));

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

std::string SessionService::handleRegisterNewGlobalDirectoryService(ot::JsonDocument& _commandDoc) {
	std::string serviceURL(ot::json::getString(_commandDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL));
	m_globalDirectoryService->connect(serviceURL);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleServiceStartupFailed(ot::JsonDocument& _commandDoc) {
	// Get information of the service that failed
	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	
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

	ot::JsonDocument gssShutdownDoc;
	gssShutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());
	gssShutdownDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(actualSession->id(), gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send(url(), m_globalSessionService->serviceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response)) {
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
