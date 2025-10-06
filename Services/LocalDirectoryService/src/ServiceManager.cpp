//! @file ServiceManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Service.h"
#include "Configuration.h"
#include "ServiceManager.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTSystem/PortManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// std header
#include <thread>
#include <chrono>

ServiceManager::ServiceManager()
	: m_workerRunning(false), m_threadServiceStarter(nullptr), m_threadServiceInitializer(nullptr), m_threadHealthCheck(nullptr),
	m_threadServiceStopper(nullptr), m_threadHealthCheckFail(nullptr), m_serviceCheckAliveFrequency(1)
{
	
}

ServiceManager::~ServiceManager() {
	this->stopThreads();
}

void ServiceManager::getDebugInformation(ot::LDSDebugInfo& _info) {
	_info.setUsedPorts(m_portManager.getBlockedPorts());
	_info.setServicesIPAddress(m_servicesIpAddress);
	_info.setLastError(m_lastError);

	_info.setServiceCheckAliveFrequency(m_serviceCheckAliveFrequency.count());
	_info.setWorkerRunning(m_workerRunning);

	{
		std::lock_guard<std::mutex> lock(m_mutexRequestedServices);
		for (const RequestedService& s : m_requestedServices) {
			_info.addRequestedService(this->toDebugInfo(s));
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_mutexInitializingServices);
		for (const Service& s : m_initializingServices) {
			_info.addInitializingService(this->toDebugInfo(s));
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_mutexServices);
		for (const auto& s : m_sessions) {
			std::list<ot::LDSDebugInfo::ServiceInfo> services;
			for (const Service& service : s.second) {
				services.push_back(this->toDebugInfo(service));
			}
			ot::LDSDebugInfo::SessionInfo sessionInfo;
			sessionInfo.lssUrl = s.first.getSessionServiceURL();
			sessionInfo.sessionID = s.first.getId();
			_info.addAliveSession(std::move(sessionInfo), std::move(services));
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_mutexFailedServices);
		for (const auto& s : m_failedServices) {
			_info.addFailedService(this->toDebugInfo(s.second));
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_mutexStoppingServices);
		for (const Service& s : m_newStoppingServices) {
			_info.addNewStoppingService(this->toDebugInfo(s));
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_mutexCurrentStoppingServices);
		for (const Service& s : m_currentStoppingServices) {
			_info.addStoppingService(this->toDebugInfo(s));
		}
	}
}

void ServiceManager::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) {
	// Info
	_object.AddMember("Services.IP", ot::JsonString(m_servicesIpAddress, _allocator), _allocator);
	_object.AddMember("LastError", ot::JsonString(m_lastError, _allocator), _allocator);
	_object.AddMember("Site.ID", ot::JsonString(m_siteID, _allocator), _allocator);
	bool workerRunning = m_workerRunning;
	_object.AddMember("ThreadsRunning", workerRunning, _allocator);
	
	ot::JsonObject portManagerObj;
	ot::JsonArray portRangesArr;
	for (const auto& range : m_portManager.getPortRanges()) {
		ot::JsonObject rangeObj;
		rangeObj.AddMember("Min", range.first, _allocator);
		rangeObj.AddMember("Max", range.second, _allocator);
		portRangesArr.PushBack(rangeObj, _allocator);
	}
	portManagerObj.AddMember("PortRanges", portRangesArr, _allocator);
	portManagerObj.AddMember("BlockedPorts", ot::JsonArray(m_portManager.getBlockedPorts(), _allocator), _allocator);
	_object.AddMember("DebugPortManager", portManagerObj, _allocator);

	// Stopping Services
	ot::JsonArray newStoppingArray;
	{
		std::lock_guard<std::mutex> stoppingLock(m_mutexStoppingServices);
		for (const Service& s : m_newStoppingServices) {
			ot::JsonObject stoppingObj;
			s.addToJsonObject(stoppingObj, _allocator);
			newStoppingArray.PushBack(stoppingObj, _allocator);
		}
	}
	_object.AddMember("NewStoppingServices", newStoppingArray, _allocator);

	ot::JsonArray currentStoppingArray;
	{
		std::lock_guard<std::mutex> currentStoppingLock(m_mutexCurrentStoppingServices);
		for (const Service& s : m_currentStoppingServices) {
			ot::JsonObject stoppingObj;
			s.addToJsonObject(stoppingObj, _allocator);
			currentStoppingArray.PushBack(stoppingObj, _allocator);
		}
	}
	_object.AddMember("CurrentStoppingServices", currentStoppingArray, _allocator);

	// Running Services
	ot::JsonArray sessionsArray;
	{
		std::lock_guard<std::mutex> servicesLock(m_mutexServices);
		for (const auto& s : m_sessions) {
			ot::JsonArray serviceArray;
			for (const Service& service : s.second) {
				ot::JsonObject serviceObj;
				service.addToJsonObject(serviceObj, _allocator);
				serviceArray.PushBack(serviceObj, _allocator);
			}

			ot::JsonObject sessionInfoObj;
			s.first.addToJsonObject(sessionInfoObj, _allocator);

			ot::JsonObject sessionTopLvlObj;
			sessionTopLvlObj.AddMember("SessionInformation", sessionInfoObj, _allocator);
			sessionTopLvlObj.AddMember("Services", serviceArray, _allocator);

			sessionsArray.PushBack(sessionTopLvlObj, _allocator);
		}
	}
	_object.AddMember("AliveSessions", sessionsArray, _allocator);

	// Initializing Services
	ot::JsonArray iniArray;
	{
		std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);
		for (const Service& s : m_initializingServices) {
			ot::JsonObject obj;
			s.addToJsonObject(obj, _allocator);
			iniArray.PushBack(obj, _allocator);
		}
	}
	_object.AddMember("InitializingServices", iniArray, _allocator);

	// Startng Services
	ot::JsonArray startArray;
	{
		std::lock_guard<std::mutex> requestLock(m_mutexRequestedServices);
		for (const RequestedService& s : m_requestedServices) {
			ot::JsonObject obj;
			s.addToJsonObject(obj, _allocator);
			startArray.PushBack(obj, _allocator);
		}
	}
	_object.AddMember("RequestedServices", startArray, _allocator);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Service control

