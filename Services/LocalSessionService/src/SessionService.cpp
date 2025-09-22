//! @file SessionService.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LSS header
#include "SessionService.h"

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTSystem/PortManager.h"
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ContainerHelper.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/ServiceLogNotifier.h"

// std header
#include <chrono>
#include <fstream>

SessionService& SessionService::instance(void) {
	static SessionService g_instance;
	return g_instance;
}

int SessionService::initialize(const std::string& _ownUrl, const std::string& _gssUrl) {
	if (_ownUrl.empty()) {
		OT_LOG_EA("Uwn url may not be empty");
		return ot::AppExitCode::ServiceUrlInvalid;
	}

	SessionService& lss = SessionService::instance();
	lss.m_url = _ownUrl;

	// Determine debug port ranges
	size_t colonIndex = _ownUrl.rfind(':');
	if (colonIndex == std::string::npos) {
		OT_LOG_EA("Unable to determine own port");
		return ot::AppExitCode::FailedToConvertPort;
	}

	bool failed = false;
	ot::port_t portFrom = ot::String::toNumber<ot::port_t>(_ownUrl.substr(colonIndex + 1), failed);
	if (failed || portFrom == 0) {
		OT_LOG_EA("Invalid port number format");
		return ot::AppExitCode::FailedToConvertPort;
	}
	lss.m_debugPortManager.addPortRange(portFrom, portFrom + 79);

	// Initialize connection to GSS
	if (!lss.m_gss.connect(_gssUrl)) {
		return ot::AppExitCode::GSSRegistrationFailed;
	}

	// Initialize connection to GDS
	GSSRegistrationInfo regInfo = lss.m_gss.getRegistrationResult();
	if (!lss.m_gds.connect(regInfo.getGdsURL(), true)) {
		return ot::AppExitCode::GDSRegistrationFailed;
	}

	// Initialize system load information
	lss.m_systemLoadInformation.initialize();

	return ot::AppExitCode::Success;
}

SessionService::SessionService() 
	: m_id(ot::invalidServiceID)
{
	//
	// Development services list (this contains all services together (potentially even including experimental ones) for testing purposes)
	//
	std::list<ot::ServiceBase> DevelopmentSessionServices;
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PHREEC, OT_INFO_SERVICE_TYPE_PHREEC));
	//DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_KRIGING, OT_INFO_SERVICE_TYPE_KRIGING));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_FITTD, OT_INFO_SERVICE_TYPE_FITTD));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CartesianMeshService, OT_INFO_SERVICE_TYPE_CartesianMeshService));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_GETDP, OT_INFO_SERVICE_TYPE_GETDP));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ELMERFEM, OT_INFO_SERVICE_TYPE_ELMERFEM));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_DebugService, OT_INFO_SERVICE_TYPE_DebugService));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService));
	DevelopmentSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT, std::move(DevelopmentSessionServices));

	//
	// 3D Simulation services list (this contains all services relevant for 3D simulation)
	//
	std::list<ot::ServiceBase> Simulation3DSessionServices;
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PHREEC, OT_INFO_SERVICE_TYPE_PHREEC));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_FITTD, OT_INFO_SERVICE_TYPE_FITTD));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CartesianMeshService, OT_INFO_SERVICE_TYPE_CartesianMeshService));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_GETDP, OT_INFO_SERVICE_TYPE_GETDP));
	Simulation3DSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ELMERFEM, OT_INFO_SERVICE_TYPE_ELMERFEM));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_3DSIM, std::move(Simulation3DSessionServices));

	//
	// Data Pipeline services list (this contains all services relevant for pipeline based data processing)
	//
	std::list<ot::ServiceBase> DataPipelineSessionServices;
	DataPipelineSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	DataPipelineSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	DataPipelineSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE));
	DataPipelineSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_DATAPIPELINE, std::move(DataPipelineSessionServices));

	//
	// StudioSuite services list (this contains all services relevant for Studio Suite integration)
	//
	std::list<ot::ServiceBase> StudioSuiteSessionServices;
	StudioSuiteSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	StudioSuiteSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_STUDIOSUITE, OT_INFO_SERVICE_TYPE_STUDIOSUITE));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE, std::move(StudioSuiteSessionServices));

	//
	// LTSpice services list (this contains all services relevant for LTSpice integration)
	//
	std::list<ot::ServiceBase> LTSpiceSessionServices;
	LTSpiceSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	LTSpiceSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_LTSPICE, OT_INFO_SERVICE_TYPE_LTSPICE));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_LTSPICE, std::move(LTSpiceSessionServices));

	//
	// Pyrit services list (this contains all services relevant for Pyrit integration)
	//
	std::list<ot::ServiceBase> PyritSessionServices;
	PyritSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	PyritSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_ModelingService, OT_INFO_SERVICE_TYPE_ModelingService));
	PyritSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_VisualizationService, OT_INFO_SERVICE_TYPE_VisualizationService));
	PyritSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_TetMeshService, OT_INFO_SERVICE_TYPE_TetMeshService));
	PyritSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_PYRIT, OT_INFO_SERVICE_TYPE_PYRIT));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_PYRIT, std::move(PyritSessionServices));

	//
	// Circuit Simulation services list (this contains all services relevant for Pyrit integration)
	//
	std::list<ot::ServiceBase> CircuitSimulationSessionServices;
	CircuitSimulationSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL));
	CircuitSimulationSessionServices.push_back(ot::ServiceBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	m_mandatoryServicesMap.insert_or_assign(OT_ACTION_PARAM_SESSIONTYPE_CIRCUITSIMULATION, std::move(CircuitSimulationSessionServices));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

std::list<std::string> SessionService::getSessionIDs() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return ot::ContainerHelper::getKeys(m_sessions);
}

