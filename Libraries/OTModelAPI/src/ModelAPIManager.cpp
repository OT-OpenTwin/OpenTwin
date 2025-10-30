// @otlicense
// File: ModelAPIManager.cpp
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
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelAPI/ModelAPIManager.h"

void ot::ModelAPIManager::setModelServiceURL(const std::string& _url) {
	ModelAPIManager::instance().m_serviceUrl = _url;
}

const std::string& ot::ModelAPIManager::getModelServiceURL(void) {
	return ModelAPIManager::instance().m_serviceUrl;
}
	
bool ot::ModelAPIManager::sendToModel(MessageType _messageType, const std::string& _message, std::string& _response) {
	const ModelAPIManager& manager = ModelAPIManager::instance();

	if (manager.m_serviceUrl.empty()) {
		OT_LOG_EA("Model service not connected");
		return false;
	}

	if (!msg::send("", manager.m_serviceUrl, _messageType, _message, _response)) {
		OT_LOG_EA("Failed to send message to model");
		return false;
	}
	else OT_ACTION_IF_RESPONSE_ERROR(_response) {
		OT_LOG_EAS(_response);
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(_response) {
		OT_LOG_EAS(_response);
		return false;
	}
	else {
		return true;
	}
	
}

bool ot::ModelAPIManager::sendToModel(MessageType _messageType, const JsonDocument& _doc, std::string& _response) {
	return ModelAPIManager::sendToModel(_messageType, _doc.toJson(), _response);
}

ot::ModelAPIManager& ot::ModelAPIManager::instance(void) {
	static ModelAPIManager g_instance;
	return g_instance;
}

ot::ModelAPIManager::ModelAPIManager() {}

ot::ModelAPIManager::~ModelAPIManager() {}
