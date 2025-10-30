// @otlicense
// File: Application.h
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
#include "ServiceManager.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ActionHandler.h"

// std header
#include <list>
#include <string>

class Application : public ot::ServiceBase, public ot::ActionHandler {
public:
	static Application& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public functions

	int initialize(const char * _ownURL, const char * _globalDirectoryServiceURL);

	//! @brief Handle the crash of the global directory service.
	//! @warning The crash of the GDS is not handled yet. The application will exit with the GDSNotRunning error code.
	void globalDirectoryServiceCrashed(void);
	
	ServiceManager& getServiceManager(void) { return m_serviceManager; }

	std::list<std::string> getSupportedServices(void) const;

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	ot::ReturnMessage handleStartNewService(ot::JsonDocument& _doc);
	ot::ReturnMessage handleStartNewRelayService(ot::JsonDocument& _doc);
	ot::ReturnMessage handleSessionClosing(ot::JsonDocument& _doc);
	ot::ReturnMessage handleSessionClosed(ot::JsonDocument& _doc);
	ot::ReturnMessage handleServiceClosed(ot::JsonDocument& _doc);
	std::string handleGetDebugInformation();
	std::string handleGetSystemInformation();
	void handleSetGlobalLogFlags(ot::JsonDocument& _doc);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Constructor/Destructor

	Application();
	virtual ~Application();

	ServiceManager				m_serviceManager;
	GlobalDirectoryService		m_globalDirectoryService;
	ot::SystemInformation		m_systemLoadInformation;
};