ServiceManager::RequestResult ServiceManager::requestStartService(const ot::ServiceInitData& _serviceInformation) {
	// Check if the service is supported
	auto supportedService = Configuration::instance().getSupportedService(_serviceInformation.getServiceName());
	if (!supportedService.has_value()) {
		m_lastError = "The service \"" + _serviceInformation.getServiceName() + "\" is not supported by this Local Directory Service";
		OT_LOG_E(m_lastError);
		return RequestResult::FailedOnStart;
	}

	std::lock_guard<std::mutex> lock(m_mutexRequestedServices);
	
	OT_LOG_D("Service start requested { "
		"\"ServiceID\": " + std::to_string(_serviceInformation.getServiceID()) + 
		", \"Name\": \"" + _serviceInformation.getServiceName() + "\""
		", \"Type\": \"" + _serviceInformation.getServiceType() + "\""
		", \"SessionID\": \"" + _serviceInformation.getSessionID() + "\""
		", \"LssUrl\": \"" + _serviceInformation.getSessionServiceURL() + "\""
		" }");

	// Parallel thread checks frequently if something was added. Thread starts the next service in the queue
	m_requestedServices.push_back(RequestedService(_serviceInformation, supportedService.value()));

	this->runThreads();

	return RequestResult::Success;
}

