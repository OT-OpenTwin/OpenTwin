// @otlicense
// File: RelayedMessageHandler.h
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
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <map>
#include <mutex>
#include <string>

namespace ot {

	//! @brief This class is used to create and handle relayed messages.
	//! The class is thread safe.
	class OT_COMMUNICATION_API_EXPORT RelayedMessageHandler {
		OT_DECL_NOCOPY(RelayedMessageHandler)
		OT_DECL_DEFMOVE(RelayedMessageHandler)
	public:
		enum MessageType {
			Response,
			Execute,
			Queue,
			Control
		};

		struct Request {
			std::string receiverUrl;
			MessageType messageType;
			uint64_t messageId;
			std::string message;
		};

		RelayedMessageHandler();
		~RelayedMessageHandler() {};

		//! @brief Creates a response request message that can be sent to the receiver.
		//! @param _receiverUrl Url of initial message receiver.
		//! @param _message Response message to send.
		//! @param _messageId Message id of the initial message to which this is a response.
		//! @return The created response request message.
		std::string createResponseRequest(const std::string& _receiverUrl, const std::string& _message, uint64_t _messageId) const;

		//! @brief Creates a queue request message that can be sent to the receiver.
		//! @param _receiverUrl Url of the receiver of the message.
		//! @param _message Message to send.
		//! @return The created queue request message.
		std::string createQueueRequest(const std::string& _receiverUrl, const std::string& _message) const;

		//! @brief Creates a execute request message that can be sent to the receiver.
		//! The message id will be registered internally so that the response can be grabbed later.
		//! @param _receiverUrl Url of the receiver of the message.
		//! @param _message Message to send.
		//! @param _request Will contain the created request message on success.
		//! @param _messageId Message id to use for the message. This will be updated to the actual message id used if the request expects a response.
		//! @return False if no message id could be created (e.g. because the maximum number of message ids has been reached), true otherwise.
		bool createExecuteRequest(const std::string& _receiverUrl, const std::string& _message, std::string& _request, uint64_t& _messageId);

		//! @brief Creates a control request message that can be sent to the relayed message handler.
		//! @param _controlCommand Control command to send.
		//! @return The created control request message.
		std::string createControlRequest(const std::string& _controlCommand) const;

		//! @brief Call this function when a request message was received.
		//! If the message is a response message, the response data will be stored internally and can be grabbed later.
		//! @param _request The received request message.
		//! @return The parsed request. If the message is a response message or the deserialization failed, the message id will be 0.
		Request requestReceived(const std::string& _request);

		//! @brief Check if a response for the provided message id is still pending.
		//! @param _messageId Message id to check.
		//! @return True if a response is still pending, false otherwise.
		bool isWaitingForResponse(uint64_t _messageId);

		//! @brief Check if there are any messages waiting for a response.
		bool anyWaitingForResponse();

		//! @brief Grab the response data for the provided message id.
		std::string grabResponse(uint64_t _messageId);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		//! @brief Get the next free message id.
		//! @param _messageId Will contain the next free message id on success.
		//! @return False if no message id could be created (e.g. because the maximum number of message ids has been reached), true otherwise.
		bool nextMessageId(uint64_t& _messageId);

		std::mutex m_mutex;
		uint64_t m_lastMessageId;
		std::map<uint64_t, std::pair<bool, std::string>> m_responseData;
	};

}