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
#include "OTCore/ServiceBase.h"

// std header
#include <string>

class GlobalDirectoryService : public ot::ServiceBase {
public:
	enum ConnectionStatus {
		Connected,
		Disconnected,
		WaitingForConnection
	};

	GlobalDirectoryService(void);
	virtual ~GlobalDirectoryService();
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Connection

	bool isConnected(void) const { return m_connectionStatus == Connected; };
	ConnectionStatus connectionStatus(void) const { return m_connectionStatus; };

	//! @brief Will set the service URL and start a worker thread to register at the GDS.
	//! The connection status will be set to WaitingForConnection until the registration is completed.
	//! @param _url GDS url to set.
	void connect(const std::string& _url);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Worker methods

private:
	//! @brief Worker thread entry.
	//! Will send a register request to the GDS.
	//! If the registration fails the application will exit.
	void registerAtGlobalDirectoryService(void);

	//! @brief Health check function.
	//! Will be called by the register method after successful registration.
	//! In case that the GDS crashed the Application::globalDirectoryServiceCrashed method will be called.
	void healthCheck(void);

	//! @brief Will add the current system values to the provided JSON document.
	void addSystemValues(ot::JsonDocument& _jsonDocument);

	bool				m_isShuttingDown;
	ConnectionStatus	m_connectionStatus;

	GlobalDirectoryService(GlobalDirectoryService&) = delete;
	GlobalDirectoryService& operator = (GlobalDirectoryService&) = delete;
};