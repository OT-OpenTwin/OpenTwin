// @otlicense

//! @file ModelAPIManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