ServiceManager::RequestResult ServiceManager::requestStartRelayService(const ot::ServiceInitData& _serviceInformation, std::string& _relayServiceURL, std::string& _websocketUrl) {
	auto supportedService = Configuration::instance().getSupportedService(_serviceInformation.getServiceName());
	if (!supportedService.has_value()) {
		m_lastError = "The service \"" + _serviceInformation.getServiceName() + "\" is not supported by this Local Directory Service";
		OT_LOG_E(m_lastError);
		return RequestResult::FailedOnStart;
	}

	std::lock_guard<std::mutex> requestLock(m_mutexRequestedServices);
	
	OT_LOG_D("Service start requested { "
		"\"ServiceID\": " + std::to_string(_serviceInformation.getServiceID()) +
		", \"Name\": \"" + _serviceInformation.getServiceName() + "\""
		", \"Type\": \"" + _serviceInformation.getServiceType() + "\""
		", \"SessionID\": \"" + _serviceInformation.getSessionID() + "\""
		", \"LssUrl\": \"" + _serviceInformation.getSessionServiceURL() + "\""
		" }");

	Service newService(this, RequestedService(_serviceInformation, supportedService.value()));

	// Attempt to start service

	ot::RunResult result = newService.run(m_servicesIpAddress, m_portManager.determineAndBlockAvailablePort(), m_portManager.determineAndBlockAvailablePort());
	if (!result.isOk()) {
		m_portManager.freePort(newService.getPort());
		m_portManager.freePort(newService.getWebsocketPort());

		OT_LOG_E("Service start failed { \"ErrorCode\": " + std::to_string(result.getErrorCode()) + ", \"ErrorMessage\": \"" + result.getErrorMessage() + 
			"\", " + logInfo(newService) + " }");
		
		return RequestResult::FailedOnStart;
	}

	// The relay could be started, now ensure its alive
	OT_LOG_D("Relay started successfully. Now checking if alive");
	ot::JsonDocument checkCommandDoc;
	checkCommandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CheckRelayStartupCompleted, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
	checkCommandDoc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_serviceInformation, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
	std::string checkCommandString = checkCommandDoc.toJson();

	int attempt = 0;
	bool checkOk = false;
	std::string errorMessage;

	OT_LOG_D("Checking relay startup completed for relay service at \"" + newService.getUrl() + "\"");

	for (int attempt = 0; attempt < 3; attempt++) {
		std::string responseStr;
		if (ot::msg::send("", newService.getUrl(), ot::EXECUTE, checkCommandString, responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
			if (response == ot::ReturnMessage::True) {
				checkOk = true;
				break;
			}
			else if (response == ot::ReturnMessage::False) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			else {
				OT_LOG_E("Failed to check relay startup completed { \"Response\": \"" + response.getWhat() + "\", " + logInfo(newService) + " }");
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}

			// Ensure process is still running
			ot::RunResult result = newService.checkAlive();
			if (!result.isOk()) {
				// It is possible that the port was blocked. What happened: The process started but then died.
				OT_LOG_E("Relay service died while trying to check if it is alive. { \"ExitCode\": " + std::to_string(result.getErrorCode()) + 
					", \"ExitMessage\": \"" + result.getErrorMessage() + "\", " + logInfo(newService) + " }");
				break;
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}

	// Process started and we tried to ping it repeatedly but the service was not responsive. Consider it dead. 
	if (!checkOk) {
		// Service start failed
		OT_LOG_E("Relay Service check alive failed { " + logInfo(newService) + " }");
		ot::RunResult result = newService.shutdown();
		OT_LOG_E("Shutting relay service down with error { \"ErrorCode\": " + std::to_string(result.getErrorCode()) + 
			", \"ErrorMessage\": \"" + result.getErrorMessage() + "\", " + logInfo(newService) + " }");

		return RequestResult::FailedOnPing;
	}

	OT_LOG_D("Verified that relay service is alive");

	_relayServiceURL = newService.getUrl();
	_websocketUrl = newService.getWebsocketUrl();

	// Store information
	std::lock_guard<std::mutex> sessionLock(m_mutexServices);
	sessionServices(_serviceInformation).push_back(std::move(newService));

	return RequestResult::Success;
}

void ServiceManager::sessionClosing(const std::string& _sessionID) {
	this->cleanUpRequestedList(_sessionID);
	this->cleanUpIniList(_sessionID);

	std::lock_guard<std::mutex> servicesLock(m_mutexServices);
	for (auto& session : m_sessions) {
		if (session.first.getId() == _sessionID) {
			// Mark all alive services in the given session as stopping
			for (auto& service : session.second) {
				// Set the service to shutting down
				service.setShuttingDown(true);
			}
		}
	}
}

void ServiceManager::sessionClosed(const std::string& _sessionID) {
	this->cleanUp(_sessionID);
}

void ServiceManager::serviceDisconnected(const std::string& _sessionID, ot::serviceID_t _serviceID) {
	// Clean up start requests
	{
		std::lock_guard<std::mutex> requestedLock(m_mutexRequestedServices);
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); ) {
			if (it->getInitData().getSessionID() == _sessionID && it->getInitData().getServiceID() == _serviceID) {
				it = m_requestedServices.erase(it);
			}
			else {
				it++;
			}
		}
	}

	// Clean up initializing services
	{
		std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);
		for (auto it = m_initializingServices.begin(); it != m_initializingServices.end(); ) {
			if (it->getInfo().getSessionID() == _sessionID && it->getInfo().getServiceID() == _serviceID) {
				std::lock_guard<std::mutex> stoppingLock(m_mutexStoppingServices);
				m_newStoppingServices.push_back(std::move(*it));
				it = m_initializingServices.erase(it);
			}
			else {
				it++;
			}
		}
	}

	// Clean up the current services
	std::lock_guard<std::mutex> servicesLock(m_mutexServices);
	for (auto& s : m_sessions) {
		if (s.first.getId() == _sessionID) {
			for (auto it = s.second.begin(); it != s.second.end(); ) {
				if (it->getInfo().getServiceID() == _serviceID) {
					std::lock_guard<std::mutex> stoppingLock(m_mutexStoppingServices);
					m_newStoppingServices.push_back(std::move(*it));
					it = s.second.erase(it);
				}
				else {
					it++;
				}
			}
		}
	}
}

