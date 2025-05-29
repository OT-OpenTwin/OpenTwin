//! @file ServiceManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Service.h"
#include "Configuration.h"
#include "ServiceManager.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTSystem/PortManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// std header
#include <thread>
#include <chrono>

ServiceManager::ServiceManager()
	: m_isShuttingDown(false), m_threadServiceStarter(nullptr), m_threadServiceInitializer(nullptr), m_threadHealthCheck(nullptr),
	m_threadServiceStopper(nullptr), m_generalWait(false)
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
	for (auto s : m_services) {
		ot::JsonArray serviceArray;
		for (Service& service : *s.second) {
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
	for (Service& s : m_initializingServices) {
		ot::JsonObject obj;
		s.addToJsonObject(obj, _allocator);
		iniArray.PushBack(obj, _allocator);
	}
	m_mutexInitializingServices.unlock();
	_object.AddMember("InitializingServices", iniArray, _allocator);

	// Startng Services
	m_mutexRequestedServices.lock();
	ot::JsonArray startArray;
	for (ServiceInformation& s : m_requestedServices) {
		ot::JsonObject obj;
		s.addToJsonObject(obj, _allocator);
		startArray.PushBack(obj, _allocator);
	}
	m_mutexRequestedServices.unlock();
	_object.AddMember("StartingServices", startArray, _allocator);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Service control

ServiceManager::RequestResult ServiceManager::requestStartService(const ServiceInformation& _serviceInformation) {
	// Check if the service is supported
	if (!Configuration::instance().supportsService(_serviceInformation.getName())) {
		OT_LOG_E("The service \"" + _serviceInformation.getName() + "\" is not supported by this Local Directory Service");
		m_lastError = "The service \"" + _serviceInformation.getName() + "\" is not supported by this Local Directory Service";
		return RequestResult::FailedOnStart;
	}

	std::lock_guard<std::mutex> lock(m_mutexRequestedServices);
	
	// Parallel thread checks frequently if something was added. Thread starts the next service in the queue
	m_requestedServices.push_back(_serviceInformation);

	this->runThreads();

	return RequestResult::Success;
}

ServiceManager::RequestResult ServiceManager::requestStartRelayService(const SessionInformation& _sessionInformation, std::string& _websocketUrl, std::string& _relayServiceURL) {
	m_mutexRequestedServices.lock();

	Service newService(this, ServiceInformation(OT_INFO_SERVICE_TYPE_RelayService, OT_INFO_SERVICE_TYPE_RelayService, _sessionInformation));

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
	OT_LOG_D("Relay started successfully. Now checking if allive.");
	ot::JsonDocument checkCommandDoc;
	checkCommandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CheckRelayStartupCompleted, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
	std::string checkCommandString = checkCommandDoc.toJson();

	int attempt = 0;
	bool checkOk = false;
	std::string errorMessage;

	OT_LOG_D("Checking relay startup completed for relay service at \"" + newService.getUrl() + "\"");

	for (int attempt = 0; attempt < 3; attempt++) {
		std::string response;
		if (ot::msg::send("", newService.getUrl(), ot::EXECUTE, checkCommandString, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			if (response == OT_ACTION_RETURN_VALUE_TRUE) {
				checkOk = true;
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

			// Ensure process is still running
			ot::RunResult result = newService.checkAlive();
			if (!result.isOk()) {
				// It is possible that the port was blocked. What happened: The process started but then died.
				OT_LOG_E("Relayservice died while trying to check if it is alive. Exit code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());
				break;
			}
		}
		else {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
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
	sessionServices(_sessionInformation)->push_back(std::move(newService));

	m_mutexServices.unlock();

	return RequestResult::Success;
}

void ServiceManager::sessionClosing(const std::string& _sessionID) {
	this->cleanUpSession_RequestedList(_sessionID);

	std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);

	this->cleanUpSession_IniList(_sessionID);

	std::lock_guard<std::mutex> servicesLock(m_mutexServices);
	for (auto& session : m_services) {
		if (session.first.getId() == _sessionID) {
			// Mark all alive services in the given session as stopping
			for (auto& service : *session.second) {
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

void ServiceManager::serviceDisconnected(const ServiceInformation& _info, const std::string& _serviceURL) {
	// Clean up start requests
	m_mutexRequestedServices.lock();
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); it++) {
			if (it->getName() == _info.getName() && it->getType() == _info.getType() && it->getSessionId() == _info.getSessionId()) {
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
			if (it->getInfo().getSessionId() == _info.getSessionId() &&
				it->getInfo().getName() == _info.getName() &&
				it->getInfo().getType() == _info.getType())
			{
				m_stoppingServices.push_back(std::move(*m_initializingServices.erase(it)));

				erased = true;
				break;
			}
		}
	}
	m_mutexInitializingServices.unlock();

	// Clean up the current services
	m_mutexServices.lock();
	for (auto& s : m_services) {
		if (s.first.getId() == _info.getSessionId()) {
			for (auto it = s.second->begin(); it != s.second->end(); it++) {
				if (it->getInfo().getName() == _info.getName() &&
					it->getInfo().getType() == _info.getType() &&
					it->getUrl() == _serviceURL)
				{
					m_stoppingServices.push_back(std::move(*it));
					s.second->erase(it);
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

void ServiceManager::getSessionInformation(ot::JsonArray& sessionInfo, ot::JsonAllocator& allocator) {

	for (auto session : m_services) {

		ot::JsonArray servicesInfo;

		for (Service& service : *session.second) {
			ot::JsonValue serviceInfo;
			serviceInfo.SetObject();

			serviceInfo.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(service.getUrl(), allocator), allocator);
			serviceInfo.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getInfo().getType(), allocator), allocator);

			servicesInfo.PushBack(serviceInfo, allocator);
		}

		ot::JsonValue info;
		info.SetObject();

		info.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(session.first.getId(), allocator), allocator);
		info.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, servicesInfo, allocator);

		sessionInfo.PushBack(info, allocator);
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

void ServiceManager::serviceStartFailed(const ServiceInformation& _info) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceStartupFailed, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_info.getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_info.getType(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_info.getSessionId(), doc.GetAllocator()), doc.GetAllocator());

	// Fire message
	ot::msg::sendAsync("", _info.getSessionServiceURL(), ot::EXECUTE, doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}

void ServiceManager::sendInitializeMessage(Service&& _info) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_info.getInfo().getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_info.getInfo().getType(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_info.getInfo().getSessionId(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_info.getInfo().getSessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	
	// Send message
	std::string response;
	if (!ot::msg::send("", _info.getUrl(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_W("Failed to send initialize message to service { \"Name\": \"" + _info.getInfo().getName() + "\", \"Type\": \"" +
			_info.getInfo().getType() + "\", \"Url\": \"" + _info.getUrl() + "\" }");

		serviceInitializeFailed(_info);
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_W("Invalid initialize message response from service { \"Name\": \"" + _info.getInfo().getName() + "\", \"Type\": \"" +
			_info.getInfo().getType() + "\", \"Url\": \"" + _info.getUrl() + "\", \"Response\": \"" + response + "\" }");

		serviceInitializeFailed(_info);
	}
	else {
		// Service was pinged, did its initialization call, so now we move it to alive (doing health check)
		m_mutexServices.lock();
		sessionServices(_info.getInfo().getSessionInformation())->push_back(std::move(_info));
		m_mutexServices.unlock();
	}
}

void ServiceManager::serviceInitializeFailed(const Service& _info) {
	m_mutexRequestedServices.lock();
	m_requestedServices.push_back(_info.getInfo());
	m_mutexRequestedServices.unlock();
}

std::vector<Service> * ServiceManager::sessionServices(const SessionInformation& _sessionInformation) {
	auto it = m_services.find(_sessionInformation);
	if (it == m_services.end()) {
		std::vector<Service> * newEntry = new std::vector<Service>();
		m_services.insert_or_assign(_sessionInformation, newEntry);
		return newEntry;
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
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_service.getInfo().getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_service.getInfo().getType(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(_service.getUrl(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_service.getInfo().getSessionId(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", _service.getInfo().getSessionServiceURL(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send shutdown completed message to session service { \"Name\": \"" + _service.getInfo().getName() +
			"\", \"Type\": \"" + _service.getInfo().getType() + "\", \"Service.Url\": \"" + _service.getUrl() + "\", \"LSS.Url\": \"" + _service.getInfo().getSessionServiceURL() + "\" }");
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invalid shutdown completed response from session service { \"Name\": \"" + _service.getInfo().getName() +
			"\", \"Type\": \"" + _service.getInfo().getType() + "\", \"Service.Url\": \"" + _service.getUrl() + "\", \"LSS.Url\": \"" + _service.getInfo().getSessionServiceURL() +
			"\", \"Response\": \"" + response + "\" }");
	}
}

void ServiceManager::notifySessionEmergencyShutdown(const Service& _crashedService) {
	// Clean up startup requests
	std::lock_guard<std::mutex> lock(m_mutexRequestedServices);

	OT_LOG_E("Preparing Session Emergency Shutdown for session: \"" + _crashedService.getInfo().getSessionId() + 
		"\". Reason: Service crashed { \"Name\": \"" + _crashedService.getInfo().getName() + 
		"\", \"Type\": \"" + _crashedService.getInfo().getType() + "\" }");

	bool erased = true;

	// Remove all requested services that are related to the session
	while (erased) {
		erased = false;
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end() && !erased; it++) {
			if (it->getSessionId() == _crashedService.getInfo().getSessionId()) {
				m_requestedServices.erase(it);
				erased = true;
				break;
			}
		}
	}

	// Clean up running services
	ServiceInformation serviceInfo = _crashedService.getInfo();
	auto it = m_services.find(serviceInfo.getSessionInformation());
	if (it != m_services.end()) {
		delete it->second;
	}
	m_services.erase(serviceInfo.getSessionInformation());

	// Notify session service about the crash
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceFailure, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(serviceInfo.getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(serviceInfo.getType(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(serviceInfo.getSessionId(), doc.GetAllocator()), doc.GetAllocator());
	
	// Fire message
	ot::msg::sendAsync("", serviceInfo.getSessionServiceURL(), ot::EXECUTE, doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Cleanup

void ServiceManager::cleanUpSession_RequestedList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> reqLock(m_mutexRequestedServices);

	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); it++) {
			if (it->getSessionId() == _sessionID) {
				OT_LOG_D("Handling session closed (Session: " + _sessionID + "): Removing requested service: " + it->getName());

				m_requestedServices.erase(it);
				erased = true;
				break;
			}
		}
	}
}

void ServiceManager::cleanUpSession_IniList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShutdown, doc.GetAllocator()), doc.GetAllocator());
	std::string cmd = doc.toJson();

	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_initializingServices.begin(); it != m_initializingServices.end(); it++) {
			if (it->getInfo().getSessionId() == _sessionID) {
				// Send shutdown message to the service
				auto x = m_initializingServices.erase(it);

				std::string response;
				if (!ot::msg::send("", x->getUrl(), ot::EXECUTE, cmd, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					OT_LOG_EAS("Failed to send shutdown message to service { \"Name\": \"" + x->getInfo().getName() + "\", \"Type\": \"" +
						x->getInfo().getType() + "\", \"Url\": \"" + x->getUrl() + "\" }");
				}

				m_stoppingServices.push_back(std::move(*x));
				erased = true;
				break;
			}
		}
	}
}

void ServiceManager::cleanUpSession_AliveList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> serviceLock(m_mutexServices);

	for (auto s : m_services) {
		if (s.first.getId() == _sessionID) {
			for (auto& service : *s.second) {
				m_stoppingServices.push_back(std::move(service));
			}
			m_services.erase(s.first);
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
			ServiceInformation info = m_requestedServices.front();
			m_requestedServices.pop_front();

			OT_LOG_D("Starting service: " + info.getName());

			// Requested services modifications completed
			m_mutexRequestedServices.unlock();
			
			// Increase start counter
			info.incrStartCounter();

			// Check if max restarts is reached
			if (info.getStartCounter() > info.getMaxStartupRestarts()) {
				OTAssert(0, "Maximum number of start attempts reached");
				OT_LOG_E("Maximum number of start attempts (" + std::to_string(info.getMaxStartupRestarts()) +
					") reached for service (name = \"" + info.getName() + "\"; type = \"" + info.getType() + "\")");
				
				// Notify and skip
				this->serviceStartFailed(info);
				continue;
			}

			// Initialize service
			Service newService(this, info);
			
			// Attempt to start service
			ot::RunResult result = newService.run(m_servicesIpAddress, m_portManager.determineAndBlockAvailablePort());
			
			if (!result.isOk()) {
				OT_LOG_E("Service start failed with error code: " + std::to_string(result.getErrorCode()) + " and error message: " + result.getErrorMessage());
				
				// Clean up port numbers
				m_portManager.freePort(newService.getPort());
				m_portManager.freePort(newService.getWebsocketPort());

				// Service start failed
				this->serviceStartFailed(info);
			}
			else {
				OT_LOG_D("Service was started (name = \"" + info.getName() + "\"; type = \"" + info.getType() + "\")");

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

			OT_LOG_D("Initialising service: " + info.getInfo().getName());

			// Increase initialize attempt of the service
			info.incrIniAttempt();

			// Check if service executable is still running
			ot::RunResult result =	info.checkAlive();

			if (result.isOk()) {
				// Service executable is 

				OT_LOG_D("Pinging service { \"Name\": \"" + info.getInfo().getName() + "\", \"Type\": \"" + info.getInfo().getType() + "\" }");

				// Attempt to send ping
				std::string response;
				if (!ot::msg::send("", info.getUrl(), ot::EXECUTE, pingCommand, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					// Failed to ping

					OT_LOG_D("Failed to ping service (attempt = \"" + std::to_string(info.getIniAttempt()) + "\"; name = \"" + info.getInfo().getName() + "\"; type = \"" +
						info.getInfo().getType() + "\"; url = \"" + info.getUrl() + "\")");

					if (info.getIniAttempt() > 3) {
						OT_LOG_W("Failed to ping service 3 times. Moving service back to restart");
						this->serviceInitializeFailed(info);
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(std::move(info));
						m_mutexInitializingServices.unlock();
					}
				}
				else if (response != OT_ACTION_CMD_Ping) {
					// Invalid response

					OT_LOG_W("Invalid response from service (name = \"" + info.getInfo().getName() + "\"; type = \"" +
						info.getInfo().getType() + "\"; url = \"" + info.getUrl() + "\")");

					if (info.getIniAttempt() > 3) {
						this->serviceInitializeFailed(info);
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(std::move(info));
						m_mutexInitializingServices.unlock();
					}
				}
				else {
					OT_LOG_D("Service ping success (name = \"" + info.getInfo().getName() + "\"; type = \"" +
						info.getInfo().getType() + "\"; url = \"" + info.getUrl() + "\")");

					// Service alive

					this->sendInitializeMessage(std::move(info));
				}
			}
			else {
				OT_LOG_E("Service check alive failed during initialization: { "
					"\"AppErrorCode\": " + std::to_string(result.getErrorCode()) + 
					"\", \"ServiceName\": \"" + info.getInfo().getName() +
					"\", \"ServiceType\": \"" + info.getInfo().getType() +
					"\", \"ServiceUrl\": \"" + info.getUrl() +
					"\" }. App Error Message: " + result.getErrorMessage());
				
				result = info.shutdown();
				OT_LOG_I("Service shutdown after crash completed with error code: " + std::to_string(result.getErrorCode()) + " and Message: " + result.getErrorMessage());

				this->serviceInitializeFailed(info);
			}
		}
	}
}

void ServiceManager::workerHealthCheck(void) {
	while (!m_isShuttingDown) {
		// Lock mutex for entire health check
		{
			std::lock_guard<std::mutex> lock(m_mutexServices);
			if (!m_services.empty()) {
				bool erased = false;
				// Itereate through every service in every session
				for (auto& session : m_services) {
					for (auto it = session.second->begin(); it != session.second->end(); it++) {

						// Check if service crashed
						ot::RunResult result = it->checkAlive();
						if (!result.isOk()) {
							// Service died
							if (it->isShuttingDown()) {
								// The shutdown was intentional

								OT_LOG_D("Service shutdown completed { \"Name\": \"" + it->getInfo().getName() +
									"\", \"Type\": \"" + it->getInfo().getType() + "\", \"Url\": \"" + it->getUrl() + "\" }");

								// Notify LSS about shutdown completed
								this->notifyServiceShutdownCompleted(*it);

								// Remove the service from the session list
								std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
								m_stoppingServices.push_back(std::move(*session.second->erase(it)));
							}
							else {
								OT_LOG_E("Service checkAlive failed. Error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());
								
								ot::RunResult result = it->shutdown();
								OT_LOG_E("Shuting service down with error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());

								if (it->getStartCounter() < it->getInfo().getMaxCrashRestarts()) {
									// Attempt to restart service, if successful the list did not change and we can continue the health check.
									if (this->restartServiceAfterCrash(*it)) {
										continue;
									}
								}

								// Either the restart failed or the restart counter reached its max value.
								// We notify the session service, clean up the lists and cancel the current health check.
								// Health check will not be terminated just the current loop will be cancelled.
								this->notifySessionEmergencyShutdown(*it);
							}

							erased = true;
							break;
						}
					}
					
					if (erased) {
						break;
					}

				} // for session
			} // !services.empty
		} // mutex lock scope

		// sleep for one second
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);

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
				m_portManager.freePort(service.getWebsocketPort());
			}
		}

		m_mutexStoppingServices.unlock();

		// Unlock mutex and wait for 1 second before next check
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}
