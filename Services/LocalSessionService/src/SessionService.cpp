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

size_t writeFunction(void *, size_t, size_t, std::string*);

SessionService::SessionService() 
	: m_globalSessionService(nullptr), m_globalDirectoryService(nullptr), m_id(ot::invalidServiceID)
{
	m_globalDirectoryService = new GlobalDirectoryService(this);

	//
	// Development services list (this contains all services together (potentially even including experimental ones) for testing purposes)
	//
	std::vector<ot::ServiceBase> *DevelopmentSessionServices = new std::vector<ot::ServiceBase>;
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PHREEC, OT_INFO_SERVICE_TYPE_PHREEC));
	//DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_KRIGING, OT_INFO_SERVICE_TYPE_KRIGING));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_FITTD, OT_INFO_SERVICE_TYPE_FITTD));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CartesianMeshService, OT_INFO_SERVICE_TYPE_CartesianMeshService));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_GETDP, OT_INFO_SERVICE_TYPE_GETDP));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ELMERFEM, OT_INFO_SERVICE_TYPE_ELMERFEM));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_DebugService, OT_INFO_SERVICE_TYPE_DebugService));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService));
	DevelopmentSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT, DevelopmentSessionServices);

	//
	// 3D Simulation services list (this contains all services relevant for 3D simulation)
	//
	std::vector<ot::ServiceBase> *Simulation3DSessionServices = new std::vector<ot::ServiceBase>;
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PHREEC, OT_INFO_SERVICE_TYPE_PHREEC));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_FITTD, OT_INFO_SERVICE_TYPE_FITTD));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CartesianMeshService, OT_INFO_SERVICE_TYPE_CartesianMeshService));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_GETDP, OT_INFO_SERVICE_TYPE_GETDP));
	Simulation3DSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ELMERFEM, OT_INFO_SERVICE_TYPE_ELMERFEM));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_3DSIM, Simulation3DSessionServices);

	//
	// Data Pipeline services list (this contains all services relevant for pipeline based data processing)
	//
	std::vector<ot::ServiceBase> *DataPipelineSessionServices = new std::vector<ot::ServiceBase>;
	DataPipelineSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	DataPipelineSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	DataPipelineSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE));
	DataPipelineSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_DATAPIPELINE, DataPipelineSessionServices);

	//
	// StudioSuite services list (this contains all services relevant for Studio Suite integration)
	//
	std::vector<ot::ServiceBase>* StudioSuiteSessionServices = new std::vector<ot::ServiceBase>;
	StudioSuiteSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	StudioSuiteSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_STUDIOSUITE, OT_INFO_SERVICE_TYPE_STUDIOSUITE));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE, StudioSuiteSessionServices);

	//
	// LTSpice services list (this contains all services relevant for LTSpice integration)
	//
	std::vector<ot::ServiceBase>* LTSpiceSessionServices = new std::vector<ot::ServiceBase>;
	LTSpiceSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	LTSpiceSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_LTSPICE, OT_INFO_SERVICE_TYPE_LTSPICE));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_LTSPICE, LTSpiceSessionServices);

	//
	// Pyrit services list (this contains all services relevant for Pyrit integration)
	//
	std::vector<ot::ServiceBase>* PyritSessionServices = new std::vector<ot::ServiceBase>;
	PyritSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	PyritSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	PyritSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	PyritSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	PyritSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PYRIT, OT_INFO_SERVICE_TYPE_PYRIT));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_PYRIT, PyritSessionServices);

	//
	// Circuit Simulation services list (this contains all services relevant for Pyrit integration)
	//
	std::vector<ot::ServiceBase>* CircuitSimulationSessionServices = new std::vector<ot::ServiceBase>;
	CircuitSimulationSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	CircuitSimulationSessionServices->push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_CIRCUITSIMULATION, CircuitSimulationSessionServices);
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

