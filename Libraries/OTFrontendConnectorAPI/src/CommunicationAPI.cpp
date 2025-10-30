// @otlicense
// File: CommunicationAPI.cpp
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
#include "OTSystem/Exception.h"
#include "OTCore/LogDispatcher.h"
#include "OTFrontendConnectorAPI/CommunicationAPI.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Public API

void ot::CommunicationAPI::setDefaultConnectorServiceUrl(const std::string& _url) {
	CommunicationAPI::instance()->m_defaultConnectorServiceUrl = _url;
}

bool ot::CommunicationAPI::sendExecute(const std::string& _message, std::string& _response) {
	CommunicationAPI* instance = CommunicationAPI::instance();
	if (instance->m_defaultConnectorServiceUrl.empty()) {
		OT_LOG_EA("Default connector service URL not set");
		throw Exception::InvalidArgument("Default connector service URL not set");
	}
	return instance->sendExecuteAPI(instance->m_defaultConnectorServiceUrl, _message, _response);
}

bool ot::CommunicationAPI::sendQueue(const std::string& _message) {
	CommunicationAPI* instance = CommunicationAPI::instance();
	if (instance->m_defaultConnectorServiceUrl.empty()) {
		OT_LOG_EA("Default connector service URL not set");
		throw Exception::InvalidArgument("Default connector service URL not set");
	}
	return instance->sendQueueAPI(instance->m_defaultConnectorServiceUrl, _message);
}

bool ot::CommunicationAPI::sendExecute(const std::string& _receiverUrl, const std::string& _message, std::string& _response) {
	return CommunicationAPI::instance()->sendExecuteAPI(_receiverUrl, _message, _response);
}

bool ot::CommunicationAPI::sendQueue(const std::string& _receiverUrl, const std::string& _message) {
	return CommunicationAPI::instance()->sendQueueAPI(_receiverUrl, _message);
}

bool ot::CommunicationAPI::sendExecuteToService(const BasicServiceInformation& _serviceInfo, const std::string& _message, std::string& _response) {
	return CommunicationAPI::instance()->sendExecuteToServiceAPI(_serviceInfo, _message, _response);
}

bool ot::CommunicationAPI::sendQueueToService(const BasicServiceInformation& _serviceInfo, const std::string& _message) {
	return CommunicationAPI::instance()->sendQueueToServiceAPI(_serviceInfo, _message);
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::CommunicationAPI::CommunicationAPI() {
	if (getInstanceReference()) {
		OT_LOG_EA("CommunicationAPI instance already exists");
		throw Exception::ObjectAlreadyExists("CommunicationAPI instance already exists");
	}
	getInstanceReference() = this;
}

ot::CommunicationAPI::~CommunicationAPI() {
	OTAssert(getInstanceReference(), "CommunicationAPI instance does not exist");
	getInstanceReference() = nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Instance management

ot::CommunicationAPI* ot::CommunicationAPI::instance() {
	ot::CommunicationAPI* instance = getInstanceReference();
	if (!instance) {
		OT_LOG_EA("CommunicationAPI instance not available");
		throw Exception::ObjectNotFound("CommunicationAPI instance not available");
	}
	return instance;
}

ot::CommunicationAPI*& ot::CommunicationAPI::getInstanceReference() {
	static CommunicationAPI* g_instance = { nullptr };
	return g_instance;
}