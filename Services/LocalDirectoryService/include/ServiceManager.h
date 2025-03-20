#pragma once

#include "ServiceInformation.h"
#include "SessionInformation.h"

#include <map>
#include <string>
#include <vector>
#include <list>
#include <mutex>

#include "OTCore/JSON.h"
#include "OTSystem/Network.h"

class Service;
namespace std { class thread; }

class ServiceManager {
public:
	ServiceManager();
	virtual ~ServiceManager();

	//! @brief Add the object contents to the provided JSON object
	//! @param _document The JSON document (used to get the allocator)
	//! @param _object The JSON object to add the contents to
	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator);

	// ##################################################################################################################################

	// Setter/Getter

	void setServiceIP(const std::string& _ip) { m_servicesIpAddress = _ip; }
	
	const std::string& lastError(void) const { return m_lastError; }

	void setSiteID(const std::string& _siteID) { m_siteID = _siteID; }
	const std::string& siteID(void) const { return m_siteID; }

	// ##################################################################################################################################

	// Service control

	//! @brief Will attempt to start a new service
	//! @param _sessionInformation Information about the session where the service should be connected to
	//! @param _serviceInformation Information about the service that should be started
	//! @param _serviceURL The URL where the service will be running will be written here
	bool requestStartService(const SessionInformation& _sessionInformation, const ServiceInformation& _serviceInformation);

	bool requestStartRelayService(const SessionInformation& _sessionInformation, std::string& _websocketUrl, std::string& _relayServiceURL);

	void sessionClosed(const std::string& _sessionID);

	void serviceDisconnected(const std::string& _sessionID, const ServiceInformation& _info, const std::string& _serviceURL);

	void GetSessionInformation(ot::JsonArray &sessionInfo, ot::JsonAllocator &allocator);

private:

	struct RequestedService {
		SessionInformation session;
		ServiceInformation service;
		unsigned int startAttempt = 0;
	};
	struct InitializingService {
		Service * service;
		RequestedService startInfo;
		int initializeAttempt = 0; 
	};

	void runThreads(void);
	
	void serviceStartFailed(RequestedService _info);
	void sendInitializeMessage(InitializingService _info);
	void serviceInitializeFailed(InitializingService _info);

	std::vector<Service *> * sessionServices(const SessionInformation& _sessionInformation);
	bool restartServiceAfterCrash(const SessionInformation& _sessionInformation, Service * _service);
	void notifySessionEmergencyShutdown(const SessionInformation& _sessionInformation, Service * _crashedService);

	//! @brief Clean up session related information from requested services list.
	//! @param _sessionID Session info to clean up.
	void cleanUpSession_RequestedList(const std::string& _sessionID);
	void cleanUpSession_IniList(const std::string& _sessionID);
	void cleanUpSession_AliveList(const std::string& _sessionID);
	
	void workerServiceStarter(void);
	void workerServiceInitializer(void);
	void workerHealthCheck(void);
	void workerServiceStopper(void);

	std::string												m_servicesIpAddress;
	std::string												m_lastError;
	std::string												m_siteID;
	
	bool													m_isShuttingDown;

	std::thread *											m_threadServiceStarter;
	std::thread *											m_threadServiceInitializer;
	std::thread *											m_threadHealthCheck;
	std::thread *											m_threadServiceStopper;

	std::atomic_bool m_generalWait;

	std::map<SessionInformation, std::vector<Service *> *>	m_services;
	std::mutex												m_mutexServices;

	std::list<RequestedService>								m_requestedServices;
	std::mutex												m_mutexRequestedServices;

	std::list<InitializingService>							m_initializingServices;
	std::mutex												m_mutexInitializingServices;

	std::list<Service *>									m_stoppingServices;
	std::mutex												m_mutexStoppingServices;
};