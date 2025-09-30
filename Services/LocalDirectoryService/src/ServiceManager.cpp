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
	: m_isShuttingDown(false), m_threadServiceStarter(nullptr), m_threadServiceInitializer(nullptr), m_threadHealthCheck(nullptr),
	m_threadServiceStopper(nullptr), m_generalWait(false), m_serviceCheckAliveFrequency(1)
{
	
}

ServiceManager::~ServiceManager() {
	m_isShuttingDown = true;
}

void ServiceManager::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) {
	// Info
	_object.AddMember("Services.IP", ot::JsonString(m_servicesIpAddress, _allocator), _allocator);
	_object.AddMember("LastError", ot::JsonString(m_lastError, _allocator), _allocator);
	_object.AddMember("Site.ID", ot::JsonString(m_siteID, _allocator), _allocator);
	_object.AddMember("IsShuttingDown", m_isShuttingDown, _allocator);
	_object.AddMember("Starter.ThreadRunning", (m_threadServiceStarter != nullptr), _allocator);
	_object.AddMember("Initializer.ThreadRunning", (m_threadServiceInitializer != nullptr), _allocator);
	_object.AddMember("HealthCheck.ThreadRunning", (m_threadHealthCheck != nullptr), _allocator);
	
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
	m_mutexStoppingServices.lock();
	ot::JsonArray stoppingArray;
	for (Service& s : m_stoppingServices) {
		ot::JsonObject stoppingObj;
		s.addToJsonObject(stoppingObj, _allocator);
		stoppingArray.PushBack(stoppingObj, _allocator);
	}
	m_mutexStoppingServices.unlock();
	_object.AddMember("StoppingServices", stoppingArray, _allocator);

	// Running Services
	m_mutexServices.lock();
	ot::JsonArray sessionsArray;
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
		_object.AddMember("SessionInformation", sessionInfoObj, _allocator);
		_object.AddMember("Services", serviceArray, _allocator);

		sessionsArray.PushBack(sessionTopLvlObj, _allocator);
	}
	m_mutexServices.unlock();
	_object.AddMember("AliveSessions", sessionsArray, _allocator);

	// Initializing Services
	m_mutexInitializingServices.lock();
	ot::JsonArray iniArray;
	for (const Service& s : m_initializingServices) {
		ot::JsonObject obj;
		s.addToJsonObject(obj, _allocator);
		iniArray.PushBack(obj, _allocator);
	}
	m_mutexInitializingServices.unlock();
	_object.AddMember("InitializingServices", iniArray, _allocator);

	// Startng Services
	m_mutexRequestedServices.lock();
	ot::JsonArray startArray;
	for (const RequestedService& s : m_requestedServices) {
		ot::JsonObject obj;
		s.addToJsonObject(obj, _allocator);
		startArray.PushBack(obj, _allocator);
	}
	m_mutexRequestedServices.unlock();
	_object.AddMember("RequestedServices", startArray, _allocator);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Service control

ServiceManager::RequestResult ServiceManager::requestStartService(const ot::ServiceInitData& _serviceInformation) {
	// Check if the service is supported
	auto supportedService = Configuration::instance().getSupportedService(_serviceInformation.getServiceName());
	if (!supportedService.has_value()) {
		OT_LOG_E("The service \"" + _serviceInformation.getServiceName() + "\" is not supported by this Local Directory Service");
		m_lastError = "The service \"" + _serviceInformation.getServiceName() + "\" is not supported by this Local Directory Service";
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
		OT_LOG_E("The service \"" + _serviceInformation.getServiceName() + "\" is not supported by this Local Directory Service");
		m_lastError = "The service \"" + _serviceInformation.getServiceName() + "\" is not supported by this Local Directory Service";
		return RequestResult::FailedOnStart;
	}

	m_mutexRequestedServices.lock();

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
		m_mutexRequestedServices.unlock();

		m_portManager.freePort(newService.getPort());
		m_portManager.freePort(newService.getWebsocketPort());

		OT_LOG_E("Service start failed with return value: " + std::to_string(result.getErrorCode()) + " and message: " + result.getErrorMessage());
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
				OT_LOG_E("Failed to check relay startup completed: " + response.getWhat());
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}

			// Ensure process is still running
			ot::RunResult result = newService.checkAlive();
			if (!result.isOk()) {
				// It is possible that the port was blocked. What happened: The process started but then died.
				OT_LOG_E("Relayservice died while trying to check if it is alive. Exit code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());
				break;
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}

	// Process started and we tried to ping it repeatedly but the service was not responsive. Consider it dead. 
	if (!checkOk) {
		m_mutexRequestedServices.unlock();

		// Service start failed
		OT_LOG_E("Relay Service check alive failed.");
		ot::RunResult result = newService.shutdown();
		OT_LOG_E("Shutting service down with error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());

		return RequestResult::FailedOnPing;
	}

	OT_LOG_D("Verified that relay service is alive");

	_relayServiceURL = newService.getUrl();
	_websocketUrl = newService.getWebsocketUrl();

	// Store information
	m_mutexRequestedServices.unlock();

	m_mutexServices.lock();
	sessionServices(_serviceInformation).push_back(std::move(newService));

	m_mutexServices.unlock();

	return RequestResult::Success;
}

void ServiceManager::sessionClosing(const std::string& _sessionID) {
	this->cleanUpSession_RequestedList(_sessionID);

	std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);

	this->cleanUpSession_IniList(_sessionID);

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
	this->cleanUpSession_RequestedList(_sessionID);

	std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
	
	this->cleanUpSession_IniList(_sessionID);
	this->cleanUpSession_AliveList(_sessionID);
}

