// @otlicense
// File: GlobalDirectoryService.h
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

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ServiceInitData.h"
#include "OTCommunication/CommunicationTypes.h"

// std header
#include <list>
#include <mutex>
#include <string>

namespace std { class thread; }

//! @brief The GlobalDirectoryService handles all communication with the GDS.
//! It is responsible for connecting to the GDS, starting services, and managing the connection status.
class GlobalDirectoryService {
	OT_DECL_NOCOPY(GlobalDirectoryService)
	OT_DECL_NOMOVE(GlobalDirectoryService)
public:
	enum ConnectionStatus {
		Connected,
		Disconnected,
		CheckingNewConnection
	};

	GlobalDirectoryService();
	virtual ~GlobalDirectoryService();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	//! @brief Connects to the Global Directory Service at the given URL.
	//! Will change the connection status to CheckingNewConnection and start a health check thread.
	//! @param _url Global Directory Service url.
	bool connect(const std::string& _url, bool _waitForConnection);

	//! @brief Checks if the Global Directory Service is connected.
	//! Will return true if the connection status is Connected, otherwise false.
	bool isConnected(void);

	//! @brief Requests the GDS to start a service.
	//! The service start will be performed asynchronously.
	//! @param _serviceInformation Information about the service to start.
	//! @param _sessionID The ID of the session that requested the service start.
	//! @param _lssUrl The URL of the LSS that requested the service to start.
	//! @return Returns true if the service was successfully requested to start, false otherwise.
	bool requestToStartService(const ot::ServiceInitData& _serviceInformation);

	//! @brief Requests the GDS to start multiple services.
	//! The service start will be performed asynchronously.
	//! @param _serviceInformation List of services to start.
	//! @param _sessionID The ID of the session that requested the service start.
	//! @param _lssUrl The URL of the LSS that requested the services to start.
	//! @return Returns true if the services were successfully requested to start, false otherwise.
	bool requestToStartServices(const ot::ServiceInitData& _generalData, const std::list<ot::ServiceBase>& _serviceInformation);

	//! @brief Requests the GDS to start a relay service.
	//! The relay service start will be performed synchronously.
	//! @param _sessionID The ID of the session that requested the relay service.
	//! @param _lssUrl The URL of the LSS that requested the relay service to start.
	//! @param _websocketURL Will be set to the websocket URL of the relay service on success.
	//! @param _relayServiceURL Will be set to the URL of the relay service on success.
	//! @return Returns true if the relay service was successfully started, false otherwise.
	bool startRelayService(const ot::ServiceInitData& _serviceInformation, std::string& _relayServiceURL, std::string& _websocketURL);
	
	void notifySessionShuttingDown(const std::string& _sessionID);

	std::string getServiceUrl();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator);

private:
	void startHealthCheck();
	void stopHealthCheck();

	//! @brief Health check thread function.
	void healthCheck();

	bool ensureConnection();
	bool sendMessage(ot::MessageType _messageType, const std::string& _message, std::string& _response);

	ConnectionStatus m_connectionStatus;  //! @brief GDS connection status.
	ot::ServiceBase m_serviceInfo;

	std::mutex       m_mutex;             //! @brief Mutex to protect the connection status and service URL.
	std::thread*     m_healthCheckThread; //! @brief Thread for the health check.
	std::atomic_bool m_healthCheckRunning;    //! @brief Flag to indicate if the worker thread should be shutting down.
};