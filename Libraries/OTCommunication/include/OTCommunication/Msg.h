// @otlicense
// File: Msg.h
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

#include "OTSystem/Flags.h"
#include "OTCommunication/CommunicationAPIExport.h"
#include "OTCommunication/CommunicationTypes.h"

 // C++ header
#include <string>

namespace ot {
	namespace msg {

		//! @brief Default OpenTwin message send timeout (30 seconds).
		static const int defaultTimeout = 30000;

		enum RequestFlag {
			NoRequestFlags    = 0 << 0,
			IsShutdownMessage = 1 << 0,
			CreateLogMessage  = 1 << 1,
			ExitOnFail        = 1 << 2,

			DefaultFlagsNoExit = CreateLogMessage,
			DefaultFlags       = DefaultFlagsNoExit | ExitOnFail
		};
		typedef ot::Flags<RequestFlag> RequestFlags;

		
		OT_COMMUNICATION_API_EXPORT const std::string getLastError();

		//! @brief Will send the provided message to the microservice framework. Returns true if succeeded
		//! @param _senderIP The sender IP address
		//! @param _receiverIP The receiver IP address
		//! @param _message The message to send
		//! @param _response Here the response will be written to
		OT_COMMUNICATION_API_EXPORT bool send(const std::string& _senderIP, const std::string& _receiverIP, ot::MessageType _type, const std::string& _message, std::string& _response, int _timeout = 0, const RequestFlags& _flags = msg::DefaultFlags);

		//! @brief Will send the provided message to the microservice framework. The massage will be send asynchronously
		//! @param _senderIP The sender IP address
		//! @param _receiverIP The receiver IP address
		//! @param _message The message to send
		//! @param _response Here the response will be written to
		OT_COMMUNICATION_API_EXPORT void sendAsync(const std::string& _senderIP, const std::string& _receiverIP, ot::MessageType _type, const std::string& _message, int _timeout = 0, const RequestFlags& _flags = msg::DefaultFlags);
	}
}

OT_ADD_FLAG_FUNCTIONS(ot::msg::RequestFlag, ot::msg::RequestFlags)