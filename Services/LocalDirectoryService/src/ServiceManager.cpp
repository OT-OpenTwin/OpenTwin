// Project header
#include "ServiceManager.h"
#include "Service.h"
#include "Configuration.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
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
	for (auto s : m_stoppingServices) {
		ot::JsonObject stoppingObj;
		s->addToJsonObject(stoppingObj, _allocator);
		stoppingArray.PushBack(stoppingObj, _allocator);
	}
	m_mutexStoppingServices.unlock();
	_object.AddMember("StoppingServices", stoppingArray, _allocator);

	// Running Services
	m_mutexServices.lock();
	ot::JsonArray sessionsArray;
	for (auto s : m_services) {
		ot::JsonArray serviceArray;
		for (auto service : *s.second) {
			ot::JsonObject serviceObj;
			service->addToJsonObject(serviceObj, _allocator);
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
	for (auto s : m_initializingServices) {
		ot::JsonObject sessionInfoObj;
		s.startInfo.session.addToJsonObject(sessionInfoObj, _allocator);

		ot::JsonObject serviceInfoObj;
		s.startInfo.service.addToJsonObject(serviceInfoObj, _allocator);

		ot::JsonObject requestObj;
		requestObj.AddMember("SessionInformation", sessionInfoObj, _allocator);
		requestObj.AddMember("ServiceInformation", serviceInfoObj, _allocator);
		requestObj.AddMember("StartAttempt", s.startInfo.startAttempt, _allocator);

		ot::JsonObject serviceObj;
		s.service->addToJsonObject(serviceObj, _allocator);

		ot::JsonObject iniObj;
		iniObj.AddMember("Service", serviceObj, _allocator);
		iniObj.AddMember("StartInformation", requestObj, _allocator);
		iniObj.AddMember("InitializeAttempt", s.initializeAttempt, _allocator);

		iniArray.PushBack(iniObj, _allocator);
	}
	m_mutexInitializingServices.unlock();
	_object.AddMember("InitializingServices", iniArray, _allocator);

	// Startng Services
	m_mutexRequestedServices.lock();
	ot::JsonArray startArray;
	for (auto s : m_requestedServices) {
		ot::JsonObject sessionInfoObj;
		s.session.addToJsonObject(sessionInfoObj, _allocator);

		ot::JsonObject serviceInfoObj;
		s.service.addToJsonObject(serviceInfoObj, _allocator);

		ot::JsonObject startObj;
		startObj.AddMember("SessionInformation", sessionInfoObj, _allocator);
		startObj.AddMember("ServiceInformation", serviceInfoObj, _allocator);
		startObj.AddMember("StartAttempt", s.startAttempt, _allocator);

		startArray.PushBack(startObj, _allocator);
	}
	m_mutexRequestedServices.unlock();
	_object.AddMember("StartingServices", startArray, _allocator);
}

// ##################################################################################################################################

// Service control

ServiceManager::RequestResult ServiceManager::requestStartService(const SessionInformation& _sessionInformation, const ServiceInformation& _serviceInformation) {
	// Check if the service is supported
	if (!LDS_CFG.supportsService(_serviceInformation.name())) {
		OT_LOG_E("The service \"" + _serviceInformation.name() + "\" is not supported by this Local Directory Service");
		m_lastError = "The service \"" + _serviceInformation.name() + "\" is not supported by this Local Directory Service";
		return RequestResult::FailedOnStart;
	}

	m_mutexRequestedServices.lock();
	
	RequestedService info;
	info.session = _sessionInformation;
	info.service = _serviceInformation;
	info.startAttempt = 0;
	//Paralllel thread checks frequently if something was added. Thread starts the next service in the queue
	m_requestedServices.push_back(info);

	m_mutexRequestedServices.unlock();

	runThreads();

	return RequestResult::Success;
}

ServiceManager::RequestResult ServiceManager::requestStartRelayService(const SessionInformation& _sessionInformation, std::string& _websocketUrl, std::string& _relayServiceURL) {
	m_mutexRequestedServices.lock();

	Service* newService = new Service(this, ServiceInformation(OT_INFO_SERVICE_TYPE_RelayService, OT_INFO_SERVICE_TYPE_RelayService));

	// Attempt to start service

	ot::RunResult result = newService->run(_sessionInformation, m_servicesIpAddress, ot::PortManager::instance().determineAndBlockAvailablePort(), ot::PortManager::instance().determineAndBlockAvailablePort());
	if (!result.isOk()) {
		m_mutexRequestedServices.unlock();

		ot::PortManager::instance().setPortNotInUse(newService->port());
		ot::PortManager::instance().setPortNotInUse(newService->websocketPort());

		// Service start failed
		delete newService;

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

	OT_LOG_D("Checking relay startup completed for relay service at \"" + newService->url() + "\"");

	for (int attempt = 0; attempt < 3; attempt++) {
		std::string response;
		if (ot::msg::send("", newService->url(), ot::EXECUTE, checkCommandString, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
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
			ot::RunResult result = newService->checkAlive();
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
		ot::RunResult result = newService->shutdown();
		OT_LOG_E("Shutting service down with error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());

		delete newService;
		return RequestResult::FailedOnPing;
	}

	OT_LOG_D("Verified that relay service is alive");

	_relayServiceURL = newService->url();
	_websocketUrl = newService->websocketUrl();


	// Store information
	m_mutexRequestedServices.unlock();

	m_mutexServices.lock();
	sessionServices(_sessionInformation)->push_back(newService);

	m_mutexServices.unlock();

	return RequestResult::Success;
}

void ServiceManager::sessionClosed(const std::string& _sessionID) {
	this->cleanUpSession_RequestedList(_sessionID);

	std::lock_guard<std::mutex> stopLock(m_mutexStoppingServices);
	this->cleanUpSession_IniList(_sessionID);
	this->cleanUpSession_AliveList(_sessionID);
}

void ServiceManager::serviceDisconnected(const std::string& _sessionID, const ServiceInformation& _info, const std::string& _serviceURL) {
	// Clean up start requests
	m_mutexRequestedServices.lock();
	bool erased = true;
	while (erased) {
		erased = false;
		for (std::list<RequestedService>::iterator it = m_requestedServices.begin();
			it != m_requestedServices.end(); it++)
		{
			if (it->service.name() == _info.name() && it->service.type() == _info.type()) {
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
			if (it->startInfo.session.id() == _sessionID && 
				it->startInfo.service.name() == _info.name() &&
				it->startInfo.service.type() == _info.type())
			{
				m_stoppingServices.push_back(it->service);
				m_initializingServices.erase(it);
				erased = true;
				break;
			}
		}
	}
	m_mutexInitializingServices.unlock();

	// Clean up the current services
	m_mutexServices.lock();
	for (auto s : m_services) {
		if (s.first.id() == _sessionID) {
			for (auto it = s.second->begin(); it != s.second->end(); it++) {
				if ((*it._Ptr)->information().name() == _info.name() &&
					(*it._Ptr)->information().type() == _info.type() &&
					(*it._Ptr)->url() == _serviceURL) 
				{
					m_stoppingServices.push_back((*it._Ptr));
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

// ##################################################################################################################################

// Private functions

void ServiceManager::runThreads(void) {
	
	if (m_threadServiceStarter == nullptr) {
		//Starts thread to start the front entry of services to start
		m_threadServiceStarter = new std::thread(&ServiceManager::workerServiceStarter, this);
	}

	if (m_threadServiceInitializer == nullptr) {
		//Starts thread to initialise the front entry of services to initialise
		m_threadServiceInitializer = new std::thread(&ServiceManager::workerServiceInitializer, this);
	}

	if (m_threadHealthCheck == nullptr) {
		m_threadHealthCheck = new std::thread(&ServiceManager::workerHealthCheck, this);
	}

	if (m_threadServiceStopper == nullptr) {
		m_threadServiceStopper = new std::thread(&ServiceManager::workerServiceStopper, this);
	}
}

void ServiceManager::serviceStartFailed(RequestedService _info) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceStartupFailed, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_info.service.name(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_info.service.type(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_info.session.id(), doc.GetAllocator()), doc.GetAllocator());

	// Clean data
	//todo: Check which information to clear

	// Fire message
	ot::msg::sendAsync("", _info.session.sessionServiceURL(), ot::EXECUTE, doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}

void ServiceManager::sendInitializeMessage(InitializingService _info) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_info.service->information().name(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_info.service->information().type(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_info.startInfo.session.id(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_info.startInfo.session.sessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	
	// Send message
	std::string response;
	if (!ot::msg::send("", _info.service->url(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_W("Failed to send initialize message to service (name = \"" + _info.service->information().name() + "\"; type = \"" +
			_info.service->information().type() + "\"; url = \"" + _info.service->url() + "\")");

		serviceInitializeFailed(_info);
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_W("Invalid initialize message response from service (name = \"" + _info.service->information().name() + "\"; type = \"" +
			_info.service->information().type() + "\"; url = \"" + _info.service->url() + "\"; response = \"" + response + ")");

		serviceInitializeFailed(_info);
	}
	else {
		// Service was pinged, did its initialization call, so now we move it to alive (doing health check)
		m_mutexServices.lock();
		sessionServices(_info.startInfo.session)->push_back(_info.service);
		m_mutexServices.unlock();
	}
}

void ServiceManager::serviceInitializeFailed(InitializingService _info) {
	delete _info.service;

	m_mutexRequestedServices.lock();
	m_requestedServices.push_back(_info.startInfo);
	m_mutexRequestedServices.unlock();
}

std::vector<Service *> * ServiceManager::sessionServices(const SessionInformation& _sessionInformation) {
	auto it = m_services.find(_sessionInformation);
	if (it == m_services.end()) {
		std::vector<Service *> * newEntry = new std::vector<Service *>();
		m_services.insert_or_assign(_sessionInformation, newEntry);
		return newEntry;
	}
	else return it->second;
}

bool ServiceManager::restartServiceAfterCrash(const SessionInformation& _sessionInformation, Service * _service) {
	// Currently services are not restarted after a crash.
	// A service crash will lead to a session shutdow of the session the service was running in.
	return false;
}

void ServiceManager::notifySessionEmergencyShutdown(const SessionInformation& _sessionInformation, Service * _crashedService) {
	SessionInformation session = _sessionInformation;
	ServiceInformation service = _crashedService->information();

	// Clean up startup requests
	std::lock_guard<std::mutex> lock(m_mutexRequestedServices);

	OT_LOG_E("Preparing Session Emergency Shutdown for session: \"" + session.id() + "\". Reason: Service crashed (Name = \"" + service.name() + "\"; Type = \"" + service.type() + "\")");

	std::list<RequestedService>::iterator it;
	bool erased = true;
	while (erased) {
		erased = false;
		it = m_requestedServices.begin();
		for (; it != m_requestedServices.end() && !erased; it++) {
			if (it->session == session) {
				m_requestedServices.erase(it);
				erased = true;
				break;
			}
		}
	}

	// Clean up services
	auto itt = m_services.find(session);
	if (itt != m_services.end()) {
		for (auto s : *itt->second) {
			delete s;
		}
		delete itt->second;
	}
	m_services.erase(session);

	// Notify session service about the crash
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceFailure, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.name(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.type(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(session.id(), doc.GetAllocator()), doc.GetAllocator());
	
	// Fire message
	ot::msg::sendAsync("", session.sessionServiceURL(), ot::EXECUTE, doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}

void ServiceManager::cleanUpSession_RequestedList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> reqLock(m_mutexRequestedServices);

	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); it++) {
			if (it->session.id() == _sessionID) {
				OT_LOG_D("Handling Session Closed (Session: " + _sessionID + "): Removing requested service: " + it->service.name());
				m_requestedServices.erase(it);
				erased = true;
				break;
			}
		}
	}
}

void ServiceManager::cleanUpSession_IniList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);

	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_initializingServices.begin(); it != m_initializingServices.end(); it++) {
			if (it->startInfo.session.id() == _sessionID) {
				m_stoppingServices.push_back(it->service);
				m_initializingServices.erase(it);
				erased = true;
				break;
			}
		}
	}
}

void ServiceManager::cleanUpSession_AliveList(const std::string& _sessionID) {
	std::lock_guard<std::mutex> serviceLock(m_mutexServices);

	for (auto s : m_services) {
		if (s.first.id() == _sessionID) {
			for (auto service : *s.second) {
				m_stoppingServices.push_back(service);
			}
			m_services.erase(s.first);
			break;
		}
	}
}

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
			RequestedService info = m_requestedServices.front();
			m_requestedServices.pop_front();

			OT_LOG_D("Starting service: " + info.service.name());

			// Requested services modifications completed
			m_mutexRequestedServices.unlock();
			
			// Increase start counter
			info.startAttempt = info.startAttempt + 1;

			// Check if max restarts is reached
			if (info.startAttempt > info.service.maxStartupRestarts()) {
				OTAssert(0, "Maximum number of start attempts reached");
				OT_LOG_E("Maximum number of start attempts (" + std::to_string(info.service.maxStartupRestarts()) + 
					") reached for service (name = \"" + info.service.name() + "\"; type = \"" + info.service.type() + "\")");
				
				// Notify and skip
				this->serviceStartFailed(info);
				continue;
			}

			// Initialize service
			Service * newService = new Service(this, info.service);
			
			// Attempt to start service
			ot::RunResult result = newService->run(info.session, m_servicesIpAddress, ot::PortManager::instance().determineAndBlockAvailablePort());
			
			if (!result.isOk()) {
				OT_LOG_E("Service start failed with error code: " + std::to_string(result.getErrorCode()) + " and error message: " + result.getErrorMessage());
				
				// Clean up port numbers
				ot::PortManager::instance().setPortNotInUse(newService->port());
				ot::PortManager::instance().setPortNotInUse(newService->websocketPort());

				//! @todo are multiple start attempts needed in this case?
				//! We have failed to launch the executable.
				//! The port is not yet an issue at this point since
				//! the webserver is launched in a different thread later in the start logic.

				// Service start failed
				delete newService;
				this->serviceStartFailed(info);
			}
			else {
				OT_LOG_D("Service was started (name = \"" + info.service.name() + "\"; type = \"" + info.service.type() + "\")");

				std::lock_guard<std::mutex> iniLock(m_mutexInitializingServices);

				// Move from start to initialize
				InitializingService iniInfo;
				iniInfo.initializeAttempt = 0;
				iniInfo.startInfo = info;
				iniInfo.service = newService;
				m_initializingServices.push_back(iniInfo);
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
			InitializingService info = m_initializingServices.front();
			m_initializingServices.pop_front();

			m_mutexInitializingServices.unlock();

			OT_LOG_D("Initialising service: " + info.service->information().name());

			// Increase initialize attempt of the service
			info.initializeAttempt = info.initializeAttempt + 1;

			// Check if service executable is still running
			ot::RunResult result =	info.service->checkAlive();

			if (result.isOk()) {
				// Service executable is 

				OT_LOG_D("Pinging service (name = \"" + info.service->information().name() + "\"; type = \"" + info.service->information().type() + "\")");

				// Attempt to send ping
				std::string response;
				if (!ot::msg::send("", info.service->url(), ot::EXECUTE, pingCommand, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					// Failed to ping

					OT_LOG_D("Failed to ping service (attempt = \"" + std::to_string(info.initializeAttempt) + "\"; name = \"" + info.service->information().name() + "\"; type = \"" +
						info.service->information().type() + "\"; url = \"" + info.service->url() + "\")");

					if (info.initializeAttempt > 3) {
						OT_LOG_W("Failed to ping service 3 times. Moving service back to restart");
						this->serviceInitializeFailed(info);
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(info);
						m_mutexInitializingServices.unlock();
					}
				}
				else if (response != OT_ACTION_CMD_Ping) {
					// Invalid response

					OT_LOG_W("Invalid response from service (name = \"" + info.service->information().name() + "\"; type = \"" +
						info.service->information().type() + "\"; url = \"" + info.service->url() + "\")");

					if (info.initializeAttempt > 3) {
						this->serviceInitializeFailed(info);
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(info);
						m_mutexInitializingServices.unlock();
					}
				}
				else {
					OT_LOG_D("Service ping success (name = \"" + info.service->information().name() + "\"; type = \"" +
						info.service->information().type() + "\"; url = \"" + info.service->url() + "\")");

					// Service alive

					this->sendInitializeMessage(info);
				}
			}
			else {
				OT_LOG_E("Service check alive failed during initialization: { "
					"\"AppErrorCode\": " + std::to_string(result.getErrorCode()) + 
					"\", \"ServiceName\": \"" + info.service->information().name() +
					"\", \"ServiceType\": \"" + info.service->information().type() +
					"\", \"ServiceUrl\": \"" + info.service->url() +
					"\" }. App Error Message: " + result.getErrorMessage());
				
				result = info.service->shutdown();
				OT_LOG_I("Service shutdown after crash completed with Error code: " + std::to_string(result.getErrorCode()) + " and Message: " + result.getErrorMessage());

				this->serviceInitializeFailed(info);
			}
		}
	}
}

void ServiceManager::GetSessionInformation(ot::JsonArray& sessionInfo, ot::JsonAllocator& allocator) {

	for (auto session : m_services) {

		ot::JsonArray servicesInfo;

		for (auto service : *session.second)
		{
			ot::JsonValue serviceInfo;
			serviceInfo.SetObject();

			serviceInfo.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(service->url(), allocator), allocator);
			serviceInfo.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service->information().type(), allocator), allocator);

			servicesInfo.PushBack(serviceInfo, allocator);
		}

		ot::JsonValue info;
		info.SetObject();

		info.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(session.first.id(), allocator), allocator);
		info.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, servicesInfo, allocator);

		sessionInfo.PushBack(info, allocator);
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
				for (auto session : m_services) {
					for (auto service : *session.second) {

						// Check if service crashed
						ot::RunResult result = service->checkAlive();
						if (!result.isOk()) {
							OT_LOG_E("Service checkAlive failed. Error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());

							ot::RunResult result = service->shutdown();
							OT_LOG_E("Shuting service down with error code: " + std::to_string(result.getErrorCode()) + "\nMessage: " + result.getErrorMessage());

							if (service->startCounter() < service->information().maxCrashRestarts()) {
								// Attempt to restart service, if successful the list did not change and
								// we can continue the health check.
								if (this->restartServiceAfterCrash(session.first, service)) {
									continue;
								}
							}

							// Either the restart failed or the restart counter reached its max value.
							// We notify the session service, clean up the lists and cancel the current health check.
							// Health check will not be terminated just the current loop will be cancelled.
							this->notifySessionEmergencyShutdown(session.first, service);
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
		std::list<Service *> servicesTocheck = m_stoppingServices;
		m_stoppingServices.clear();

		for (auto service : servicesTocheck) 
		{
			ot::RunResult result =	service->checkAlive();
			if (result.isOk()) 
			{
				// Service is still running
				m_stoppingServices.push_back(service);
			}
			else 
			{
				ot::RunResult result = service->shutdown();

				// This is not an error, we simply wait for the exe to die to free up the port after the complete shutdown.

				// Clean up port numbers
				ot::PortManager::instance().setPortNotInUse(service->port());
				ot::PortManager::instance().setPortNotInUse(service->websocketPort());

				delete service;
			}
		}

		m_mutexStoppingServices.unlock();

		// Unlock mutex and wait for 1 second before next check
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}
