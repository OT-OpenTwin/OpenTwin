// @otlicense
// File: SequenceDiaAbstractCall.cpp
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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Diagram/SequenceDiaAbstractCall.h"

std::string ot::SequenceDiaAbstractCall::callTypeToString(CallType _type) {
	switch (_type) {
	case CallType::DirectCall: return "DirectCall";
	case CallType::SelfCall: return "SelfCall";
	default:
		OT_LOG_EAS("Unknown CallType enum value (" + std::to_string(static_cast<int>(_type)) + ")");
		return "Unknown";
	}
}

ot::SequenceDiaAbstractCall::CallType ot::SequenceDiaAbstractCall::stringToCallType(const std::string& _str) {
	if (_str == "DirectCall") return CallType::DirectCall;
	if (_str == "SelfCall") return CallType::SelfCall;
	else {
		OT_LOG_EAS("Unknown CallType string value \"" + _str + "\"");
		return CallType::DirectCall;
	}
}

void ot::SequenceDiaAbstractCall::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember(JsonString(callTypeJsonKey(), _allocator), JsonString(callTypeToString(getCallType()), _allocator), _allocator);
	_jsonObject.AddMember("Test", JsonString(m_text, _allocator), _allocator);
}

void ot::SequenceDiaAbstractCall::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	OTAssert(stringToCallType(json::getString(_jsonObject, callTypeJsonKey())) == getCallType(), "CallType mismatch when deserializing SequenceDiaAbstractCall");
	m_text = json::getString(_jsonObject, "Test");
}