bool SessionService::getIsServiceInDebugMode(const std::string& _serviceName) {
	return m_debugServices.contains(_serviceName);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

Session SessionService::createSession(const std::string& _sessionID, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _sessionType) {
	if (_sessionID.length() == 0) {
		throw ot::Exception::InvalidArgument("Session ID must not be empty");
	}

	if (m_sessions.find(_sessionID) != m_sessions.end()) {
		throw ot::Exception::ObjectAlreadyExists("Session \"" + _sessionID + "\" already exists");
	}

	return Session(_sessionID, _userName, _projectName, _collectionName, _sessionType);
}

Session& SessionService::getSession(const std::string& _sessionID) {
	auto it = m_sessions.find(_sessionID);
	if (it == m_sessions.end()) {
		OT_LOG_E("Session not found \"" + _sessionID + "\"");
		throw ot::Exception::ObjectNotFound("Session \"" + _sessionID + "\" not found");
	}
	else {
		return it->second;
	}
}

bool SessionService::runMandatoryServices(Session& _session) {
	auto it = m_mandatoryServicesMap.find(_session.getType());
	if (it == m_mandatoryServicesMap.end()) {
		OT_LOG_E("No mandatory services registered for the session { \"Type\": \"" + _session.getType() + "\" }");
		throw ot::Exception::ObjectNotFound("No mandatory services registered for the session { \"Type\": \"" + _session.getType() + "\" }");
	}

	// Collect all services to run in debug and release mode
	std::list<ot::ServiceBase> debugServices;
	std::list<ot::ServiceBase> releaseServices;

	for (const ot::ServiceBase& serviceInfo : it->second) {
		if (this->getIsServiceInDebugMode(serviceInfo.getServiceName())) {
			debugServices.push_back(serviceInfo);
		}
		else {
			releaseServices.push_back(serviceInfo);
		}
	}

	// Request the user to start the debug services
	for (const ot::ServiceBase& serviceInfo : debugServices) {
		this->runServiceInDebug(serviceInfo, _session);
	}

	// Send release mode services to the GDS
	if (!releaseServices.empty()) {
		for (ot::ServiceBase& serviceInfo : releaseServices) {
			Service& newService = _session.addRequestedService(serviceInfo);
			serviceInfo.setServiceID(newService.getServiceID());
		}

		OT_LOG_D("Starting services via DirectoryService (Service.Count = \"" + std::to_string(releaseServices.size()) + "\")");
		if (!m_gds.requestToStartServices(releaseServices, _session.getID(), m_url)) {
			OT_LOG_E("Failed to request service start");
			return false;
		}
	}

	return true;
}

void SessionService::updateLogMode(const ot::LogModeManager& _newData) {
	m_logModeManager = _newData;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_SetLogFlags, doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, flagsArr, doc.GetAllocator());

	std::string notificationMessage = doc.toJson();

	for (auto& it : m_sessions) {
		it.second.sendBroadcast(ot::invalidServiceID, notificationMessage);
	}
}