void ServiceManager::serviceDisconnected(const std::string& _sessionID, ot::serviceID_t _serviceID) {
	// Clean up start requests
	m_mutexRequestedServices.lock();
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); it++) {
			if (it->getInitData().getSessionID() == _sessionID && it->getInitData().getServiceID() == _serviceID) {
				m_requestedServices.erase(it);
				erased = true;
				break;
			}
		}
	}
	m_mutexRequestedServices.unlock();

	// Clean up initializing services
	m_mutexInitializingServices.lock();
	m_mutexStoppingServices.lock();
	erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_initializingServices.begin(); it != m_initializingServices.end(); it++) {
			if (it->getInfo().getSessionID() == _sessionID && it->getInfo().getServiceID() == _serviceID) {
				m_stoppingServices.push_back(std::move(*m_initializingServices.erase(it)));

				erased = true;
				break;
			}
		}
	}
	m_mutexInitializingServices.unlock();

	// Clean up the current services
	m_mutexServices.lock();
	for (auto& s : m_sessions) {
		if (s.first.getId() == _sessionID) {
			for (auto it = s.second.begin(); it != s.second.end(); it++) {
				if (it->getInfo().getServiceID() == _serviceID) {
					m_stoppingServices.push_back(std::move(*it));
					s.second.erase(it);
					break;
				}
			}
			break;
		}
	}
	m_mutexStoppingServices.unlock();
	m_mutexServices.unlock();
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

void ServiceManager::runThreads(void) {
	if (m_threadServiceStarter == nullptr) {
		// Starts thread to start the front entry of services to start
		m_threadServiceStarter = new std::thread(&ServiceManager::workerServiceStarter, this);
	}

	if (m_threadServiceInitializer == nullptr) {
		// Starts thread to initialise the front entry of services to initialise
		m_threadServiceInitializer = new std::thread(&ServiceManager::workerServiceInitializer, this);
	}

	if (m_threadHealthCheck == nullptr) {
		m_threadHealthCheck = new std::thread(&ServiceManager::workerHealthCheck, this);
	}

	if (m_threadServiceStopper == nullptr) {
		m_threadServiceStopper = new std::thread(&ServiceManager::workerServiceStopper, this);
	}
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
		m_mutexServices.lock();
		this->sessionServices(_info.getInfo()).push_back(std::move(_info));
		m_mutexServices.unlock();
	}
}

void ServiceManager::serviceInitializeFailed(Service&& _info) {
	m_mutexRequestedServices.lock();
	m_requestedServices.push_back(RequestedService(std::move(_info.getInfo()), std::move(_info.getStartupData())));
	m_mutexRequestedServices.unlock();
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

bool ServiceManager::restartServiceAfterCrash(const Service& _service) {
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
		OT_LOG_E("Failed to send shutdown completed message to session service { \"ServiceID\": " + std::to_string(_service.getInfo().getServiceID()) + ", \"Name\": \"" + _service.getInfo().getServiceName() + "\", \"Type\": \"" +
			_service.getInfo().getServiceType() + "\", \"Service.Url\": \"" + _service.getUrl() + "\", \"LSS.Url\": \"" + _service.getInfo().getSessionServiceURL() + "\" }");
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invalid shutdown completed response from session service { \"ServiceID\": " + std::to_string(_service.getInfo().getServiceID()) + ", \"Name\": \"" + _service.getInfo().getServiceName() + "\", \"Type\": \"" +
			_service.getInfo().getServiceType() + "\", \"Service.Url\": \"" + _service.getUrl() + "\", \"LSS.Url\": \"" + _service.getInfo().getSessionServiceURL() +
			"\", \"Response\": \"" + response + "\" }");
	}
}

