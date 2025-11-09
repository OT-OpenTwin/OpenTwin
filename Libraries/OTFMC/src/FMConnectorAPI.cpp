// @otlicense
// File: FMConnectorAPI.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTFMC/FileManager.h"
#include "OTFMC/FMConnectorAPI.h"

bool ot::FMConnectorAPI::initialize() {
	return FMConnectorAPI::instance().initializePrivate();
}

void ot::FMConnectorAPI::shutdown() {
	OT_LOG_D("Shutting down FileManagerConnector API");
	FMConnectorAPI::instance().shutdownPrivate();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ot::FMConnectorAPI::initializePrivate() {
	if (m_initialized) {
		OT_LOG_W("DataManagerConnector API already initialized");
		return true;
	}

	// Initialize DataManager
	m_manager.reset(new FileManager);

	m_initialized = true;
	return true;
}

void ot::FMConnectorAPI::shutdownPrivate() {
	if (!m_initialized) {
		return;
	}

	// Shutdown DataManager
	m_manager.reset();
	m_initialized = false;
}

ot::FMConnectorAPI& ot::FMConnectorAPI::instance() {
	static FMConnectorAPI g_instance;
	return g_instance;
}

ot::FMConnectorAPI::FMConnectorAPI()
	: m_initialized(false)
{

}

ot::FMConnectorAPI::~FMConnectorAPI() {
}