void ServiceManager::addPortRange(ot::port_t _start, ot::port_t _end) {
	m_portManager.addPortRange(_start, _end);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ServiceManager::getSessionInformation(ot::JsonArray& _sessionInfo, ot::JsonAllocator& _allocator) {
	for (auto& session : m_sessions) {

		ot::JsonArray servicesArr;

		for (Service& service : session.second) {
			ot::JsonObject serviceObj;
			
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_ID, service.getInfo().getServiceID(), _allocator);
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.getInfo().getServiceName(), _allocator), _allocator);
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getInfo().getServiceType(), _allocator), _allocator);
			serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(service.getUrl(), _allocator), _allocator);

			servicesArr.PushBack(serviceObj, _allocator);
		}

		ot::JsonObject sessionObj;

		sessionObj.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(session.first.getId(), _allocator), _allocator);
		sessionObj.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, servicesArr, _allocator);

		_sessionInfo.PushBack(sessionObj, _allocator);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ServiceManager::runThreads() {
	if (m_workerRunning) {
		return;
	}

	OTAssert(m_threadServiceStarter == nullptr, "Service starter thread is already running");
	OTAssert(m_threadServiceInitializer == nullptr, "Service initializer thread is already running");
	OTAssert(m_threadHealthCheck == nullptr, "Service health check thread is already running");
	OTAssert(m_threadServiceStopper == nullptr, "Service stopper thread is already running");
	OTAssert(m_threadHealthCheckFail == nullptr, "Service health check fail thread is already running");

	m_workerRunning = true;

	m_threadServiceStarter = new std::thread(&ServiceManager::workerServiceStarter, this);
	m_threadServiceInitializer = new std::thread(&ServiceManager::workerServiceInitializer, this);
	m_threadHealthCheck = new std::thread(&ServiceManager::workerHealthCheck, this);
	m_threadServiceStopper = new std::thread(&ServiceManager::workerServiceStopper, this);
	m_threadHealthCheckFail = new std::thread(&ServiceManager::workerHealthCheckFail, this);
}

void ServiceManager::stopThreads() {
	if (!m_workerRunning) {
		return;
	}

	OTAssertNullptr(m_threadServiceStarter);
	OTAssertNullptr(m_threadServiceInitializer);
	OTAssertNullptr(m_threadHealthCheck);
	OTAssertNullptr(m_threadServiceStopper);
	OTAssertNullptr(m_threadHealthCheckFail);

	// Stop threads
	m_workerRunning = false;

	if (m_threadServiceStarter->joinable()) {
		m_threadServiceStarter->join();
	}
	delete m_threadServiceStarter;
	m_threadServiceStarter = nullptr;

	if (m_threadServiceInitializer->joinable()) {
		m_threadServiceInitializer->join();
	}
	delete m_threadServiceInitializer;
	m_threadServiceInitializer = nullptr;

	if (m_threadHealthCheck->joinable()) {
		m_threadHealthCheck->join();
	}
	delete m_threadHealthCheck;
	m_threadHealthCheck = nullptr;

	if (m_threadServiceStopper->joinable()) {
		m_threadServiceStopper->join();
	}
	delete m_threadServiceStopper;
	m_threadServiceStopper = nullptr;

	if (m_threadHealthCheckFail->joinable()) {
		m_threadHealthCheckFail->join();
	}
	delete m_threadHealthCheckFail;
	m_threadHealthCheckFail = nullptr;
}