Session * SessionService::getSession(const std::string& _sessionID) {
	auto s = m_sessionMap.find(_sessionID);
	if (s == m_sessionMap.end()) {
		OT_LOG_E("Session not found \"" + _sessionID + "\"");
		return nullptr;
	}
	else {
		return s->second;
	}
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
		if (isServiceInDebugMode(s.getServiceName())) {
			debugServices.push_back(s);
		}
		else {
			releaseServices.push_back(s);
		}
	}

	// Request the user to start all the debug services
	for (auto s : debugServices) {
		std::string serviceUrl;
		if (!runServiceInDebug(s.getServiceName(), s.getServiceType(), _session, serviceUrl)) {
			OT_LOG_E("Failed to run service in debug mode (Name = \"" + s.getServiceName() + "\"; Type = \"" + s.getServiceType() + "\")");
			return false;
		}
	}

	// Send release mode services to the GDS
	if (!releaseServices.empty()) {
		for (auto s : releaseServices) {
			_session->addRequestedService(s.getServiceName(), s.getServiceType());
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
	if (_service == nullptr) {
		OT_LOG_EA("Service is nullptr");
		return;
	}
	// Get services session
	Session * actualSession = _service->getSession();
	if (actualSession == nullptr) {
		OT_LOG_EA("No session set");
		return;
	}

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
		if (!ot::msg::send(url(), m_globalSessionService->getServiceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
		}
		if (response != OT_ACTION_RETURN_VALUE_OK) {
			OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
		}

#endif // OT_USE_GSS

		removeSession(actualSession);
		OT_LOG_D("The session with the ID \"" + actualSession->id() + "\" was closed automatically. Reason: No more services running");
	}
}

void SessionService::removeSession(Session* _session) {
	this->forgetSession(_session);
	delete _session;
}

void SessionService::forgetSession(Session* _session) {
	ServiceRunStarter::instance().sessionClosing(_session->id());
	m_sessionMap.erase(_session->id());
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
		if (!m_globalDirectoryService->requestToStartRelayService(_session->id(), _websocketURL, _serviceURL)) {
			return false;
		}
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

std::string SessionService::handleGetGlobalServicesURL(ot::JsonDocument& _commandDoc) {
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
	doc.AddMember(OT_ACTION_PARAM_SERVICE_GDSURL, ot::JsonString(m_globalDirectoryService->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

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
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(s.getServiceName(), doc.GetAllocator()), doc.GetAllocator());
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(s.getServiceType(), doc.GetAllocator()), doc.GetAllocator());
			servicesArr.PushBack(serviceObj, doc.GetAllocator());
		}
		doc.PushBack(sessionObj, doc.GetAllocator());
	}
	return doc.toJson();
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
	std::string hostAddress = ot::json::getString(_commandDoc, OT_ACTION_PARAM_HOST);
	// fixme: instead of providing the service port, the service URL should be passed to the sender
	std::string senderIP(ot::IpConverter::filterIpFromSender(hostAddress + ":xxxx", servicePort));

	// Optional params
	bool shouldRunRelayService = false;
	if (_commandDoc.HasMember(OT_ACTION_PARAM_START_RELAY)) {
		shouldRunRelayService = ot::json::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY); 
	}

	// Get the requested session
	Session * theSession = getSession(sessionID);

	if (theSession == nullptr) {
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
	
	// Add response information
	response.AddMember(OT_ACTION_PARAM_SERVICE_ID, newID, response.GetAllocator());

	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		response.AddMember(OT_ACTION_PARAM_UI_ToolBarTabOrder, ot::JsonArray(theSession->toolBarTabOrder(), response.GetAllocator()), response.GetAllocator());
	}
	if (m_logModeManager.getGlobalLogFlagsSet()) {
		ot::JsonArray logArr;
		ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), logArr, response.GetAllocator());
		response.AddMember(OT_ACTION_PARAM_LogFlags, logArr, response.GetAllocator());
	}

	OTAssertNullptr(theService);

	// Add service to run starter (will send run command later)
	ServiceRunStarter::instance().addService(theSession, theService);

	return response.toJson();
}

