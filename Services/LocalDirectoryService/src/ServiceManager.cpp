// Project header
#include "ServiceManager.h"
#include "Service.h"
#include "Configuration.h"

// OpenTwin header
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinSystem/PortManager.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/Msg.h"

// C++ header
#include <iostream>
#include <thread>

ServiceManager::ServiceManager()
	: m_isShuttingDown(false), m_threadServiceStarter(nullptr), m_threadServiceInitializer(nullptr), m_threadHealthCheck(nullptr),
	m_threadServiceStopper(nullptr)
{
	
}

ServiceManager::~ServiceManager() {
	m_isShuttingDown = true;
}

void ServiceManager::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) {
	// Info
	ot::rJSON::add(_document, _object, "Services.IP", m_servicesIpAddress);
	ot::rJSON::add(_document, _object, "LastError", m_lastError);
	ot::rJSON::add(_document, _object, "Site.ID", m_siteID);
	ot::rJSON::add(_document, _object, "IsShuttingDown", m_isShuttingDown);
	ot::rJSON::add(_document, _object, "Starter.ThreadRunning", (m_threadServiceStarter != nullptr));
	ot::rJSON::add(_document, _object, "Initializer.ThreadRunning", (m_threadServiceInitializer != nullptr));
	ot::rJSON::add(_document, _object, "HealthCheck.ThreadRunning", (m_threadHealthCheck != nullptr));
	
	// Stopping Services
	m_mutexStoppingServices.lock();
	OT_rJSON_createValueArray(stoppingArray);
	for (auto s : m_stoppingServices) {
		OT_rJSON_createValueObject(stoppingObj);
		s->addToJsonObject(_document, stoppingObj);
		stoppingArray.PushBack(stoppingObj, _document.GetAllocator());
	}
	m_mutexStoppingServices.unlock();
	ot::rJSON::add(_document, _object, "StoppingServices", stoppingArray);

	// Running Services
	m_mutexServices.lock();
	OT_rJSON_createValueArray(sessionsArray);
	for (auto s : m_services) {
		OT_rJSON_createValueArray(serviceArray);
		for (auto service : *s.second) {
			OT_rJSON_createValueObject(serviceObject);
			service->addToJsonObject(_document, serviceObject);
			serviceArray.PushBack(serviceObject, _document.GetAllocator());
		}

		OT_rJSON_createValueObject(sessionInfoObj);
		s.first.addToJsonObject(_document, sessionInfoObj);

		OT_rJSON_createValueObject(sessionTopLvlObj);
		ot::rJSON::add(_document, sessionTopLvlObj, "SessionInformation", sessionInfoObj);
		ot::rJSON::add(_document, sessionTopLvlObj, "Services", serviceArray);

		sessionsArray.PushBack(sessionTopLvlObj, _document.GetAllocator());
	}
	m_mutexServices.unlock();
	ot::rJSON::add(_document, _object, "AliveSessions", sessionsArray);

	// Initializing Services
	m_mutexInitializingServices.lock();
	OT_rJSON_createValueArray(iniArray);
	for (auto s : m_initializingServices) {
		OT_rJSON_createValueObject(sessionInfoObj);
		s.startInfo.session.addToJsonObject(_document, sessionInfoObj);

		OT_rJSON_createValueObject(serviceInfoObj);
		s.startInfo.service.addToJsonObject(_document, serviceInfoObj);

		OT_rJSON_createValueObject(requestObj);
		ot::rJSON::add(_document, requestObj, "SessionInformation", sessionInfoObj);
		ot::rJSON::add(_document, requestObj, "ServiceInformation", serviceInfoObj);
		ot::rJSON::add(_document, requestObj, "StartAttempt", s.startInfo.startAttempt);


		OT_rJSON_createValueObject(serviceObj);
		s.service->addToJsonObject(_document, serviceObj);

		OT_rJSON_createValueObject(iniObj);
		ot::rJSON::add(_document, iniObj, "Service", serviceObj);
		ot::rJSON::add(_document, iniObj, "StartInformation", requestObj);
		ot::rJSON::add(_document, iniObj, "InitializeAttempt", s.initializeAttempt);

		iniArray.PushBack(iniObj, _document.GetAllocator());
	}
	m_mutexInitializingServices.unlock();
	ot::rJSON::add(_document, _object, "InitializingServices", iniArray);

	// Startng Services
	m_mutexRequestedServices.lock();
	OT_rJSON_createValueArray(startArray);
	for (auto s : m_requestedServices) {
		OT_rJSON_createValueObject(sessionInfoObj);
		s.session.addToJsonObject(_document, sessionInfoObj);

		OT_rJSON_createValueObject(serviceInfoObj);
		s.service.addToJsonObject(_document, serviceInfoObj);

		OT_rJSON_createValueObject(startObj);
		ot::rJSON::add(_document, startObj, "SessionInformation", sessionInfoObj);
		ot::rJSON::add(_document, startObj, "ServiceInformation", serviceInfoObj);
		ot::rJSON::add(_document, startObj, "StartAttempt", s.startAttempt);

		startArray.PushBack(startObj, _document.GetAllocator());
	}
	m_mutexRequestedServices.unlock();
	ot::rJSON::add(_document, _object, "StartingServices", startArray);
}