void ServiceManager::serviceStartFailed(const ot::ServiceInitData& _serviceInformation) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceStartupFailed, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _serviceInformation.getServiceID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInformation.getSessionID(), doc.GetAllocator()), doc.GetAllocator());

	// Fire message
	std::string response;
	ot::msg::send("", _serviceInformation.getSessionServiceURL(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}

void ServiceManager::sendInitializeMessage(Service&& _info) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_info.getInfo(), doc.GetAllocator()), doc.GetAllocator());
	
	// Send message
	std::string responseStr;
	if (!ot::msg::send("", _info.getUrl(), ot::EXECUTE, doc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_W("Failed to send initialize message to service { \"ServiceID\": " + std::to_string(_info.getInfo().getServiceID()) + ", \"Name\": \"" + _info.getInfo().getServiceName() + "\", \"Type\": \"" +
			_info.getInfo().getServiceType() + "\", \"Url\": \"" + _info.getUrl() + "\" }");

		this->serviceInitializeFailed(std::move(_info));
		return;
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

	if (response != ot::ReturnMessage::Ok) {
		OT_LOG_W("Service initialize failed for service { \"ServiceID\": " + std::to_string(_info.getInfo().getServiceID()) + ", \"Name\": \"" + _info.getInfo().getServiceName() + "\", \"Type\": \"" +
			_info.getInfo().getServiceType() + "\", \"Url\": \"" + _info.getUrl() + "\" } with error message: " + response.getWhat());

		this->serviceInitializeFailed(std::move(_info));
	}
	else {
		// Service was pinged, did its initialization call, so now we move it to alive (doing health check)
		std::lock_guard<std::mutex> sessionLock(m_mutexServices);
		this->sessionServices(_info.getInfo()).push_back(std::move(_info));
	}
}

void ServiceManager::serviceInitializeFailed(Service&& _info) {
	std::lock_guard<std::mutex> lock(m_mutexRequestedServices);
	m_requestedServices.push_back(RequestedService(std::move(_info.getInfo()), std::move(_info.getStartupData())));
}

std::list<Service>& ServiceManager::sessionServices(const SessionInformation& _sessionInformation) {
	auto it = m_sessions.find(_sessionInformation);
	if (it == m_sessions.end()) {
		return m_sessions.insert_or_assign(_sessionInformation, std::list<Service>()).first->second;
	}
	else {
		return it->second;
	}
}

bool ServiceManager::restartServiceAfterCrash(Service& _service) {
	// Currently services are not restarted after a crash.
	// A service crash will lead to a session shutdow of the session the service was running in.
	return false;
}

void ServiceManager::notifyServiceShutdownCompleted(const Service& _service) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShutdownCompleted, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _service.getInfo().getServiceID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_service.getInfo().getSessionID(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", _service.getInfo().getSessionServiceURL(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send shutdown completed message to session service { " + logInfo(_service) + " }");
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invalid shutdown completed response from session service { \"Response\": \"" + response + "\", " + logInfo(_service) + " }");
	}
}

void ServiceManager::notifySessionEmergencyShutdown(const Service& _crashedService) {
	// Clean up startup requests
	ot::ServiceInitData serviceInfo = _crashedService.getInfo();

	OT_LOG_E("Preparing session emergency shutdown. Reason: Service crashed { " + logInfo(_crashedService) + " }");

	// Notify session service about the crash
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceFailure, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, serviceInfo.getServiceID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(serviceInfo.getSessionID(), doc.GetAllocator()), doc.GetAllocator());
	
	// Fire message
	std::string response;
	ot::msg::send("", serviceInfo.getSessionServiceURL(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}

std::string ServiceManager::logInfo(const Service& _service) const {
	return logInfo(_service.getInfo()) + ", \"ServiceUrl\": \"" + _service.getUrl() + "\"" + (_service.getWebsocketUrl().empty() ? std::string() : ", \"WebsocketUrl\": " + _service.getWebsocketUrl() + "\"");
}

std::string ServiceManager::logInfo(const RequestedService& _service) const {
	return logInfo(_service.getInitData());
}

std::string ServiceManager::logInfo(const ot::ServiceInitData& _serviceInfo) const {
	return "\"ServiceID\": " + std::to_string(_serviceInfo.getServiceID()) + ", \"ServiceName\": \"" + _serviceInfo.getServiceName() + "\", \"ServiceType\": \"" +
		_serviceInfo.getServiceType() + "\", \"SessionID\": \"" + _serviceInfo.getSessionID() + "\"";
}

ot::LDSDebugInfo::ServiceInfo ServiceManager::toDebugInfo(const ot::ServiceInitData& _serviceInfo) const {
	ot::LDSDebugInfo::ServiceInfo info;

	info.id = _serviceInfo.getServiceID();
	info.name = _serviceInfo.getServiceName();
	info.type = _serviceInfo.getServiceType();
	info.sessionID = _serviceInfo.getSessionID();
	info.lssUrl = _serviceInfo.getSessionServiceURL();

	return info;
}

ot::LDSDebugInfo::ServiceInfo ServiceManager::toDebugInfo(const Service& _service) const {
	ot::LDSDebugInfo::ServiceInfo info = toDebugInfo(_service.getInfo());

	info.url = _service.getUrl();
	info.websocketUrl = _service.getWebsocketUrl();
	info.isShuttingDown = _service.isShuttingDown();

	this->addDebugInfo(_service.getStartupData(), info);

	return info;
}

ot::LDSDebugInfo::ServiceInfo ServiceManager::toDebugInfo(const RequestedService& _service) const {
	ot::LDSDebugInfo::ServiceInfo info = toDebugInfo(_service.getInitData());
	this->addDebugInfo(_service.getStartupData(), info);

	return info;
}

void ServiceManager::addDebugInfo(const ServiceStartupData& _startupData, ot::LDSDebugInfo::ServiceInfo& _info) const {
	_info.startCounter = _startupData.getStartCounter();
	_info.iniAttempt = _startupData.getIniAttempt();
	_info.maxCrashRestarts = _startupData.getMaxCrashRestarts();
	_info.maxStartupRestarts = _startupData.getMaxStartupRestarts();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Cleanup

void ServiceManager::cleanUp(const std::string& _sessionID) {
	this->cleanUpRequestedList(_sessionID);
	this->cleanUpIniList(_sessionID);
	this->cleanUpAliveList(_sessionID);
}

void ServiceManager::cleanUpRequestedList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> reqLock(m_mutexRequestedServices);

	for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); ) {
		if (it->getInitData().getSessionID() == _sessionID) {
			OT_LOG_D("Removing requested service { " + logInfo(*it) + " }");
			it = m_requestedServices.erase(it);
		}
		else {
			it++;
		}
	}
}