void SessionService::serviceFailure(const std::string& _sessionID, ot::serviceID_t _serviceID) {
	std::lock_guard<std::mutex> lock(m_mutex);

	Session& session = this->getSession(_sessionID);

	// Prepare session information
	session.removeFailedService(_serviceID);
	session.prepareSessionForShutdown();
	
	// Notify the GDS about the shutdown
	m_gds.notifySessionShuttingDown(_sessionID);
	
	// Shutdown the session as emergency
	session.shutdownSession(_serviceID, true);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Session Information



// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

Service& SessionService::runServiceInDebug(const ot::ServiceBase& _serviceInfo, Session& _session) {
	OT_LOG_D("Starting service in debug mode { \"Name\": \"" + _serviceInfo.getServiceName() + "\", \"Type\": \"" + _serviceInfo.getServiceType() + "\" }");

	// Get dev root path
	char* buffer = nullptr;
	size_t bufferSize = 0;
	if (_dupenv_s(&buffer, &bufferSize, "OPENTWIN_DEV_ROOT") != 0 || buffer == nullptr) {
		OT_LOG_E("Please specify the environment variable \"OPENTWIN_DEV_ROOT\"");
		throw ot::Exception::InvalidArgument("Please specify the environment variable \"OPENTWIN_DEV_ROOT\"");
	}

	std::string path = buffer;
	delete[] buffer;
	buffer = nullptr;

	std::string serviceNameLower = ot::String::toLower(_serviceInfo.getServiceName());

	path.append("\\Deployment\\" + serviceNameLower + ".cfg");

	// Create new service url
	size_t colonIndex = m_url.rfind(':');
	if (colonIndex == std::string::npos) {
		OT_LOG_EA("Unable to determine own port");
		throw ot::Exception::InvalidArgument("Unable to determine own port");
	}
	std::string serviceURL = m_url.substr(0, colonIndex);
	std::string websocketURL = serviceURL;

	ot::port_t servicePort = m_debugPortManager.determineAndBlockAvailablePort();
	ot::port_t websocketPort = ot::invalidPortNumber;

	serviceURL.append(":").append(std::to_string(servicePort));

	// Register new service
	Service& newDebugService = _session.addRequestedService(_serviceInfo);
	newDebugService.setIsDebug(true);
	newDebugService.setServiceURL(serviceURL);

	// Create start params
	ot::JsonDocument statupParams;
	statupParams.AddMember(OT_ACTION_PARAM_SITE_ID, ot::JsonString("1", statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(serviceURL, statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_SERVICE_ID, newDebugService.getServiceID(), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(this->getUrl(), statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL, ot::JsonString("", statupParams.GetAllocator()), statupParams.GetAllocator());
	statupParams.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_session.getID(), statupParams.GetAllocator()), statupParams.GetAllocator());

	// If the service is a relay service add websocket information
	if (newDebugService.getServiceType() == OT_INFO_SERVICE_TYPE_RelayService) {
		websocketPort = m_debugPortManager.determineAndBlockAvailablePort();
		websocketURL.append(":").append(std::to_string(websocketPort));
		newDebugService.setWebsocketUrl(websocketURL);
		statupParams.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(websocketURL, statupParams.GetAllocator()), statupParams.GetAllocator());
	}

	// Open config file for writing
	std::ofstream stream(path);
	
	if (!stream.is_open()) {
		// Remove service from session
		OT_LOG_E("Failed to open file stream for writing \"" + path + "\"");
		_session.serviceDisconnected(newDebugService.getServiceID(), false);

		// Free the used debug ports
		m_debugPortManager.freePort(servicePort);
		if (websocketPort != ot::invalidPortNumber) {
			m_debugPortManager.freePort(websocketPort);
		}

		throw ot::Exception::File("Failed to open file stream for writing \"" + path + "\"");
	}

	// Write configuration to file
	stream << statupParams.toJson();
	stream.close();

	OT_LOG_D("Debug service params written to file { \"Service\": \"" + newDebugService.getServiceName() + "\", \"File\": \"" + path + "\" }");

	return newDebugService;
}

Service& SessionService::runRelayService(Session& _session, const std::string& _serviceName, const std::string& _serviceType) {
	ot::ServiceBase relayInfo(OT_INFO_SERVICE_TYPE_RelayService, OT_INFO_SERVICE_TYPE_RelayService);

	if (this->getIsServiceInDebugMode(OT_INFO_SERVICE_TYPE_RelayService)) {
		// Start relay in debug mode
		Service& relay = this->runServiceInDebug(relayInfo, _session);
		relay.setServiceName(_serviceName);
		relay.setServiceType(_serviceType);

		// Create check command
		OT_LOG_D("Relay start requested in debug mode. Now waiting to come alive.");
		ot::JsonDocument checkCommandDoc;
		checkCommandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CheckRelayStartupCompleted, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
		checkCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, relay.getServiceID(), checkCommandDoc.GetAllocator());
		std::string checkCommandString = checkCommandDoc.toJson();

		// Wait until the service is alive
		while (true) {
			std::string response;
			if (ot::msg::send("", relay.getServiceURL(), ot::EXECUTE, checkCommandString, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				if (response == OT_ACTION_RETURN_VALUE_TRUE) {
					OT_LOG_D("Relay service startup completed successfully");
					break;
				}
				else if (response == OT_ACTION_RETURN_VALUE_FALSE) {
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(500ms);
				}
				else {
					OT_LOG_E("Invalid response \"" + response + "\"");
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(500ms);
				}
			}
			else {
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(500ms);
			}
		}

		return relay;
	}
	else {
		OT_LOG_D("Starting service \"" OT_INFO_SERVICE_TYPE_RelayService "\" via DirectoryService");

		Service& relay = _session.addRequestedService(relayInfo);

		std::string newServiceUrl;
		std::string newWebsocketUrl;
		if (!m_gds.startRelayService(relay.getServiceID(), _session.getID(), m_url, newServiceUrl, newWebsocketUrl)) {
			throw ot::Exception::ObjectNotFound("Failed to start relay service");
		}

		relay.setServiceName(_serviceName);
		relay.setServiceType(_serviceType);
		relay.setServiceURL(newServiceUrl);
		relay.setWebsocketUrl(newWebsocketUrl);

		return relay;
	}
}

bool SessionService::hasMandatoryService(const std::string& _serviceName) const {
	for (const auto& it : m_mandatoryServicesMap) {
		for (const ot::ServiceBase& serviceInfo : it.second) {
			if (serviceInfo.getServiceName() == _serviceName) {
				return true;
			}
		}
	}
	return false;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void SessionService::workerShutdownSession() {
	while (m_workerRunning) {
		//! @todo Implement shutdown worker
	}
	OT_LOG_D("Shutdown session worker finished");
}

void SessionService::workerRunServices() {
	while (m_workerRunning) {
		//! @todo Implement run services worker
	}
	OT_LOG_D("Run services worker finished");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

std::string SessionService::handleGetDBURL(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_gss.isConnected()) {
		return m_gss.getRegistrationResult().getDataBaseURL();
	}
	else {
		OT_LOG_E("There is no active connection to the GSS");
		return "";
	}
}

std::string SessionService::handleGetAuthURL(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_gss.isConnected()) {
		return m_gss.getRegistrationResult().getAuthURL();
	}
	else {
		OT_LOG_E("There is no active connection to the GSS");
		return "";
	}
}

std::string SessionService::handleGetGlobalServicesURL(ot::JsonDocument& _commandDoc) {
	// The info message is only present if the request is done by the UI frontend upon startup
	if (_commandDoc.HasMember(OT_ACTION_PARAM_MESSAGE)) {
		if (ot::json::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE) == OT_INFO_MESSAGE_LogIn) {
			return OT_ACTION_RETURN_INDICATOR_Error "Invalid global session service URL";
		}
	}

	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_gss.isConnected()) {
		GSSRegistrationInfo info = m_gss.getRegistrationResult();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_PARAM_SERVICE_DBURL, ot::JsonString(info.getDataBaseURL(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_AUTHURL, ot::JsonString(info.getAuthURL(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_GDSURL, ot::JsonString(m_gds.getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

		return doc.toJson();
	}
	else {
		OT_LOG_E("There is no active connection to the GSS");
		return "";
	}
}

std::string SessionService::handleGetMandatoryServices(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::JsonDocument doc(rapidjson::kArrayType);
	for (const auto& session : m_mandatoryServicesMap) {
		ot::JsonObject sessionObj;
		sessionObj.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(session.first, doc.GetAllocator()), doc.GetAllocator());
		
		ot::JsonArray servicesArr;
		for (const ot::ServiceBase& service : session.second) {
			ot::JsonObject serviceObj;
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.getServiceName(), doc.GetAllocator()), doc.GetAllocator());
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getServiceType(), doc.GetAllocator()), doc.GetAllocator());
			servicesArr.PushBack(serviceObj, doc.GetAllocator());
		}
		doc.PushBack(sessionObj, doc.GetAllocator());
	}
	return doc.toJson();
}

