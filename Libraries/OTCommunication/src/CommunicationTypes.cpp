// @otlicense
// File: CommunicationTypes.cpp
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

#include "OTCore/LogDispatcher.h"
#include "OTCommunication/CommunicationTypes.h"

ot::LogFlag ot::messageTypeToLogFlag(ot::MessageType _type) {
	switch (_type)
	{
	case ot::QUEUE: return ot::QUEUED_INBOUND_MESSAGE_LOG;
	case ot::EXECUTE: return ot::INBOUND_MESSAGE_LOG;
	case ot::EXECUTE_ONE_WAY_TLS: return ONEWAY_TLS_INBOUND_MESSAGE_LOG;
	default:
		OTAssert(0, "Unknown message type");
		return ot::INBOUND_MESSAGE_LOG;
	}
}

std::string ot::toString(ot::MessageType _type) {
	switch (_type)
	{
	case ot::QUEUE: return "Queue";
	case ot::EXECUTE: return "Execute";
	case ot::EXECUTE_ONE_WAY_TLS: return "ExecuteOneWayTls";
	case ot::SECURE_MESSAGE_TYPES:
	case ot::ALL_MESSAGE_TYPES:
		OTAssert(0, "Not supported message type");
		OT_LOG_E("Not supported message type");
		break;
	default:
		OTAssert(0, "Unknown message type");
		OT_LOG_E("Unknown message type");
		break;
	}
	return "Execute";
}

ot::MessageType ot::stringToMessageTypeFlag(const std::string& _type) {
	if (_type == toString(ot::QUEUE)) return ot::QUEUE;
	else if (_type == toString(ot::EXECUTE)) return ot::EXECUTE;
	else if (_type == toString(ot::EXECUTE_ONE_WAY_TLS)) return ot::EXECUTE_ONE_WAY_TLS;
	else {
		OTAssert(0, "Unknown message type");
		OT_LOG_E("Unknown message type");
		return ot::EXECUTE;
	}
}