void ServiceManager::notifySessionEmergencyShutdown(const Service& _crashedService) {
	// Clean up startup requests
	std::lock_guard<std::mutex> lock(m_mutexRequestedServices);

	ot::ServiceInitData serviceInfo = _crashedService.getInfo();

	OT_LOG_E("Preparing Session Emergency Shutdown for session: \"" + serviceInfo.getSessionID() + 
		"\". Reason: Service crashed { \"ServiceID\": " + std::to_string(serviceInfo.getServiceID()) + ", \"Name\": \"" + serviceInfo.getServiceName() +
		"\", \"Type\": \"" + serviceInfo.getServiceType() + "\" }");

	bool erased = true;

	// Remove all requested services that are related to the session
	for (auto it = m_requestedServices.begin(); it != m_requestedServices.end() && !erased; ) {
		if (it->getInitData().getSessionID() == serviceInfo.getSessionID()) {
			it = m_requestedServices.erase(it);
		}
		else {
			it++;
		}
	}

	// Clean up running services
	m_sessions.erase(serviceInfo);

	// Notify session service about the crash
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceFailure, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, serviceInfo.getServiceID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(serviceInfo.getSessionID(), doc.GetAllocator()), doc.GetAllocator());
	
	// Fire message
	std::string response;
	ot::msg::send("", serviceInfo.getSessionServiceURL(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Cleanup

void ServiceManager::cleanUpSession_RequestedList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> reqLock(m_mutexRequestedServices);

	for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); ) {
		if (it->getInitData().getSessionID() == _sessionID) {
			OT_LOG_D("Handling session closed (Session: " + _sessionID + "): Removing requested service { \"ID\": " + std::to_string(it->getInitData().getServiceID()) + ", \"Name\": \"" + it->getInitData().getServiceName() + "\" }");

			it = m_requestedServices.erase(it);
		}
		else {
			it++;
		}
	}
}

void ServiceManager::cleanUpSession_IniList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShutdown, doc.GetAllocator()), doc.GetAllocator());
	std::string cmd = doc.toJson();

	for (auto it = m_initializingServices.begin(); it != m_initializingServices.end(); ) {
		if (it->getInfo().getSessionID() == _sessionID) {
			// Send shutdown message to the service
			std::string response;
			if (!ot::msg::send("", it->getUrl(), ot::EXECUTE, cmd, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_EAS("Failed to send shutdown message to service { "
					"\"ID\": " + std::to_string(it->getInfo().getServiceID()) + ", \"Name\": \"" + it->getInfo().getServiceName() + "\", \"Type\": \"" +
					it->getInfo().getServiceType() + "\", \"Url\": \"" + it->getUrl() + "\", \"SessionID\": \"" + it->getInfo().getSessionID() + "\" }");
			}

			m_stoppingServices.push_back(std::move(*it));;
			it = m_initializingServices.erase(it);
		}
		else {
			it++;
		}
	}
}

