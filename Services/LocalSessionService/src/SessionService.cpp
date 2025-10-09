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
#include "OTCore/LogDispatcher.h"
#include "OTCore/String.h"
#include "OTCore/DebugHelper.h"
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
	lss.m_debugPortManager.addPortRange(portFrom + 1, portFrom + 79);

	// Initialize connection to GSS
	if (!lss.m_gss.connect(_gssUrl)) {
		return ot::AppExitCode::GSSRegistrationFailed;
	}
	GSSRegistrationInfo regInfo = lss.m_gss.getRegistrationResult();

#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
	ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_LocalSessionService, regInfo.getLoggingURL(), true);
#else
	ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_LocalSessionService, regInfo.getLoggingURL(), false);
#endif

	ot::DebugHelper::serviceSetupCompleted(ot::ServiceBase(OT_INFO_SERVICE_TYPE_LocalSessionService, OT_INFO_SERVICE_TYPE_LocalSessionService, lss.getUrl(), ot::invalidServiceID));

	lss.m_authUrl = regInfo.getAuthURL();
	lss.m_lmsUrl = regInfo.getLMSURL();

	// Initialize log flags
	ot::LogDispatcher::instance().setLogFlags(regInfo.getLogFlags());

	// Initialize connection to GDS
	if (!regInfo.getGdsURL().empty() && !lss.m_gds.connect(regInfo.getGdsURL(), true)) {
		return ot::AppExitCode::GDSRegistrationFailed;
	}

	lss.m_dataBaseUrl = regInfo.getDataBaseURL();

	// Initialize system load information
	lss.m_systemLoadInformation.initialize();

	return ot::AppExitCode::Success;
}

SessionService::SessionService() 
	: m_id(ot::invalidServiceID), m_workerRunning(false), m_shutdownWorkerThread(nullptr)
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



	connectAction(OT_ACTION_CMD_GetAuthorisationServerUrl, this, &SessionService::handleGetAuthURL, ot::ALL_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_GetGlobalServicesUrl, this, &SessionService::handleGetGlobalServicesURL, ot::ALL_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_CreateNewSession, this, &SessionService::handleCreateNewSession, ot::ALL_MESSAGE_TYPES);

	connectAction(OT_ACTION_CMD_GetSystemInformation, this, &SessionService::handleGetSystemInformation, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_GetMandatoryServices, this, &SessionService::handleGetMandatoryServices, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ConfirmService, this, &SessionService::handleConfirmService, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ShowService, this, &SessionService::handleShowService, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_GetSessionExists, this, &SessionService::handleGetSessionExists, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ServiceClosing, this, &SessionService::handleServiceClosing, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ShutdownSession, this, &SessionService::handleShutdownSession, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ServiceFailure, this, &SessionService::handleServiceFailure, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ServiceShutdownCompleted, this, &SessionService::handleServiceShutdownCompleted, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ServiceEnableDebug, this, &SessionService::handleEnableServiceDebug, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ServiceDisableDebug, this, &SessionService::handleDisableServiceDebug, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_GetDebugInformation, this, &SessionService::handleGetDebugInformation, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_CheckStartupCompleted, this, &SessionService::handleCheckStartupCompleted, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_AddMandatoryService, this, &SessionService::handleAddMandatoryService, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, this, &SessionService::handleRegisterNewGlobalDirectoryService, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_RegisterNewLibraryManagementService, this, &SessionService::handleRegisterNewLibraryManagementService, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_ServiceStartupFailed, this, &SessionService::handleServiceStartupFailed, ot::SECURE_MESSAGE_TYPES);
	connectAction(OT_ACTION_CMD_SetGlobalLogFlags, this, &SessionService::handleSetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES);
}

SessionService::~SessionService() {
	m_mutex.lock();
	this->stopWorkerThreads();
	m_mutex.unlock();
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
		OT_LOG_E("No mandatory services registered for the session { \"SessionType\": \"" + _session.getType() + "\" }");
		throw ot::Exception::ObjectNotFound("No mandatory services registered for the session { \"Type\": \"" + _session.getType() + "\" }");
	}

	// Collect all services to run in debug and release mode
	std::list<ot::ServiceBase> debugServices;
	std::list<ot::ServiceBase> releaseServices;

	ot::serviceID_t validServiceID = ot::invalidServiceID;

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

		OT_LOG_D("Starting services via DirectoryService { \"ServiceCount\": " + std::to_string(releaseServices.size()) + " }");

		ot::ServiceInitData generalData = _session.createServiceInitData(1);

		if (!m_gds.requestToStartServices(generalData, releaseServices)) {
			OT_LOG_E("Failed to request service start");
			return false;
		}
	}

	return true;
}