// ##################################################################################################################################

// Service control

bool ServiceManager::requestStartService(const SessionInformation& _sessionInformation, const ServiceInformation& _serviceInformation) {
	// Check if the service is supported
	if (!LDS_CFG.supportsService(_serviceInformation.getName())) {
		OT_LOG_E("The service \"" + _serviceInformation.getName() + "\" is not supported by this Local Directory Service");
		m_lastError = "The service \"" + _serviceInformation.getName() + "\" is not supported by this Local Directory Service";
		return false;
	}

	m_mutexRequestedServices.lock();
	
	RequestedService info;
	info.session = _sessionInformation;
	info.service = _serviceInformation;
	info.startAttempt = 0;
	m_requestedServices.push_back(info);

	m_mutexRequestedServices.unlock();

	runThreads();

	return true;
}

bool ServiceManager::requestStartRelayService(const SessionInformation& _sessionInformation, std::string& _websocketUrl, std::string& _relayServiceURL) {
	m_mutexRequestedServices.lock();

	ot::app::RunResult result = ot::app::GeneralError;
	Service * newService = new Service(this, ServiceInformation(OT_INFO_SERVICE_TYPE_RelayService, OT_INFO_SERVICE_TYPE_RelayService));

	while (result != ot::app::OK) {
		// Attempt to start service
		result = newService->run(_sessionInformation, m_servicesIpAddress, ot::PortManager::instance().determineAndBlockAvailablePort(), ot::PortManager::instance().determineAndBlockAvailablePort());

		if (result == ot::app::OK) {
			_relayServiceURL = newService->url();
			_websocketUrl = newService->websocketUrl();
		}
		else {
			m_mutexRequestedServices.unlock();

			ot::PortManager::instance().setPortNotInUse(newService->port());
			ot::PortManager::instance().setPortNotInUse(newService->websocketPort());

			// Service start failed
			delete newService;

			OT_LOG_E("Service start failed");
			return false;
		}
	}

	// Store information
	m_mutexRequestedServices.unlock();

	m_mutexServices.lock();
	sessionServices(_sessionInformation)->push_back(newService);

	m_mutexServices.unlock();

	return true;
}

void ServiceManager::sessionClosed(const std::string& _sessionID) {
	// Clean up startup requests
	m_mutexRequestedServices.lock();
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); it++) {
			if (it->session.id() == _sessionID) {
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
			if (it->startInfo.session.id() == _sessionID) {
				m_stoppingServices.push_back(it->service);
				m_initializingServices.erase(it);
				erased = true;
				break;
			}
		}
	}
	m_mutexInitializingServices.unlock();

	// Clean up current services
	m_mutexServices.lock();

	for (auto s : m_services) {
		if (s.first.id() == _sessionID) {
			for (auto service : *s.second) {
				m_stoppingServices.push_back(service);
			}
			m_services.erase(s.first);
			break;
		}
	}

	m_mutexStoppingServices.unlock();
	m_mutexServices.unlock();
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
			if (it->service.getName() == _info.getName() && it->service.getType() == _info.getType()) {
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
				it->startInfo.service.getName() == _info.getName() &&
				it->startInfo.service.getType() == _info.getType()) 
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
				if ((*it._Ptr)->information().getName() == _info.getName() && 
					(*it._Ptr)->information().getType() == _info.getType() &&
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
		m_threadServiceStarter = new std::thread(&ServiceManager::workerServiceStarter, this);
	}

	if (m_threadServiceInitializer == nullptr) {
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
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceStartupFailed);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, _info.service.getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, _info.service.getType());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, _info.session.id());

	// Clean data
	//todo: Check which information to clear

	// Fire message
	ot::msg::sendAsync("", _info.session.sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(doc));
}

