// @otlicense
// File: GlobalSessionService.h
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

// LSS header
#include "GSSRegistrationInfo.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/CommunicationTypes.h"

// std header
#include <mutex>

namespace std { class thread; }

class GlobalSessionService {
	OT_DECL_NOCOPY(GlobalSessionService)
	OT_DECL_NOMOVE(GlobalSessionService)
public:
	enum ConnectionStatus {
		Connected,
		Disconnected,
		CheckingNewConnection
	};

	GlobalSessionService();
	virtual ~GlobalSessionService();

	//! @brief Connects to the Global Session Service at the given URL.
	//! Will change the connection status to CheckingNewConnection and start a health check thread.
	//! The LSS information will be updated on success.
	//! @warning This function will block until the connection is established or fails.
	//! @param _url Global Directory Service url.
	bool connect(const std::string& _url);
	
	std::string getServiceUrl();

	bool isConnected();

	bool confirmSession(const std::string& _sessionID, const std::string& _userName);

	bool notifySessionShutdownCompleted(const std::string& _sessionID);

	void setGDSUrl(const std::string& _gdsUrl);

	GSSRegistrationInfo getRegistrationResult();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator);

private:
	bool ensureConnection();
	bool sendMessage(ot::MessageType _messageType, const std::string& _message, std::string& _response);

	void startHealthCheck();
	void stopHealthCheck();

	void healthCheck();

	std::mutex          m_mutex;
	std::atomic_bool    m_healthCheckRunning;

	ot::ServiceBase     m_serviceInfo;
	ConnectionStatus    m_connectionStatus;
	std::thread*        m_workerThread;
	GSSRegistrationInfo m_registrationResult;
};