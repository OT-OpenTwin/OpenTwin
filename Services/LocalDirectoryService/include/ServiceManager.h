// @otlicense
// File: ServiceManager.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// LDS header
#include "Service.h"
#include "RequestedService.h"
#include "SessionInformation.h"
#include "ServiceStartupData.h"

// OpenTwin header
#include "OTSystem/Network.h"
#include "OTSystem/RunResult.h"
#include "OTSystem/PortManager.h"
#include "OTCore/JSON.h"
#include "OTCommunication/LDSDebugInfo.h"
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

	void getDebugInformation(ot::LDSDebugInfo& _info);

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

	const std::string& lastError() const { return m_lastError; };

	void setSiteID(const std::string& _siteID) { m_siteID = _siteID; };
	const std::string& siteID() const { return m_siteID; };

	void getSessionInformation(ot::JsonArray& sessionInfo, ot::JsonAllocator& allocator);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:

	void runThreads();
	void stopThreads();
	
	void serviceStartFailed(const ot::ServiceInitData& _serviceInformation);
	void sendInitializeMessage(Service&& _info);
	void serviceInitializeFailed(Service&& _info);

	std::list<Service>& sessionServices(const SessionInformation& _sessionInformation);

	bool restartServiceAfterCrash(Service& _service);
	void notifyServiceShutdownCompleted(const Service& _service);
	void notifySessionEmergencyShutdown(const Service& _crashedService);

	std::string logInfo(const Service& _service) const;
	std::string logInfo(const RequestedService& _service) const;
	std::string logInfo(const ot::ServiceInitData& _serviceInfo) const;

	ot::LDSDebugInfo::ServiceInfo toDebugInfo(const ot::ServiceInitData& _serviceInfo) const;
	ot::LDSDebugInfo::ServiceInfo toDebugInfo(const Service& _service) const;
	ot::LDSDebugInfo::ServiceInfo toDebugInfo(const RequestedService& _service) const;
	void addDebugInfo(const ServiceStartupData& _startupData, ot::LDSDebugInfo::ServiceInfo& _info) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Cleanup

	void cleanUp(const std::string& _sessionID);

	//! @brief Clean up session related information from requested services list.
	//! @param _sessionID Session info to clean up.
	void cleanUpRequestedList(const std::string& _sessionID);

	//! @brief Send shutdown message to all services that are currently initializing in the given session and clean up information
	//! @warning The initializing services mutex is expected to be locked when calling this function.
	//! @param _sessionID Session info to clean up.
	void cleanUpIniList(const std::string& _sessionID);

	//! @brief Move all services in the given session from the alive list to the stopping list and clean up information.
	//! @warning The initializing services mutex is expected to be locked when calling this function.
	//! @param _sessionID Session info to clean up.
	void cleanUpAliveList(const std::string& _sessionID);
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void workerServiceStarter();
	void workerServiceInitializer();
	void workerHealthCheck();
	void workerHealthCheckFail();
	void workerServiceStopper();

	ot::PortManager                                      m_portManager;

	std::string                                          m_servicesIpAddress;
	std::string                                          m_lastError;
	std::string                                          m_siteID;
	
	std::atomic_bool                                     m_workerRunning;

	std::list<std::thread*>                              m_threadsServiceStarter;
	std::list<std::thread*>                              m_threadsServiceInitializer;
	std::thread *                                        m_threadHealthCheck;
	std::thread *                                        m_threadServiceStopper;
	std::thread *                                        m_threadHealthCheckFail;

	std::chrono::seconds								 m_serviceCheckAliveFrequency;

	std::map<SessionInformation, std::list<Service>>     m_sessions;
	std::mutex                                           m_mutexServices;

	std::list<RequestedService>                          m_requestedServices;
	std::mutex                                           m_mutexRequestedServices;

	std::list<Service>                                   m_initializingServices;
	std::mutex                                           m_mutexInitializingServices;

	std::list<std::pair<ot::RunResult, Service>>         m_failedServices;
	std::mutex                                           m_mutexFailedServices;

	std::mutex                                           m_mutexStoppingServices;
	std::list<Service>                                   m_newStoppingServices;

	std::mutex                                           m_mutexCurrentStoppingServices;
	std::list<Service>                                   m_currentStoppingServices;
};