void SessionService::updateLogMode(const ot::LogFlags& _newData) {
	ot::LogDispatcher::instance().setLogFlags(_newData);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_SetLogFlags, doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(_newData, flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, flagsArr, doc.GetAllocator());

	std::string notificationMessage = doc.toJson();

	for (auto& it : m_sessions) {
		it.second.sendBroadcast(ot::invalidServiceID, notificationMessage);
	}
}

void SessionService::serviceFailure(const std::string& _sessionID, ot::serviceID_t _serviceID) {
	std::lock_guard<std::mutex> lock(m_mutex);

	OT_LOG_W("Handling service failure { \"ServiceID\"" + std::to_string(_serviceID) + ", \"SessionID\": \"" + _sessionID + "\" }");

	Session& session = this->getSession(_sessionID);

	// Prepare session information
	session.removeFailedService(_serviceID, m_debugPortManager);
	session.prepareSessionForShutdown(_serviceID, m_debugPortManager);
	
	// Notify the GDS about the shutdown
	m_gds.notifySessionShuttingDown(_sessionID);
	
	m_shutdownQueue.push_back(std::make_pair(_sessionID, true));

	this->startWorkerThreads();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Session Information



// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

Service& SessionService::runServiceInDebug(const ot::ServiceBase& _serviceInfo, Session& _session) {
	OT_LOG_D("Starting service in debug mode { " + Service::debugLogString(_serviceInfo, _session.getID()) + "}");

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
		std::string msg = "Unable to determine own port { \"Url\": \"" + m_url + "\" }";
		OT_LOG_EAS(msg);
		throw ot::Exception::InvalidArgument(msg);
	}
	std::string serviceURL = m_url.substr(0, colonIndex);
	std::string websocketURL = serviceURL;

	ot::port_t servicePort = m_debugPortManager.determineAndBlockAvailablePort();
	ot::port_t websocketPort = ot::invalidPortNumber;

	serviceURL.append(":").append(std::to_string(servicePort));

	// Register new service
	Service& newDebugService = _session.addRequestedService(_serviceInfo);
	newDebugService.setIsDebug();
	newDebugService.setServiceURL(serviceURL);

	// Create start params
	ot::JsonDocument statupParams;
	statupParams.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(serviceURL, statupParams.GetAllocator()), statupParams.GetAllocator());

	ot::ServiceInitData iniData = _session.createServiceInitData(newDebugService.getServiceID());
	statupParams.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(iniData, statupParams.GetAllocator()), statupParams.GetAllocator());
	
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
		_session.serviceDisconnected(newDebugService.getServiceID(), false, m_debugPortManager);

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

	OT_LOG_D("Debug service params written to file { " + newDebugService.debugLogString(_session.getID()) + ", \"File\": \"" + path + "\" }");

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
		checkCommandDoc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_session.createServiceInitData(relay.getServiceID()), checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());

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

		ot::ServiceInitData iniData = _session.createServiceInitData(relay.getServiceID());

		if (!m_gds.startRelayService(iniData, newServiceUrl, newWebsocketUrl)) {
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

void SessionService::startWorkerThreads() {
	if (m_workerRunning) {
		return;
	}

	m_workerRunning = true;

	OTAssert(!m_shutdownWorkerThread, "Shutdown worker thread already running");
	m_shutdownWorkerThread = new std::thread(&SessionService::shutdownWorker, this);
}

void SessionService::stopWorkerThreads() {
	if (!m_workerRunning) {
		return;
	}

	OTAssertNullptr(m_shutdownWorkerThread);

	m_workerRunning = false;
	
	if (m_shutdownWorkerThread->joinable()) {
		m_shutdownWorkerThread->join();
	}
	delete m_shutdownWorkerThread;
	m_shutdownWorkerThread = nullptr;
}

void SessionService::shutdownWorker() {
	while (m_workerRunning) {
		bool matched = this->checkShuttingDown();
		matched |= this->checkShutdownCompleted();

		if (!matched) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	OT_LOG_D("Shutdown session worker finished");
}

bool SessionService::checkShuttingDown() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_shutdownQueue.empty()) {
		return false;
	}

	std::pair<std::string, bool> shutdownInfo = std::move(m_shutdownQueue.front());
	m_shutdownQueue.pop_front();

	auto it = m_sessions.find(shutdownInfo.first);
	if (it == m_sessions.end()) {
		OT_LOG_E("Session not found \"" + shutdownInfo.first + "\"");
		return false;
	}

	// Grab session and shut it down
	Session& session = it->second;
	session.shutdownSession(ot::invalidServiceID, shutdownInfo.second, m_debugPortManager);

	// If all services are in debug mode we can remove the session immediately
	if (!session.hasShuttingDownServices()) {
		m_shutdownCompletedQueue.push_back(std::move(shutdownInfo.first));
	}

	return true;
}