std::string SessionService::handleGetSystemInformation(ot::JsonDocument& _doc) {
	std::lock_guard<std::mutex> lock(m_mutex);

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

	for (auto& session : m_sessions) {
		ot::JsonValue info;
		info.SetObject();

		info.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(session.second.getID(), reply.GetAllocator()), reply.GetAllocator());
		info.AddMember(OT_ACTION_PARAM_SESSION_PROJECT, ot::JsonString(session.second.getProjectName(), reply.GetAllocator()), reply.GetAllocator());
		info.AddMember(OT_ACTION_PARAM_SESSION_USER, ot::JsonString(session.second.getUserName(), reply.GetAllocator()), reply.GetAllocator());
		info.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(session.second.getType(), reply.GetAllocator()), reply.GetAllocator());

		ot::JsonArray servicesInformation;
		session.second.addAliveServicesToJsonArray(servicesInformation, reply.GetAllocator());

		info.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, servicesInformation, reply.GetAllocator());

		sessionInfo.PushBack(info, reply.GetAllocator());
	}

	reply.AddMember(OT_ACTION_PARAM_Sessions, sessionInfo, reply.GetAllocator());

	return reply.toJson();
}

std::string SessionService::handleCreateNewSession(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_gds.isConnected()) {
		return OT_ACTION_RETURN_INDICATOR_Error "No global directory service connected";
	}

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
	std::string serviceUrl = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_URL);

	// Optional params
	bool shouldRunRelayService = ot::json::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY);

	// Notify GSS that the session request was received (confirm session)
	if (!m_gss.confirmSession(sessionID, userName)) {
		return OT_ACTION_RETURN_INDICATOR_Error "Failed to confirm session at GSS";
	}

	// Create the session
	Session newSession = this->createSession(sessionID, userName, projectName, collectionName, sessionType);

	// Set the user credentials
	newSession.setCredentialsUsername(credentialsUserName);
	newSession.setCredentialsPassword(credentialsUserPassword);
	newSession.setUserCollection(userCollection);

	// Set the temp database user credentials
	newSession.setDatabaseUsername(databaseUserName);
	newSession.setDatabasePassword(databaseUserPassword);

	// Create response document
	ot::JsonDocument responseDoc;

	// Create a new service and store its information
	if (shouldRunRelayService) {
		OT_LOG_D("Relay service requested by session creator");

		Service& relayService = this->runRelayService(newSession, serviceName, serviceType);
		relayService.setRequested(true);
		relayService.setAlive(false);
		
		OT_LOG_D("Relay service started { \"Service.URL\": \"" + relayService.getServiceURL() + "\", \"Websocket.URL\": \"" + relayService.getWebsocketUrl() + "\" }");

		responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, relayService.getServiceID(), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(relayService.getWebsocketUrl(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}
	else {
		OT_LOG_D("Session created without relay service (for session creator)");

		ot::ServiceBase requestingServiceInfo(serviceName, serviceType);
		requestingServiceInfo.setServiceURL(serviceUrl);
		requestingServiceInfo.setSiteID("1");

		Service& requestingService = newSession.addRequestedService(requestingServiceInfo);
		
		// Since the service that requested the session is already alive we don't have to wait for it to "come alive"
		requestingService.setRequested(true);
		requestingService.setAlive(false);

		responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, requestingService.getServiceID(), responseDoc.GetAllocator());
	}

	OT_LOG_D("Running mandatory services");
	if (!this->runMandatoryServices(newSession)) {
		throw ot::Exception::RequestFailed("Failed to start mandatory services");
	}
	
	// Add response information
	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		OT_LOG_D("Adding ToolBar Tab order to the response document");
		responseDoc.AddMember(OT_ACTION_PARAM_UI_ToolBarTabOrder, ot::JsonArray(newSession.getToolBarTabOrder(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}

	responseDoc.AddMember(OT_ACTION_PARAM_GlobalLoggerUrl, ot::JsonString(ot::ServiceLogNotifier::instance().loggingServiceURL(), responseDoc.GetAllocator()), responseDoc.GetAllocator());

	if (m_logModeManager.getGlobalLogFlagsSet()) {
		ot::JsonArray logData;
		ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), logData, responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_LogFlags, logData, responseDoc.GetAllocator());
	}

	// Finally store the session and start its health check
	Session& addedSession = m_sessions.insert_or_assign(newSession.getID(), std::move(newSession)).first->second;
	addedSession.startHealthCheck();

	return responseDoc.toJson();
}

