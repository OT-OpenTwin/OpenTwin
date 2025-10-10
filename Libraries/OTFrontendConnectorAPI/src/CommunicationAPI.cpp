//! @file CommunicationAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/LogDispatcher.h"
#include "OTFrontendConnectorAPI/CommunicationAPI.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Public API

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
		throw Exception::ObjectNotFound("CommunicationAPI instance not available");
	}
	return instance;
}

ot::CommunicationAPI*& ot::CommunicationAPI::getInstanceReference() {
	static CommunicationAPI* g_instance = { nullptr };
	return g_instance;
}