void ServiceManager::sendInitializeMessage(InitializingService _info) {
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_Init);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, _info.service->information().getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, _info.service->information().getType());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, _info.startInfo.session.id());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_SERVICE_URL, _info.startInfo.session.sessionServiceURL());

	// Send message
	std::string response;
	if (!ot::msg::send("", _info.service->url(), ot::EXECUTE, ot::rJSON::toJSON(doc), response)) {
		OT_LOG_W("Failed to send initialize message to service (name = \"" + _info.service->information().getName() + "\"; type = \"" +
			_info.service->information().getType() + "\"; url = \"" + _info.service->url() + "\")");

		serviceInitializeFailed(_info);
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_W("Invalid initialize message response from service (name = \"" + _info.service->information().getName() + "\"; type = \"" +
			_info.service->information().getType() + "\"; url = \"" + _info.service->url() + "\"; response = \"" + response + ")");

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
	//todo: Well we have to do something, dont we :D
	return false;
}

void ServiceManager::notifySessionEmergencyShutdown(const SessionInformation& _sessionInformation, Service * _crashedService) {
	SessionInformation session = _sessionInformation;
	ServiceInformation service = _crashedService->information();

	// Clean up startup requests
	m_mutexRequestedServices.lock();

	OT_LOG_E("Preparing Session Emergency Shutdown for session: \"" + session.id() + "\". Reason: Service crashed (Name = \"" + service.getName() + "\"; Type = \"" + service.getType() + "\")");

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

	m_mutexRequestedServices.unlock();
		
	// Notify session service about the crash
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceFailure);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, session.id());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, service.getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, service.getType());

	// Fire message
	ot::msg::sendAsync("", session.sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(doc));
}

void ServiceManager::workerServiceStarter(void) {
	while (!m_isShuttingDown) {

		// Check if a service was requested
		m_mutexRequestedServices.lock();
		if (m_requestedServices.empty()) {
			m_mutexRequestedServices.unlock();
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}
		else {
			RequestedService info = m_requestedServices.front();
			m_requestedServices.pop_front();

			m_mutexRequestedServices.unlock();
			
			// Increase start counter
			info.startAttempt = info.startAttempt + 1;

			// Check if max restarts is reached
			if (info.startAttempt > info.service.getMaxStartupRestarts()) {
				otAssert(0, "Maximum number of start attempts reached");
				OT_LOG_E("Maximum number of start attempts (" + std::to_string(info.service.getMaxStartupRestarts()) + 
					") reached for service (name = \"" + info.service.getName() + "\"; type = \"" + info.service.getType() + "\")");
				
				// Notify and skip
				serviceStartFailed(info);
				continue;
			}

			// Initialize service
			Service * newService = new Service(this, info.service);
			
			// Attempt to start service
			ot::app::RunResult result = newService->run(info.session, m_servicesIpAddress, ot::PortManager::instance().determineAndBlockAvailablePort());
			if (result != ot::app::OK) {
				// Clean up port numbers
				ot::PortManager::instance().setPortNotInUse(newService->port());
				ot::PortManager::instance().setPortNotInUse(newService->websocketPort());

				//todo: are multiple start attempts needed in this case?

				// Service start failed
				delete newService;

				serviceStartFailed(info);
			}
			else {
				OT_LOG_D("Service was started (name = \"" + info.service.getName() + "\"; type = \"" + info.service.getType() + "\")");

				// Move from start to initialize
				m_mutexInitializingServices.lock();

				InitializingService iniInfo;
				iniInfo.initializeAttempt = 0;
				iniInfo.startInfo = info;
				iniInfo.service = newService;
				m_initializingServices.push_back(iniInfo);

				m_mutexInitializingServices.unlock();
			}
		}
	}
}

