// @otlicense
// File: CommunicationTypes.h
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
#include "OTSystem/Flags.h"		// Add flag fuctions
#include "OTCore/LogDispatcher.h"	// LogFlag type

namespace ot {

	//! @brief The message types describes how a message should be delivered
	enum MessageType {
		DEFAULT_MESSAGE_TYPE            = 0 << 0, // Default message type.
		QUEUE							= 1 << 0,
		EXECUTE							= 1 << 1,
		EXECUTE_ONE_WAY_TLS				= 1 << 2,
		SECURE_MESSAGE_TYPES			= QUEUE | EXECUTE,
		ALL_MESSAGE_TYPES				= SECURE_MESSAGE_TYPES | EXECUTE_ONE_WAY_TLS
	};

	//! @brief The message types describes how a message should be delivered
	typedef ot::Flags<ot::MessageType> MessageTypeFlags;

	__declspec(dllexport) ot::LogFlag messageTypeToLogFlag(ot::MessageType _type);

	//! @brief Returns a string representation for the given message type flag
	//! @param _flag The message type flag
	__declspec(dllexport) std::string toString(MessageType _flag);

	//! @brief Returns the message type that is represented by the provided string
	//! @param _flag The string containing the message type flag
	__declspec(dllexport) MessageType stringToMessageTypeFlag(const std::string& _type);
}

OT_ADD_FLAG_FUNCTIONS(ot::MessageType);