void ServiceManager::cleanUpIniList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceEmergencyShutdown, doc.GetAllocator()), doc.GetAllocator());
	std::string cmd = doc.toJson();

	for (auto it = m_initializingServices.begin(); it != m_initializingServices.end(); ) {
		if (it->getInfo().getSessionID() == _sessionID) {
			// Send shutdown message to the service
			std::string response;
			if (!ot::msg::send("", it->getUrl(), ot::EXECUTE, cmd, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_EAS("Failed to send shutdown message to service { " + logInfo(*it) + " }");
			}

			std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
			m_newStoppingServices.push_back(std::move(*it));;
			it = m_initializingServices.erase(it);
		}
		else {
			it++;
		}
	}
}

void ServiceManager::cleanUpAliveList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> serviceLock(m_mutexServices);
	
	for (auto it = m_sessions.begin(); it != m_sessions.end(); ) {
		if (it->first.getId() == _sessionID) {
			std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
			m_newStoppingServices.splice(m_newStoppingServices.end(), std::move(it->second));

			it = m_sessions.erase(it);
		}
		else {
			it++;
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void ServiceManager::workerServiceStarter() {
	while (m_workerRunning) {

		// Check if a service was requested
		m_mutexRequestedServices.lock();
		
		if (m_requestedServices.empty()) {
			// Nothing to do, unlock and wait
			m_mutexRequestedServices.unlock();

			// Sleep for 100ms before next check
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else {
			// Get next requested service and remove it from list
			RequestedService info = std::move(m_requestedServices.front());
			m_requestedServices.pop_front();

			OT_LOG_D("Starting service: " + info.getInitData().getServiceName());

			// Requested services modifications completed
			m_mutexRequestedServices.unlock();
			
			// Check if max restarts is reached
			if (info.incrStartCounter() > info.getMaxStartupRestarts()) {
				OT_LOG_EAS("Maximum number of start attempts (" + std::to_string(info.getMaxStartupRestarts()) +
					") reached for service  { " + logInfo(info) + " }");
				
				// Notify and skip
				this->serviceStartFailed(info.getInitData());
				continue;
			}

			// Initialize service
			Service newService(this, std::move(info));
			
			// Attempt to start service
			ot::RunResult result = newService.run(m_servicesIpAddress, m_portManager.determineAndBlockAvailablePort());
			
			if (!result.isOk()) {
				OT_LOG_E("Service start failed { \"ErrorCode\": " + std::to_string(result.getErrorCode()) + 
					", \"ErrorMessage\": \"" + result.getErrorMessage() + "\", " + logInfo(newService) + " }");
				
				// Clean up port numbers
				m_portManager.freePort(newService.getPort());
				if (newService.getWebsocketPort() != ot::invalidPortNumber) {
					m_portManager.freePort(newService.getWebsocketPort());
				}
				
				// Service start failed
				this->serviceStartFailed(newService.getInfo());
			}
			else {
				OT_LOG_D("Service was started { "
					"\"ID\": " + std::to_string(newService.getInfo().getServiceID()) + ", \"Name\": \"" + newService.getInfo().getServiceName() + "\", \"Type\": \"" +
					newService.getInfo().getServiceType() + "\", \"Url\": \"" + newService.getUrl() + "\", \"SessionID\": \"" + newService.getInfo().getSessionID() + "\" }");

				std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);

				// Move from start to initialize
				m_initializingServices.push_back(std::move(newService));
			}
		}
	}
}

void ServiceManager::workerServiceInitializer() {
	// Create ping command once
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingCommand = pingDoc.toJson();

	while (m_workerRunning) {

		// Check if a service was requested
		m_mutexInitializingServices.lock();

		if (m_initializingServices.empty()) {
			m_mutexInitializingServices.unlock();

			// Nothing to initialize, sleep
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}
		else {
			// Get next service to initialize and remove it from the list
			Service info(std::move(m_initializingServices.front()));
			m_initializingServices.pop_front();

			m_mutexInitializingServices.unlock();

			OT_LOG_D("Initialising service: { "
				"\"ID\": " + std::to_string(info.getInfo().getServiceID()) + ", \"Name\": \"" + info.getInfo().getServiceName() + "\", \"Type\": \"" +
				info.getInfo().getServiceType() + "\", \"Url\": \"" + info.getUrl() + "\", \"SessionID\": \"" + info.getInfo().getSessionID() + "\" }");

			// Increase initialize attempt of the service
			info.incrIniAttempt();

			// Check if service executable is still running
			ot::RunResult result = info.checkAlive();

			if (result.isOk()) {
				// Service executable is 

				OT_LOG_D("Pinging service { "
					"\"ID\": " + std::to_string(info.getInfo().getServiceID()) + ", \"Name\": \"" + info.getInfo().getServiceName() + "\", \"Type\": \"" +
					info.getInfo().getServiceType() + "\", \"Url\": \"" + info.getUrl() + "\", \"SessionID\": \"" + info.getInfo().getSessionID() + "\" }");

				// Attempt to send ping
				std::string response;
				if (!ot::msg::send("", info.getUrl(), ot::EXECUTE, pingCommand, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					// Failed to ping

					OT_LOG_D("Failed to ping service (attempt " + std::to_string(info.getIniAttempt()) + "/" + std::to_string(info.getStartupData().getMaxStartupRestarts()) + "): { "
						"\"ID\": " + std::to_string(info.getInfo().getServiceID()) + ", \"Name\": \"" + info.getInfo().getServiceName() + "\", \"Type\": \"" +
						info.getInfo().getServiceType() + "\", \"Url\": \"" + info.getUrl() + "\", \"SessionID\": \"" + info.getInfo().getSessionID() + "\" }");

					if (info.getIniAttempt() > 3) {
						OT_LOG_W("Failed to ping service 3 times. Moving service back to restart { " + logInfo(info) + " }");
						this->serviceInitializeFailed(std::move(info));
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(std::move(info));
						m_mutexInitializingServices.unlock();
					}
				}
				else if (response != OT_ACTION_CMD_Ping) {
					// Invalid response

					OT_LOG_W("Invalid response from service { "
						"\"ID\": " + std::to_string(info.getInfo().getServiceID()) + ", \"Name\": \"" + info.getInfo().getServiceName() + "\", \"Type\": \"" +
						info.getInfo().getServiceType() + "\", \"Url\": \"" + info.getUrl() + "\", \"SessionID\": \"" + info.getInfo().getSessionID() + "\" }");

					if (info.getIniAttempt() > 3) {
						this->serviceInitializeFailed(std::move(info));
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(std::move(info));
						m_mutexInitializingServices.unlock();
					}
				}
				else {
					OT_LOG_D("Service ping success { "
						"\"ID\": " + std::to_string(info.getInfo().getServiceID()) + ", \"Name\": \"" + info.getInfo().getServiceName() + "\", \"Type\": \"" +
						info.getInfo().getServiceType() + "\", \"Url\": \"" + info.getUrl() + "\", \"SessionID\": \"" + info.getInfo().getSessionID() + "\" }");

					// Service alive

					this->sendInitializeMessage(std::move(info));
				}
			}
			else {
				OT_LOG_E("Service check alive failed during initialization: { \"AppErrorCode\": " + std::to_string(result.getErrorCode()) + 
					"\", \"AppErrorMessage\": " + result.getErrorMessage() + "\", " + logInfo(info) + " }");
				
				result = info.shutdown();
				OT_LOG_I("Service shutdown after crash completed with error code: " + std::to_string(result.getErrorCode()) + " and Message: " + result.getErrorMessage());

				this->serviceInitializeFailed(std::move(info));
			}
		}
	}
}

void ServiceManager::workerHealthCheck() {
	while (m_workerRunning) {
		// Lock mutex for entire health check
		{
			std::lock_guard<std::mutex> lock(m_mutexServices);
			// Itereate through every service in every session
			for (auto& session : m_sessions) {
				for (auto serviceIt = session.second.begin(); serviceIt != session.second.end(); ) {
					// Check if service crashed
					ot::RunResult result = serviceIt->checkAlive();
					if (!result.isOk()) {
						OT_LOG_D("Service is not running anymore { "
							"\"ID\": " + std::to_string(serviceIt->getInfo().getServiceID()) + ", \"Name\": \"" + serviceIt->getInfo().getServiceName() + "\", \"Type\": \"" +
							serviceIt->getInfo().getServiceType() + "\", \"Url\": \"" + serviceIt->getUrl() + "\", \"SessionID\": \"" + serviceIt->getInfo().getSessionID() + "\" }");

						std::lock_guard<std::mutex> failLock(m_mutexFailedServices);
						m_failedServices.push_back(std::make_pair(std::move(result), std::move(*serviceIt)));
						serviceIt = session.second.erase(serviceIt);
					}
					else {
						serviceIt++;
					}
				}

			} // for session
		} // mutex lock scope

		// sleep until next service is checked
		std::this_thread::sleep_for(m_serviceCheckAliveFrequency);

	}
}

void ServiceManager::workerHealthCheckFail() {
	while (m_workerRunning) {
		m_mutexFailedServices.lock();
		if (m_failedServices.empty()) {
			m_mutexFailedServices.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		else {
			auto serviceData = std::move(m_failedServices.front());
			m_failedServices.pop_front();
			m_mutexFailedServices.unlock();

			// Service died
			if (serviceData.second.isShuttingDown()) {
				// The shutdown was intentional

				OT_LOG_D("Service shutdown completed { "
					"\"ID\": " + std::to_string(serviceData.second.getInfo().getServiceID()) + ", \"Name\": \"" + serviceData.second.getInfo().getServiceName() + "\", \"Type\": \"" +
					serviceData.second.getInfo().getServiceType() + "\", \"Url\": \"" + serviceData.second.getUrl() + "\", \"SessionID\": \"" + serviceData.second.getInfo().getSessionID() + "\" }");

				// Notify LSS about shutdown completed
				this->notifyServiceShutdownCompleted(serviceData.second);

				// Remove the service from the session list
				std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
				m_newStoppingServices.push_back(std::move(serviceData.second));
			}
			else {
				OT_LOG_E("Service checkAlive failed  { \"ErrorCode\": " + std::to_string(serviceData.first.getErrorCode()) + ", "
					"\"ErrorMessage\": \"" + serviceData.first.getErrorMessage() + "\", " + logInfo(serviceData.second) + " }");

				ot::RunResult result = serviceData.second.shutdown();
				OT_LOG_E("Shutting down service with error { \"ErrorCode\": " + std::to_string(result.getErrorCode()) + ", "
					"\"ErrorMessage\": \"" + result.getErrorMessage() + "\", " + logInfo(serviceData.second) + " }");

				if (serviceData.second.getStartCounter() < serviceData.second.getStartupData().getMaxCrashRestarts()) {
					// Attempt to restart service, if successful the list did not change and we can continue the health check.
					if (this->restartServiceAfterCrash(serviceData.second)) {
						continue;
					}
				}

				// Remove all requested services that are related to the session
				this->cleanUpRequestedList(serviceData.second.getInfo().getSessionID());
				this->cleanUpIniList(serviceData.second.getInfo().getSessionID());

				// Either the restart failed or the restart counter reached its max value.
				// We notify the session service, clean up the lists and cancel the current health check.
				this->notifySessionEmergencyShutdown(serviceData.second);

				std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
				m_newStoppingServices.push_back(std::move(serviceData.second));
			}
		}
	}
}

void ServiceManager::workerServiceStopper() {
	while (m_workerRunning) {
		// Lock mutex for check
		{
			std::lock_guard<std::mutex> lock(m_mutexStoppingServices);
			m_currentStoppingServices.splice(m_currentStoppingServices.end(), std::move(m_newStoppingServices));
			m_newStoppingServices.clear();
		}
		
		{
			std::lock_guard<std::mutex> lock(m_mutexCurrentStoppingServices);
			for (auto it = m_currentStoppingServices.begin(); it != m_currentStoppingServices.end(); ) {
				ot::RunResult result = it->checkAlive();
				if (!result.isOk()) {
					result = it->shutdown();

					// This is not an error, we simply wait for the exe to die to free up the port after the complete shutdown.

					// Clean up port numbers
					m_portManager.freePort(it->getPort());
					if (it->getWebsocketPort() != ot::invalidPortNumber) {
						m_portManager.freePort(it->getWebsocketPort());
					}

					it = m_currentStoppingServices.erase(it);
				}
				else {
					it++;
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