void ServiceManager::workerServiceInitializer(void) {
	// Create ping command once
	OT_rJSON_createDOC(pingDoc);
	ot::rJSON::add(pingDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
	std::string pingCommand = ot::rJSON::toJSON(pingDoc);

	while (!m_isShuttingDown) {

		// Check if a service was requested
		m_mutexInitializingServices.lock();
		if (m_initializingServices.empty()) {
			m_mutexInitializingServices.unlock();
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}
		else {
			InitializingService info = m_initializingServices.front();
			m_initializingServices.pop_front();

			m_mutexInitializingServices.unlock();

			info.initializeAttempt = info.initializeAttempt + 1;

			if (info.service->checkAlive()) {
				OT_LOG_D("Pinging service (name = \"" + info.service->information().getName() + "\"; type = \"" + info.service->information().getType() + "\")");

				// Attempt to send ping
				std::string response;
				if (!ot::msg::send("", info.service->url(), ot::EXECUTE, pingCommand, response, 1000)) {
					// Failed to ping

					OT_LOG_D("Failed to ping service (attempt = \"" + std::to_string(info.initializeAttempt) + "\"; name = \"" + info.service->information().getName() + "\"; type = \"" +
						info.service->information().getType() + "\"; url = \"" + info.service->url() + "\")");

					if (info.initializeAttempt > 3) {
						OT_LOG_W("Failed to ping service 3 times. Moving service back to restart");
						serviceInitializeFailed(info);
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(info);
						m_mutexInitializingServices.unlock();
					}
				}
				else if (response != OT_ACTION_CMD_Ping) {
					// Invalid response

					OT_LOG_W("Invalid response from service (name = \"" + info.service->information().getName() + "\"; type = \"" +
						info.service->information().getType() + "\"; url = \"" + info.service->url() + "\")");

					if (info.initializeAttempt > 3) {
						serviceInitializeFailed(info);
					}
					else {
						m_mutexInitializingServices.lock();
						m_initializingServices.push_back(info);
						m_mutexInitializingServices.unlock();
					}
				}
				else {
					OT_LOG_D("Service ping success (name = \"" + info.service->information().getName() + "\"; type = \"" +
						info.service->information().getType() + "\"; url = \"" + info.service->url() + "\")");

					// Service alive

					sendInitializeMessage(info);
				}
			}
			else {
				OT_LOG_W("Service died while attempting to ping (name = \"" + info.service->information().getName() + "\"; type = \"" +
					info.service->information().getType() + "\"; url = \"" + info.service->url() + "\")");

				serviceInitializeFailed(info);
			}
		}
	}
}

void ServiceManager::workerHealthCheck(void) {
	while (!m_isShuttingDown) {
		// Lock mutex for entire health check
		m_mutexServices.lock();
		if (!m_services.empty()) {
			bool erased = false;
			// Itereate through every service in every session
			for (auto session : m_services) {
				for (auto service : *session.second) {
					// Chek if service crashed
					if (!service->checkAlive()) {
						if (service->startCounter() < service->information().getMaxCrashRestarts()) {
							// Attempt to restart service, if successful the list did not change and
							// we can continue the health check
							if (restartServiceAfterCrash(session.first, service)) continue;
						}

						// Either the restart failed or the restart counter reached its max value
						// We notify the session service, clean up the lists and cancel the current health check
						// Health check will not be terminated just the current loop will be cancelled
						notifySessionEmergencyShutdown(session.first, service);
						erased = true;
						break;
					}
				}
				if (erased) break;
			}
		}

		// Unlock mutex and wait for 1 second before next health check
		m_mutexServices.unlock();
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}

void ServiceManager::workerServiceStopper(void) {
	while (!m_isShuttingDown) {
		// Lock mutex for check
		m_mutexStoppingServices.lock();
		std::list<Service *> tocheck = m_stoppingServices;
		m_stoppingServices.clear();

		for (auto s : tocheck) {
			if (s->checkAlive()) {
				// Service is still running
				m_stoppingServices.push_back(s);
			}
			else {
				// Clean up port numbers
				ot::PortManager::instance().setPortNotInUse(s->port());
				ot::PortManager::instance().setPortNotInUse(s->websocketPort());

				delete s;
			}
		}

		m_mutexStoppingServices.unlock();

		// Unlock mutex and wait for 1 second before next check
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}