std::string SessionService::handleCheckProjectOpen(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	// Required session params
	std::string projectName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_PROJECT_NAME);
	
	// Check session
	for (auto& session : m_sessions) {
		if (session.second.getProjectName() == projectName) {
			return ot::ReturnMessage::toJson(ot::ReturnMessage::True, session.second.getUserName());
		}
	}

	return ot::ReturnMessage::toJson(ot::ReturnMessage::False);
}

std::string SessionService::handleConfirmService(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	// Required params
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	ot::serviceID_t serviceID = static_cast<ot::serviceID_t>(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	
	Session& theSession = this->getSession(sessionID);
	
	// Set the service alive
	if (_commandDoc.HasMember(OT_ACTION_PARAM_SERVICE_URL)) {
		std::string serviceUrl = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_URL);
		theSession.setServiceAlive(serviceID, serviceUrl, true);
	}
	else {
		theSession.setServiceAlive(serviceID, true);
	}
	
	ot::ServiceBase serviceInfo = theSession.getServiceInfo(serviceID);

	// Create response document
	ot::JsonDocument response;

	// Add response information
	if (serviceInfo.getServiceType() == OT_INFO_SERVICE_TYPE_UI) {
		response.AddMember(OT_ACTION_PARAM_UI_ToolBarTabOrder, ot::JsonArray(theSession.getToolBarTabOrder(), response.GetAllocator()), response.GetAllocator());
	}
	if (m_logModeManager.getGlobalLogFlagsSet()) {
		ot::JsonArray logArr;
		ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), logArr, response.GetAllocator());
		response.AddMember(OT_ACTION_PARAM_LogFlags, logArr, response.GetAllocator());
	}

	// If all services are ready, add services information
	if (!theSession.hasRequestedServices(serviceID)) {
		// Session is already active
		ot::JsonArray aliveServices;
		theSession.addAliveServicesToJsonArray(aliveServices, response.GetAllocator());
		response.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, aliveServices, response.GetAllocator());

		response.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(theSession.getUserName(), response.GetAllocator()), response.GetAllocator());
		response.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(theSession.getProjectName(), response.GetAllocator()), response.GetAllocator());

		theSession.sendRunCommand();
	}

	return response.toJson();
}