void ServiceManager::cleanUpSession_AliveList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> serviceLock(m_mutexServices);

	for (auto& s : m_sessions) {
		if (s.first.getId() == _sessionID) {
			for (auto& service : s.second) {
				m_stoppingServices.push_back(std::move(service));
			}
			m_sessions.erase(s.first);
			break;
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void ServiceManager::workerServiceStarter(void) {
	while (!m_isShuttingDown) {

		// Check if a service was requested
		m_mutexRequestedServices.lock();
		
		if (m_requestedServices.empty()) {
			// Nothing to do, unlock and wait
			m_mutexRequestedServices.unlock();

			// Sleep for 100ms before next check
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
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
				OTAssert(0, "Maximum number of start attempts reached");
				OT_LOG_E("Maximum number of start attempts (" + std::to_string(info.getMaxStartupRestarts()) +
					") reached for service  { \"ID\": " + std::to_string(info.getInitData().getServiceID()) + 
					", \"Name\": \"" + info.getInitData().getServiceName() + "\", \"Type\": \"" + info.getInitData().getServiceType() + "\" }");
				
				// Notify and skip
				this->serviceStartFailed(info.getInitData());
				continue;
			}

			// Initialize service
			Service newService(this, std::move(info));
			
			// Attempt to start service
			ot::RunResult result = newService.run(m_servicesIpAddress, m_portManager.determineAndBlockAvailablePort());
			
			if (!result.isOk()) {
				OT_LOG_E("Service start failed { \"ErrorCode\": " + std::to_string(result.getErrorCode()) + ", \"ErrorMessage\": \"" + result.getErrorMessage() + "\", "
					"\"ServiceID\": " + std::to_string(newService.getInfo().getServiceID()) + ", "
					"\"ServiceName\": \"" + newService.getInfo().getServiceName() + "\", "
					"\"ServiceType\": \"" + newService.getInfo().getServiceType() + "\", "
					"\"SessionID\": \"" + newService.getInfo().getSessionID() + "\" }"
				);
				
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

void ServiceManager::workerServiceInitializer(void) {
	// Create ping command once
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingCommand = pingDoc.toJson();

	while (!m_isShuttingDown) {

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
						OT_LOG_W("Failed to ping service 3 times. Moving service back to restart");
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
				OT_LOG_E("Service check alive failed during initialization: { "
					"\"AppErrorCode\": " + std::to_string(result.getErrorCode()) + 
					"\", \"ID\": " + std::to_string(info.getInfo().getServiceID()) + ", \"Name\": \"" + info.getInfo().getServiceName() + "\", \"Type\": \"" +
					info.getInfo().getServiceType() + "\", \"Url\": \"" + info.getUrl() + "\", \"SessionID\": \"" + info.getInfo().getSessionID() + "\" "
					"}. App Error Message: " + result.getErrorMessage());
				
				result = info.shutdown();
				OT_LOG_I("Service shutdown after crash completed with error code: " + std::to_string(result.getErrorCode()) + " and Message: " + result.getErrorMessage());

				this->serviceInitializeFailed(std::move(info));
			}
		}
	}
}

void ServiceManager::workerHealthCheck(void) {
	while (!m_isShuttingDown) {
		// Lock mutex for entire health check
		{
			std::lock_guard<std::mutex> lock(m_mutexServices);
			if (!m_sessions.empty()) {
				// Itereate through every service in every session
				for (auto& session : m_sessions) {
					for (auto serviceIt = session.second.begin(); serviceIt != session.second.end(); ) {
						// Check if service crashed
						ot::RunResult result = serviceIt->checkAlive();
						if (!result.isOk()) {
							OT_LOG_D("Service is not running anymore { "
								"\"ID\": " + std::to_string(serviceIt->getInfo().getServiceID()) + ", \"Name\": \"" + serviceIt->getInfo().getServiceName() + "\", \"Type\": \"" +
								serviceIt->getInfo().getServiceType() + "\", \"Url\": \"" + serviceIt->getUrl() + "\", \"SessionID\": \"" + serviceIt->getInfo().getSessionID() + "\" }");

							// Service died
							if (serviceIt->isShuttingDown()) {
								// The shutdown was intentional

								OT_LOG_D("Service shutdown completed { "
									"\"ID\": " + std::to_string(serviceIt->getInfo().getServiceID()) + ", \"Name\": \"" + serviceIt->getInfo().getServiceName() + "\", \"Type\": \"" +
									serviceIt->getInfo().getServiceType() + "\", \"Url\": \"" + serviceIt->getUrl() + "\", \"SessionID\": \"" + serviceIt->getInfo().getSessionID() + "\" }");

								// Notify LSS about shutdown completed
								this->notifyServiceShutdownCompleted(*serviceIt);

								// Remove the service from the session list
								std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
								m_stoppingServices.push_back(std::move(*serviceIt));

								serviceIt = session.second.erase(serviceIt);
							}
							else {
								OT_LOG_E("Service checkAlive failed. Error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());
								
								ot::RunResult result = serviceIt->shutdown();
								OT_LOG_E("Shuting service down with error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());

								if (serviceIt->getStartCounter() < serviceIt->getStartupData().getMaxCrashRestarts()) {
									// Attempt to restart service, if successful the list did not change and we can continue the health check.
									if (this->restartServiceAfterCrash(*serviceIt)) {
										serviceIt++;
										continue;
									}
								}

								// Either the restart failed or the restart counter reached its max value.
								// We notify the session service, clean up the lists and cancel the current health check.
								this->notifySessionEmergencyShutdown(*serviceIt);
								serviceIt++;
							}
						}
						else {
							serviceIt++;
						}
					}

				} // for session
			} // !services.empty
		} // mutex lock scope

		// sleep until next service is checked
		std::this_thread::sleep_for(m_serviceCheckAliveFrequency);

	}
}

void ServiceManager::workerServiceStopper(void) {
	while (!m_isShuttingDown) {
		// Lock mutex for check
		m_mutexStoppingServices.lock();
		std::list<Service> servicesTocheck;
		servicesTocheck.splice(servicesTocheck.end(), std::move(m_stoppingServices));
		m_stoppingServices.clear();

		for (Service& service : servicesTocheck) {
			ot::RunResult result =	service.checkAlive();
			if (result.isOk()) {
				// Service is still running
				m_stoppingServices.push_back(std::move(service));
			}
			else {
				ot::RunResult result = service.shutdown();

				// This is not an error, we simply wait for the exe to die to free up the port after the complete shutdown.

				// Clean up port numbers
				m_portManager.freePort(service.getPort());
				if (service.getWebsocketPort() != ot::invalidPortNumber) {
					m_portManager.freePort(service.getWebsocketPort());
				}
			}
		}

		m_mutexStoppingServices.unlock();

		// Unlock mutex and wait for 1 second before next check
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}
