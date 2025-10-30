// @otlicense
// File: GuiAPIManager.cpp
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
#include "OTCore/ThisService.h"
#include "OTCommunication/Msg.h"
#include "OTGuiAPI/GuiAPIManager.h"

ot::GuiAPIManager& ot::GuiAPIManager::instance(void) {
	static GuiAPIManager g_instance;
	return g_instance;
}

void ot::GuiAPIManager::frontendConnected(const ServiceBase& _serviceInformation) {
	m_frontend = _serviceInformation;
	if (m_frontend.getServiceURL().empty()) {
		OT_LOG_W("Provided frontend infromation does not contain a url");
	}
	else {
		OT_LOG_D("User frontend url set to \"" + m_frontend.getServiceURL() + "\"");
	}
}

void ot::GuiAPIManager::frontendDisconnected(void) {
	m_frontend.setServiceURL("");
}

bool ot::GuiAPIManager::sendQueuedRequestToFrontend(const std::string& _message) {
	if (!this->isFrontendConnected()) {
		OT_LOG_W("User frontend is not connected");
		return false;
	}

	std::string tmp;
	return msg::send(ThisService::instance().getServiceURL(), m_frontend.getServiceURL(), ot::QUEUE, _message, tmp);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

ot::GuiAPIManager::GuiAPIManager() {
	m_frontend.setServiceURL("");
}

ot::GuiAPIManager::~GuiAPIManager() {

}