std::string SessionService::handleGetSystemInformation(ot::JsonDocument& _doc) {

	double globalCpuLoad = 0, globalMemoryLoad = 0;
	m_systemLoadInformation.getGlobalCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad);

	double processCpuLoad = 0, processMemoryLoad = 0;
	m_systemLoadInformation.getCurrentProcessCPUAndMemoryLoad(processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, "Local Session Service", reply.GetAllocator());

	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	// Now we add information about the session services
	ot::JsonArray sessionInfo;

	for (auto session : sessions()) {
		ot::JsonValue info;
		info.SetObject();

		info.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(session->id(), reply.GetAllocator()), reply.GetAllocator());
		info.AddMember(OT_ACTION_PARAM_SESSION_PROJECT, ot::JsonString(session->projectName(), reply.GetAllocator()), reply.GetAllocator());
		info.AddMember(OT_ACTION_PARAM_SESSION_USER, ot::JsonString(session->userName(), reply.GetAllocator()), reply.GetAllocator());
		info.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(session->type(), reply.GetAllocator()), reply.GetAllocator());

		ot::JsonArray servicesInformation;
		session->servicesInformation(servicesInformation, reply.GetAllocator());

		info.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, servicesInformation, reply.GetAllocator());

		sessionInfo.PushBack(info, reply.GetAllocator());
	}

	reply.AddMember(OT_ACTION_PARAM_SESSION_LIST, sessionInfo, reply.GetAllocator());

	return reply.toJson();
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
	std::string databaseUserName = ot::json::getString(_commandDoc, OT_PARAM_DB_USERNAME);
	std::string databaseUserPassword = ot::json::getString(_commandDoc, OT_PARAM_DB_PASSWORD);

	// Required service params
	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string serviceType = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE);
	std::string servicePort = ot::json::getString(_commandDoc, OT_ACTION_PARAM_PORT);
	std::string hostAddress = ot::json::getString(_commandDoc, OT_ACTION_PARAM_HOST);
	// fixme: critical: instead of providing the service port, the service URL should be passed to the sender
	std::string senderIP(ot::IpConverter::filterIpFromSender(hostAddress + ":xxxx", servicePort));

	// Optional params
	bool runMandatory = true;
	bool shouldRunRelayService = false;

	try { shouldRunRelayService = ot::json::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY); }
	catch (...) {}

	// Create the session
	Session * theSession = createSession(sessionID, userName, projectName, collectionName, sessionType);

	// Set the user credentials
	theSession->setCredentialsUsername(credentialsUserName);
	theSession->setCredentialsPassword(credentialsUserPassword);
	theSession->setUserCollection(userCollection);

	// Set the temp database user credentials
	theSession->setDatabaseUsername(databaseUserName);
	theSession->setDatabasePassword(databaseUserPassword);

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

	if (runMandatory) {
		OT_LOG_D("Running mandatory services");
		bool success = runMandatoryServices(theSession);
		if (!success)
		{
			throw std::exception("Failed to start mandatory services");
		}
	}
	else {
		OT_LOG_D("Creator does NOT want the mandatory services to be started");
	}

	OTAssertNullptr(theService);
	
	// Add response information
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, newID, responseDoc.GetAllocator());

	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		OT_LOG_D("Adding ToolBar Tab order to the response document");
		responseDoc.AddMember(OT_ACTION_PARAM_UI_ToolBarTabOrder, ot::JsonArray(theSession->toolBarTabOrder(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}

	if (m_logModeManager.getGlobalLogFlagsSet()) {
		ot::JsonArray logData;
		ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), logData, responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_LogFlags, logData, responseDoc.GetAllocator());
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

	Session * theSession = getSession(sessionID);
	if (theSession) {
		Service * theService = theSession->getService(serviceID);
		if (theService == nullptr) {
			std::string errorMessage("A service with the ID \"");
			errorMessage.append(std::to_string(serviceID)).append("\" was not registered");
			throw ErrorException(errorMessage.c_str());
		}
		theSession->broadcastMessage(theService, message);
	}
	else {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleGetServicesInSession(ot::JsonDocument& _commandDoc) {
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));

	auto s = m_sessionMap.find(sessionID);
	if (s == m_sessionMap.end()) {
		std::string errorMessage("A session with the ID \"");
		errorMessage.append(sessionID).append("\" does not exist");
		throw ErrorException(errorMessage.c_str());
	}
	return s->second->getServiceListJSON();
}

std::string SessionService::handleGetSessionExists(ot::JsonDocument& _commandDoc) {
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	auto itm = m_sessionMap.find(sessionID);
	if (itm == m_sessionMap.end()) return OT_ACTION_RETURN_VALUE_FALSE;
	else return OT_ACTION_RETURN_VALUE_TRUE;
}