std::string SessionService::handleGetSessionExists(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	auto itm = m_sessions.find(sessionID);
	if (itm == m_sessions.end()) {
		return OT_ACTION_RETURN_VALUE_FALSE;
	}
	else {
		return OT_ACTION_RETURN_VALUE_TRUE;
	}
}

std::string SessionService::handleServiceClosing(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	
	Session& theSession = this->getSession(sessionID);
	theSession.serviceDisconnected(serviceID, true);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleShutdownSession(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	
	Session& session = this->getSession(sessionID);
	session.prepareSessionForShutdown();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleServiceFailure(ot::JsonDocument& _commandDoc) {
	OT_LOG_D("Service failure: Received.. Locking service...");

	// Get information of the service that failed
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));

	this->serviceFailure(sessionID, serviceID);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleServiceShutdownCompleted(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));

	Session& theSession = this->getSession(sessionID);
	theSession.setServiceShutdownCompleted(serviceID);

	if (!theSession.hasAliveServices()) {
		// All alive service have been removed, close the session
		m_gss.notifySessionShutdownCompleted(sessionID);
		m_gds.notifySessionShutdownCompleted(sessionID);
		m_sessions.erase(sessionID);
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleEnableServiceDebug(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	
	if (!this->hasMandatoryService(serviceName)) {
		return OT_ACTION_RETURN_INDICATOR_Error "The specified service is not contained in the mandatory services list";
	}
	else {
		m_debugServices.insert(serviceName);
		OT_LOG_D("Enabled the debug mode for the service \"" + serviceName + "\"");
		return OT_ACTION_RETURN_VALUE_OK;
	}
}

std::string SessionService::handleDisableServiceDebug(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	m_debugServices.erase(serviceName);
	OT_LOG_D("Disabled the debug mode for the service \"" + serviceName + "\"");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleGetDebugInformation(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::JsonDocument doc;

	ot::JsonObject gssObj;
	m_gss.addToJsonObject(gssObj, doc.GetAllocator());
	doc.AddMember("GSS", gssObj, doc.GetAllocator());

	ot::JsonObject gdsObj;
	m_gds.addToJsonObject(gdsObj, doc.GetAllocator());
	doc.AddMember("GDS", gdsObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_SITE_ID, ot::JsonString(m_siteID, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_id, doc.GetAllocator());

	ot::JsonArray dbgArr;
	for (const std::string& dbg : m_debugServices) {
		dbgArr.PushBack(ot::JsonString(dbg, doc.GetAllocator()), doc.GetAllocator());
	}
	doc.AddMember("DebugServices", dbgArr, doc.GetAllocator());

	ot::JsonArray mandatoryArr;
	for (const auto& mandatory : m_mandatoryServicesMap) {
		ot::JsonObject mandatoryObj;
		mandatoryObj.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(mandatory.first, doc.GetAllocator()), doc.GetAllocator());
		
		ot::JsonArray mservicesArr;
		for (const ot::ServiceBase& mservice : mandatory.second) {
			ot::JsonObject mserviceObj;
			mserviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(mservice.getServiceName(), doc.GetAllocator()), doc.GetAllocator());
			mserviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(mservice.getServiceType(), doc.GetAllocator()), doc.GetAllocator());
			mservicesArr.PushBack(mserviceObj, doc.GetAllocator());
		}
		mandatoryObj.AddMember("Services", mservicesArr, doc.GetAllocator());
		mandatoryArr.PushBack(mandatoryObj, doc.GetAllocator());
	}
	doc.AddMember("MandatoryServices", mandatoryArr, doc.GetAllocator());

	ot::JsonArray sessionArr;
	for (auto& session : m_sessions) {
		ot::JsonObject sessionObj;
		session.second.addToJsonObject(sessionObj, doc.GetAllocator());
		sessionArr.PushBack(sessionObj, doc.GetAllocator());
	}
	doc.AddMember("Sessions", sessionArr, doc.GetAllocator());

	return doc.toJson();
}

