/*
 *	msg.h
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

#include "OTCommunication/CommunicationAPIExport.h"
#include "OTCommunication/CommunicationTypes.h"

 // C++ header
#include <string>

namespace ot {
	namespace msg {

		//! @brief Will send the provided message to the microservice framework. Returns true if succeeded
		//! @param _senderIP The sender IP address
		//! @param _receiverIP The receiver IP address
		//! @param _message The message to send
		//! @param _response Here the response will be written to
		OT_COMMUNICATION_API_EXPORT bool send(
			const std::string &				_senderIP,
			const std::string &				_receiverIP,
			ot::MessageType					_type,
			const std::string &				_message,
			std::string &					_response,
			int								_timeout = 3000,
			bool							_shutdownMessage = false,
			bool							_createLogMessage = true
		);

		//! @brief Will send the provided message to the microservice framework. The massage will be send asynchronously
		//! @param _senderIP The sender IP address
		//! @param _receiverIP The receiver IP address
		//! @param _message The message to send
		//! @param _response Here the response will be written to
		OT_COMMUNICATION_API_EXPORT void sendAsync(
			const std::string &				_senderIP,
			const std::string &				_receiverIP,
			ot::MessageType					_type,
			const std::string &				_message,
			int								_timeout = 3000
		);
	}
}