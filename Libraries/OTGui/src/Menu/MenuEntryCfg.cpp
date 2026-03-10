// @otlicense
// File: MenuEntryCfg.cpp
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
#include "OTGui/Menu/MenuEntryCfg.h"

const std::string& ot::MenuEntryCfg::classNameJsonKey() {
	static std::string key("ClassName");
	return key;
}

void ot::MenuEntryCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(MenuEntryCfg::classNameJsonKey(), _allocator), JsonString(this->getClassName(), _allocator), _allocator);
}

void ot::MenuEntryCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	OTAssert(json::getString(_object, MenuEntryCfg::classNameJsonKey()) == this->getClassName(), "Invalid class name in JSON object for deserialization");
}