std::string SessionService::handleCheckStartupCompleted(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	ot::serviceID_t serviceID(static_cast<ot::serviceID_t>(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID)));

	Session& session = this->getSession(sessionID);

	if (session.hasRequestedServices(serviceID)) {
		return ot::ReturnMessage::toJson(ot::ReturnMessage::False);
	}
	else {
		return ot::ReturnMessage::toJson(ot::ReturnMessage::True);
	}
}

std::string SessionService::handleAddMandatoryService(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string sessionType(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_TYPE));
	std::string serviceName(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME));
	std::string serviceType(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_TYPE));

	// Locate current data
	auto it = m_mandatoryServicesMap.find(sessionType);
	std::list<ot::ServiceBase>* data = nullptr;
	if (it == m_mandatoryServicesMap.end()) {
		data = &m_mandatoryServicesMap.insert_or_assign(sessionType, std::list<ot::ServiceBase>()).first->second;
	}
	else {
		data = &it->second;
	}

	OTAssertNullptr(data);

	// Check for duplicates
	bool found = false;
	for (const ot::ServiceBase& existing : *data) {
		if (ot::String::toLower(existing.getServiceName()) == ot::String::toLower(serviceName) && 
			ot::String::toLower(existing.getServiceType()) == ot::String::toLower(serviceType)) {
			found = true;
			break;
		}
	}
	if (found) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	else {
		ot::ServiceBase info(serviceName, serviceType);
		data->push_back(std::move(info));

		OT_LOG_D("Mandatory service for prject type \"" + sessionType + "\": Added service \"" + serviceName + "\"");
		
		return OT_ACTION_RETURN_VALUE_OK;
	}
	
}

std::string SessionService::handleRegisterNewGlobalDirectoryService(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string serviceURL(ot::json::getString(_commandDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL));

	if (m_gds.connect(serviceURL, false)) {
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
}

std::string SessionService::handleServiceStartupFailed(ot::JsonDocument& _commandDoc) {
	// Get information of the service that failed
	ot::serviceID_t serviceID = static_cast<ot::serviceID_t>(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	
	this->serviceFailure(sessionID, serviceID);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string SessionService::handleSetGlobalLogFlags(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::ConstJsonArray flags = ot::json::getArray(_commandDoc, OT_ACTION_PARAM_Flags);
	m_logModeManager.setGlobalLogFlags(ot::logFlagsFromJsonArray(flags));

	ot::LogDispatcher::instance().setLogFlags(m_logModeManager.getGlobalLogFlags());

	// Update existing session services
	this->updateLogMode(m_logModeManager);

	return OT_ACTION_RETURN_VALUE_OK;
}