bool SessionService::checkShutdownCompleted() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_shutdownCompletedQueue.empty()) {
		return false;
	}

	std::string sessionID = std::move(m_shutdownCompletedQueue.front());
	m_shutdownCompletedQueue.pop_front();

	auto it = m_sessions.find(sessionID);
	if (it == m_sessions.end()) {
		OT_LOG_E("Session not found \"" + sessionID + "\"");
		return false;
	}

	// Notify the GDS and GSS about the completed shutdown
	m_gss.notifySessionShutdownCompleted(sessionID);

	// Remove the session
	m_sessions.erase(it);

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

ot::ReturnMessage SessionService::handleGetAuthURL() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_gss.isConnected()) {
		return ot::ReturnMessage(ot::ReturnMessage::Ok, m_gss.getRegistrationResult().getAuthURL());
	}
	else {
		OT_LOG_E("There is no active connection to the GSS");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No connection to global session service");
	}
}

ot::ReturnMessage SessionService::handleGetGlobalServicesURL(ot::JsonDocument& _commandDoc) {
	// The info message is only present if the request is done by the UI frontend upon startup
	if (_commandDoc.HasMember(OT_ACTION_PARAM_MESSAGE)) {
		if (ot::json::getString(_commandDoc, OT_ACTION_PARAM_MESSAGE) == OT_INFO_MESSAGE_LogIn) {
			return ot::ReturnMessage(ot::ReturnMessage::Failed, "Invalid global session service URL");
		}
	}

	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_gss.isConnected()) {
		GSSRegistrationInfo info = m_gss.getRegistrationResult();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_PARAM_SERVICE_DBURL, ot::JsonString(info.getDataBaseURL(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_AUTHURL, ot::JsonString(info.getAuthURL(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_GDSURL, ot::JsonString(m_gds.getServiceUrl(), doc.GetAllocator()), doc.GetAllocator());

		return ot::ReturnMessage(ot::ReturnMessage::Ok, doc.toJson());
	}
	else {
		OT_LOG_E("There is no active connection to the GSS");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No connection to global session service");
	}
}

ot::ReturnMessage SessionService::handleCreateNewSession(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_gds.isConnected()) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No global directory service connected");
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
	bool shouldRunRelayService = ot::json::getBool(_commandDoc, OT_ACTION_PARAM_START_RELAY);

	// Optional params
	bool serviceInitiallyHidden = false;
	if (_commandDoc.HasMember(OT_ACTION_PARAM_Hidden)) {
		serviceInitiallyHidden = ot::json::getBool(_commandDoc, OT_ACTION_PARAM_Hidden);
	}

	// Notify GSS that the session request was received (confirm session)
	if (!m_gss.confirmSession(sessionID, userName)) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to confirm session at GSS");
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

	ot::serviceID_t creatingServiceID = ot::invalidServiceID;

	// Create response document
	ot::JsonDocument responseDoc;

	// Create a new service and store its information
	if (shouldRunRelayService) {
		OT_LOG_D("Relay service requested by session creator");

		Service& relayService = this->runRelayService(newSession, serviceName, serviceType);
		relayService.setRequested(true);
		relayService.setAlive(false);
		relayService.setHidden(serviceInitiallyHidden);

		creatingServiceID = relayService.getServiceID();

		OT_LOG_D("Relay service started { " + relayService.debugLogString(sessionID) + " }");

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
		requestingService.setHidden(serviceInitiallyHidden);

		creatingServiceID = requestingService.getServiceID();
	}

	if (creatingServiceID == ot::invalidServiceID) {
		OT_LOG_E("Failed to create the initial service for the session");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to create the initial service for the session");
	}

	OT_LOG_D("Running mandatory services");
	if (!this->runMandatoryServices(newSession)) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to start mandatory services");
	}

	// Add response information
	if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
		OT_LOG_D("Adding ToolBar Tab order to the response document");
		responseDoc.AddMember(OT_ACTION_PARAM_UI_ToolBarTabOrder, ot::JsonArray(newSession.getToolBarTabOrder(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}

	responseDoc.AddMember(OT_ACTION_PARAM_GlobalLoggerUrl, ot::JsonString(ot::ServiceLogNotifier::instance().loggingServiceURL(), responseDoc.GetAllocator()), responseDoc.GetAllocator());

	ot::JsonArray logData;
	ot::addLogFlagsToJsonArray(ot::LogDispatcher::instance().getLogFlags(), logData, responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_LogFlags, logData, responseDoc.GetAllocator());

	// Finally store the session and start its health check
	Session& addedSession = m_sessions.insert_or_assign(newSession.getID(), std::move(newSession)).first->second;

	responseDoc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(addedSession.createServiceInitData(creatingServiceID), responseDoc.GetAllocator()), responseDoc.GetAllocator());

	addedSession.startHealthCheck();

	return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDoc.toJson());
}

