//! @file RelayedMessageHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCommunication/RelayedMessageHandler.h"

ot::RelayedMessageHandler::RelayedMessageHandler() :
	m_lastMessageId(0)
{

}

std::string ot::RelayedMessageHandler::createResponseRequest(const std::string& _receiverUrl, const std::string& _message, uint64_t _messageId) const {
	return "response\n" + _receiverUrl + "\n" + std::to_string(_messageId) + "\n" + _message;
}

std::string ot::RelayedMessageHandler::createQueueRequest(const std::string& _receiverUrl, const std::string& _message) const {
	return "queue\n" + _receiverUrl + "\n0\n" + _message;
}

bool ot::RelayedMessageHandler::createExecuteRequest(const std::string& _receiverUrl, const std::string& _message, std::string& _request, uint64_t& _messageId) {
	// Get next free message id
	if (!this->nextMessageId(_messageId)) {
		return false;
	}

	// Prepare the request body
	_request = "execute\n" + _receiverUrl + "\n" + std::to_string(_messageId) + "\n" + _message;

	// Set response data
	std::lock_guard<std::mutex> lock(m_mutex);
	m_responseData[_messageId] = std::make_pair(false, std::string());

	return true;
}

std::string ot::RelayedMessageHandler::createControlRequest(const std::string& _controlCommand) const {
	return "control\n\n0\n" + _controlCommand;
}

ot::RelayedMessageHandler::Request ot::RelayedMessageHandler::requestReceived(const std::string& _request) {
	Request request;
	request.messageId = 0;

	// Parse message type
	size_t ix = _request.find('\n');
	if (ix == std::string::npos) {
		OT_LOG_E("Invalid request received: No message type");
		return request;
	}

	std::string messageType = _request.substr(0, ix);
	if (messageType == "response") {
		request.messageType = MessageType::Response;
	}
	else if (messageType == "execute") {
		request.messageType = MessageType::Execute;
	}
	else if (messageType == "queue") {
		request.messageType = MessageType::Queue;
	}
	else if (messageType == "control") {
		request.messageType = MessageType::Control;
	}
	else {
		OT_LOG_E("Invalid request received: Unknown message type \"" + messageType + "\"");
		return request;
	}

	// Parse receiver url
	size_t ix2 = _request.find('\n', ix + 1);
	if (ix2 == std::string::npos) {
		OT_LOG_E("Invalid request received: No receiver url");
		return request;
	}
	request.receiverUrl = _request.substr(ix + 1, ix2 - (ix + 1));
	
	// Parse message id
	ix = ix2;
	ix2 = _request.find('\n', ix + 1);
	if (ix2 == std::string::npos) {
		OT_LOG_E("Invalid request received: No message id");
		return request;
	}
	std::string messageIdString = _request.substr(ix + 1, ix2 - (ix + 1));
	bool failed = false;
	request.messageId = String::toNumber<uint64_t>(messageIdString, failed);
	if (failed) {
		OT_LOG_E("Invalid request received: Invalid message id format: \"" + messageIdString + "\"");
		request.messageId = 0;
		return request;
	}

	// Get message
	request.message = _request.substr(ix2 + 1);

	// If this is a response message we store the response data
	if (request.messageType == MessageType::Response) {
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_responseData.find(request.messageId);
		if (it == m_responseData.end()) {
			OT_LOG_E("Received response message for unknown message id (" + std::to_string(request.messageId) + ")");
			request.messageId = 0;
			return request;
		}

		// Store response data
		it->second = std::make_pair(true, std::move(request.message));
		request.messageId = 0;
	}

	return request;
}

bool ot::RelayedMessageHandler::isWaitingForResponse(uint64_t _messageId) {
	std::lock_guard<std::mutex> lock(m_mutex);
	const auto it = m_responseData.find(_messageId);
	if (it != m_responseData.end()) {
		return !it->second.first;
	}
	else {
		return false;
	}
}

bool ot::RelayedMessageHandler::anyWaitingForResponse() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return !m_responseData.empty();
}

std::string ot::RelayedMessageHandler::grabResponse(uint64_t _messageId) {
	std::string response;

	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = m_responseData.find(_messageId);
	if (it == m_responseData.end()) {
		OT_LOG_E("Could not find response data for message id (" + std::to_string(_messageId) + ")");
	}
	else {
		response = std::move(it->second.second);
		m_responseData.erase(it);
	}
	
	return response;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ot::RelayedMessageHandler::nextMessageId(uint64_t& _messageId) {
	std::lock_guard<std::mutex> lock(m_mutex);

	_messageId = m_lastMessageId + 1;
	bool firstIteration = true;
	while (_messageId != 0) {
		if (m_responseData.find(_messageId) == m_responseData.end()) {
			m_lastMessageId = _messageId;
			return true;
		}
		_messageId++;

		if (_messageId == 0 && firstIteration) {
			firstIteration = false;
			_messageId = 1;
		}
	}

	OT_LOG_E("Could not find next free message id");
	return false;
}