std::string SessionService::handleServiceClosing(ot::JsonDocument& _commandDoc) {
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	Session * theSession = getSession(sessionID);
	if (!theSession) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

	Service * actualService = theSession->getService(serviceID);
	serviceClosing(actualService, true, true);
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
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	
	Session* session = this->getSession(sessionID);
	if (session) {
		this->forgetSession(session);
		std::thread t(&SessionService::workerShutdownSession, this, serviceID, session);
		t.detach();
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		OT_LOG_E("Session not found: \"" + sessionID + "\"");
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	
}

std::string SessionService::handleServiceFailure(ot::JsonDocument& _commandDoc) {
	OT_LOG_D("Service failure: Received.. Locking service...");
	
	// Get information of the service that failed
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));

	// Get service info (if it exists)

	Session * actualSession = getSession(sessionID);
	if (actualSession == nullptr) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

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
	if (!ot::msg::send(url(), m_globalSessionService->getServiceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
	}
#endif // OT_USE_GSS

	// Display log information
	OT_LOG_I("The session with the ID \"" + actualSession->id() + "\" was closed automatically. Reason: Session emergency shutdown");

	// Finally delete the session
	removeSession(actualSession);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleEnableServiceDebug(ot::JsonDocument& _commandDoc) {
	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	// Check if the provided services is contained in the mandatory services list
	bool found = false;
	for (auto sessionType : m_mandatoryServicesMap) {
		for (auto serviceType : *sessionType.second) {
			if (serviceType.getServiceName() == serviceName) {
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
	for (auto s : m_sessionMap) {
		Session * actualSession = s.second;
		delete actualSession;
	}
	m_sessionMap.clear();

	// todo: check if actually all clear operations were successful

	OT_LOG_I("[RESET] The session service was successfully set back to initial state.");
	OT_LOG_I("[RESET] All session and service informations were removed from the memory");
	
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
	if (theSession) 
	{
		// Check whether the initialization has been completed succesfully
		if (theSession->requestedServices().empty())
		{
			return OT_ACTION_RETURN_VALUE_TRUE;
		}
		else
		{
			std::string missingServices("");
			for (auto& services : theSession->requestedServices())
			{
				missingServices += services.first + ", ";
			}
			OT_LOG_D("Missing services:" + missingServices.substr(0, missingServices.size()-2));
			return OT_ACTION_RETURN_VALUE_FALSE;
		}
	}
	else 
	{
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
	if (theSession == nullptr) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

	Service * theService = theSession->getService(serviceID);
	if (theService == nullptr) {
		OT_LOG_EAS("Service (" + std::to_string(serviceID) + ")not found");
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
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
	
	// Get service info (if it exists)
	Session * actualSession = getSession(sessionID);
	if (!actualSession) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
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
	if (!ot::msg::send(url(), m_globalSessionService->getServiceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
	}
#endif // OT_USE_GSS

	// Display log information
	OT_LOG_W("The session with the ID \"" + actualSession->id() + 
		"\" was closed automatically. Reason: Session emergency shutdown after service startup failed");

	// Finally delete the session
	removeSession(actualSession);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleSetGlobalLogFlags(ot::JsonDocument& _commandDoc) {
	ot::ConstJsonArray flags = ot::json::getArray(_commandDoc, OT_ACTION_PARAM_Flags);
	m_logModeManager.setGlobalLogFlags(ot::logFlagsFromJsonArray(flags));

	ot::LogDispatcher::instance().setLogFlags(m_logModeManager.getGlobalLogFlags());

	// Update existing session services
	this->updateLogMode(m_logModeManager);

	return OT_ACTION_RETURN_VALUE_OK;
}

void SessionService::workerShutdownSession(ot::serviceID_t _serviceId, Session* _session) {
	// Get service info
	if (_session == nullptr) {
		OT_LOG_E("No session provided");
		return;
	}
	Service* theService = _session->getService(_serviceId);

	std::string senderServiceName("<No service name>");
	if (theService) {
		senderServiceName = theService->name();
		theService->setReceiveBroadcastMessages(false);
	}
	// Notify GDS
	m_globalDirectoryService->notifySessionClosed(_session->id());

#ifdef OT_USE_GSS
	// Notify GSS
	ot::JsonDocument gssShutdownDoc;
	gssShutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());
	gssShutdownDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_session->id(), gssShutdownDoc.GetAllocator()), gssShutdownDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send(url(), m_globalSessionService->getServiceURL(), ot::EXECUTE, gssShutdownDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send \"session shutdown completed\" notification to GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invaild response for \"session shutdown completed\" notification from GSS at \"" + m_globalSessionService->getServiceURL() + "\"");
	}

#endif
	// Remove service that requested shutdown
	_session->removeService(_serviceId, false);

	// Close session
	_session->shutdown(nullptr);

	// Show info log
	OT_LOG_D("The session with the ID \"" + _session->id() + "\" was closed. Reason: Shutdown requested by service \"" + senderServiceName + "\"");

	// Finally delete the session
	delete _session;
}

void SessionService::initializeSystemInformation() {

	m_systemLoadInformation.initialize();
}

void SessionService::updateLogMode(const ot::LogModeManager& _newData) {
	m_logModeManager = _newData;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_SetLogFlags, doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, flagsArr, doc.GetAllocator());

	for (const auto& it : m_sessionMap) {
		it.second->broadcast(nullptr, doc, false, false);
	}
}