std::string SessionService::handleGetSystemInformation() {
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

std::string SessionService::handleGetMandatoryServices() {
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

ot::ReturnMessage SessionService::handleConfirmService(ot::JsonDocument& _commandDoc) {
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
	
	ot::JsonArray logArr;
	ot::addLogFlagsToJsonArray(ot::LogDispatcher::instance().getLogFlags(), logArr, response.GetAllocator());
	response.AddMember(OT_ACTION_PARAM_LogFlags, logArr, response.GetAllocator());
	
	// If all services are ready, add services information
	if (!theSession.hasRequestedServices(serviceID)) {
		ot::ServiceRunData runData = theSession.createServiceRunData(serviceID);
		response.AddMember(OT_ACTION_PARAM_RunData, ot::JsonObject(runData, response.GetAllocator()), response.GetAllocator());

		theSession.sendRunCommand();
	}

	return ot::ReturnMessage(ot::ReturnMessage::Ok, response.toJson());
}

ot::ReturnMessage SessionService::handleShowService(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	// Required params
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	ot::serviceID_t serviceID = static_cast<ot::serviceID_t>(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));

	Session& theSession = this->getSession(sessionID);
	theSession.showService(serviceID);

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage SessionService::handleGetSessionExists(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	auto itm = m_sessions.find(sessionID);
	if (itm == m_sessions.end()) {
		return ot::ReturnMessage::False;
	}
	else {
		return ot::ReturnMessage::True;
	}
}

void SessionService::handleServiceClosing(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	
	Session& theSession = this->getSession(sessionID);
	theSession.serviceDisconnected(serviceID, true, m_debugPortManager);
}

ot::ReturnMessage SessionService::handleShutdownSession(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	
	Session& session = this->getSession(sessionID);
	session.prepareSessionForShutdown(serviceID, m_debugPortManager);

	m_gds.notifySessionShuttingDown(sessionID);

	m_shutdownQueue.push_back(std::make_pair(sessionID, false));
	this->startWorkerThreads();

	return ot::ReturnMessage::Ok;
}

void SessionService::handleServiceFailure(ot::JsonDocument& _commandDoc) {
	// Get information of the service that failed
	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));

	OT_LOG_E("Service failure reported { \"ServiceID\": " + std::to_string(serviceID) + ", \"SessionID\": \"" + sessionID + "\" }");

	this->serviceFailure(sessionID, serviceID);
}

void SessionService::handleServiceShutdownCompleted(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	ot::serviceID_t serviceID(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));

	Session& theSession = this->getSession(sessionID);
	theSession.setServiceShutdownCompleted(serviceID, m_debugPortManager);

	if (!theSession.hasShuttingDownServices()) {
		m_shutdownCompletedQueue.push_back(sessionID);
		this->startWorkerThreads();
	}
}

ot::ReturnMessage SessionService::handleEnableServiceDebug(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	
	if (!this->hasMandatoryService(serviceName)) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "The specified service is not contained in the mandatory services list");
	}
	else {
		m_debugServices.insert(serviceName);
		OT_LOG_D("Enabled the debug mode for the service \"" + serviceName + "\"");
		return ot::ReturnMessage::Ok;
	}
}

ot::ReturnMessage SessionService::handleDisableServiceDebug(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string serviceName = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SERVICE_NAME);
	auto it = m_debugServices.find(serviceName);
	if (it != m_debugServices.end()) {
		m_debugServices.erase(it);
		OT_LOG_D("Disabled the debug mode for the service \"" + serviceName + "\"");
		return ot::ReturnMessage::Ok;
	}
	else {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "The specified service was not in debug mode");
	}
}

