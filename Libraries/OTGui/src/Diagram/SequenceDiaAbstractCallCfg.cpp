// @otlicense
// File: SequenceDiaAbstractCallCfg.cpp
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
#include "OTGui/Diagram/SequenceDiaAbstractCallCfg.h"

void ot::SequenceDiaAbstractCallCfg::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember(JsonString(callClassNameJsonKey(), _allocator), JsonString(getClassName(), _allocator), _allocator);
	
	_jsonObject.AddMember("Text", JsonString(m_text, _allocator), _allocator);
}

void ot::SequenceDiaAbstractCallCfg::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	OTAssert(json::getString(_jsonObject, callClassNameJsonKey()) == getClassName(), "CallType mismatch when deserializing SequenceDiaAbstractCall");

	m_text = json::getString(_jsonObject, "Text");
}