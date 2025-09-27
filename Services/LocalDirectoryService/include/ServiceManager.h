//! @file ServiceManager.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "Service.h"
#include "RequestedService.h"
#include "SessionInformation.h"
#include "ServiceStartupData.h"

// OpenTwin header
#include "OTSystem/Network.h"
#include "OTSystem/PortManager.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ServiceInitData.h"

// std header
#include <map>
#include <list>
#include <mutex>
#include <string>
#include <vector>

namespace std { class thread; }

//! @brief The ServiceManager class is responsible for managing all services that are running in the LDS.
//! It will start, stop and monitor the services. It will also provide information about the services.
class ServiceManager {
public:
	enum class RequestResult {
		Success,
		FailedOnStart,
		FailedOnPing
	};

	ServiceManager();
	virtual ~ServiceManager();

	//! @brief Add the object contents to the provided JSON object
	//! @param _document The JSON document (used to get the allocator)
	//! @param _object The JSON object to add the contents to
	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service control

	//! @brief Will attempt to start a new service
	//! @param _sessionInformation Information about the session where the service should be connected to
	//! @param _serviceInformation Information about the service that should be started
	//! @param _serviceURL The URL where the service will be running will be written here
	RequestResult requestStartService(const ot::ServiceInitData& _serviceInformation);

	RequestResult requestStartRelayService(const ot::ServiceInitData& _serviceInformation, std::string& _relayServiceURL, std::string& _websocketUrl);

	//! @brief Will mark all services in the session as expected to shut down.
	//! Services that are currently requested will be removed from the requested list.
	//! Sercices that are currently initializing will be shut down and removed from the initializing list.
	//! @param _sessionID Session that is expected to shut down.
	void sessionClosing(const std::string& _sessionID);

	//! @brief Will clean up all services in the session.
	//! @param _sessionID 
	void sessionClosed(const std::string& _sessionID);

	void serviceDisconnected(const std::string& _sessionID, ot::serviceID_t _serviceID);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	void addPortRange(ot::port_t _start, ot::port_t _end);

	void setServiceIP(const std::string& _ip) { m_servicesIpAddress = _ip; };

	const std::string& lastError(void) const { return m_lastError; };

	void setSiteID(const std::string& _siteID) { m_siteID = _siteID; };
	const std::string& siteID(void) const { return m_siteID; };

	void getSessionInformation(ot::JsonArray& sessionInfo, ot::JsonAllocator& allocator);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:

	void runThreads(void);
	
	void serviceStartFailed(const ot::ServiceInitData& _serviceInformation);
	void sendInitializeMessage(Service&& _info);
	void serviceInitializeFailed(Service&& _info);

	std::vector<Service> * sessionServices(const SessionInformation& _sessionInformation);

	bool restartServiceAfterCrash(const Service& _service);
	void notifyServiceShutdownCompleted(const Service& _service);
	void notifySessionEmergencyShutdown(const Service& _crashedService);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Cleanup

	//! @brief Clean up session related information from requested services list.
	//! @param _sessionID Session info to clean up.
	void cleanUpSession_RequestedList(const std::string& _sessionID);

	//! @brief Send shutdown message to all services that are currently initializing in the given session and clean up information
	//! @warning The initializing services mutex is expected to be locked when calling this function.
	//! @param _sessionID Session info to clean up.
	void cleanUpSession_IniList(const std::string& _sessionID);

	//! @brief Move all services in the given session from the alive list to the stopping list and clean up information.
	//! @warning The initializing services mutex is expected to be locked when calling this function.
	//! @param _sessionID Session info to clean up.
	void cleanUpSession_AliveList(const std::string& _sessionID);
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void workerServiceStarter(void);
	void workerServiceInitializer(void);
	void workerHealthCheck(void);
	void workerServiceStopper(void);

	ot::PortManager                                      m_portManager;

	std::string                                          m_servicesIpAddress;
	std::string                                          m_lastError;
	std::string                                          m_siteID;
	
	bool                                                 m_isShuttingDown;

	std::thread *                                        m_threadServiceStarter;
	std::thread *                                        m_threadServiceInitializer;
	std::thread *                                        m_threadHealthCheck;
	std::thread *                                        m_threadServiceStopper;

	std::atomic_bool                                     m_generalWait;

	std::chrono::seconds								 m_serviceCheckAliveFrequency;

	std::map<SessionInformation, std::vector<Service>*>  m_services;
	std::mutex                                           m_mutexServices;

	std::list<RequestedService>                          m_requestedServices;
	std::mutex                                           m_mutexRequestedServices;

	std::list<Service>                                   m_initializingServices;
	std::mutex                                           m_mutexInitializingServices;

	std::list<Service>                                   m_stoppingServices;
	std::mutex                                           m_mutexStoppingServices;
};