std::string SessionService::handleGetDebugInformation() {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::LSSDebugInfo info;
	info.setWorkerRunning(m_workerRunning);
	info.setURL(m_url);
	info.setID(m_id);

	info.setGSSUrl(m_gss.getServiceUrl());
	info.setGSSConnected(m_gss.isConnected());

	info.setGDSUrl(m_gds.getServiceUrl());
	info.setGDSConnected(m_gds.isConnected());

	info.setAuthUrl(m_authUrl);
	info.setDataBaseUrl(m_dataBaseUrl);
	info.setLMSUrl(m_lmsUrl);

	info.setDebugServices(std::list<std::string>(m_debugServices.begin(), m_debugServices.end()));
	
	for (const auto& mandatory : m_mandatoryServicesMap) {
		std::list<std::string> serviceNames;
		for (const ot::ServiceBase& serviceInfo : mandatory.second) {
			serviceNames.push_back(serviceInfo.getServiceName());
		}
		info.addMandatoryService(mandatory.first, serviceNames);
	}

	info.setShutdownQueue(m_shutdownQueue);
	info.setShutdownCompletedQueue(m_shutdownCompletedQueue);

	info.setBlockedPorts(m_debugPortManager.getBlockedPorts());

	for (auto& session : m_sessions) {
		ot::LSSDebugInfo::SessionInfo sessionInfo;
		session.second.getDebugInfo(sessionInfo);
		info.addSession(std::move(sessionInfo));
	}

	return info.toJson();
}

ot::ReturnMessage SessionService::handleCheckStartupCompleted(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string sessionID(ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID));
	ot::serviceID_t serviceID(static_cast<ot::serviceID_t>(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID)));

	Session& session = this->getSession(sessionID);

	if (session.hasRequestedServices(serviceID)) {
		return ot::ReturnMessage::False;
	}
	else {
		return ot::ReturnMessage::True;
	}
}

ot::ReturnMessage SessionService::handleAddMandatoryService(ot::JsonDocument& _commandDoc) {
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
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "The service \"" + serviceName + "\" is already in the mandatory services list for the session type \"" + sessionType + "\"");
	}
	else {
		ot::ServiceBase info(serviceName, serviceType);
		data->push_back(std::move(info));

		OT_LOG_D("Mandatory service for project type \"" + sessionType + "\": Added service \"" + serviceName + "\"");
		
		return ot::ReturnMessage::Ok;
	}
}

ot::ReturnMessage SessionService::handleRegisterNewGlobalDirectoryService(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string serviceURL(ot::json::getString(_commandDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL));

	if (m_gds.connect(serviceURL, false)) {
		m_gss.setGDSUrl(serviceURL);
		return ot::ReturnMessage::Ok;
	}
	else {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to connect to GDS at \"" + serviceURL + "\"");
	}
}

ot::ReturnMessage SessionService::handleRegisterNewLibraryManagementService(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_lmsUrl = ot::json::getString(_commandDoc, OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewLibraryManagementService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL, ot::JsonString(m_lmsUrl, doc.GetAllocator()), doc.GetAllocator());
	const std::string message = doc.toJson();

	// Notify all sessions about the new LMS
	for (auto& session : m_sessions) {
		session.second.sendBroadcast(ot::invalidServiceID, message);
	}
	
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage SessionService::handleServiceStartupFailed(ot::JsonDocument& _commandDoc) {
	// Get information of the service that failed
	ot::serviceID_t serviceID = static_cast<ot::serviceID_t>(ot::json::getUInt(_commandDoc, OT_ACTION_PARAM_SERVICE_ID));
	std::string sessionID = ot::json::getString(_commandDoc, OT_ACTION_PARAM_SESSION_ID);
	
	OT_LOG_W("Service startup failed received { \"ServiceID\": " + std::to_string(serviceID) + ", \"SessionID\": \"" + sessionID + "\" }");

	this->serviceFailure(sessionID, serviceID);

	return ot::ReturnMessage::Ok;
}

void SessionService::handleSetGlobalLogFlags(ot::JsonDocument& _commandDoc) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::LogFlags flags = ot::logFlagsFromJsonArray(ot::json::getArray(_commandDoc, OT_ACTION_PARAM_Flags));

	// Update existing session services
	this->updateLogMode(flags);
}
