// @otlicense
// File: SequenceDiaRefCfg.cpp
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
#include "OTGui/Diagram/SequenceDiaRefCfg.h"

ot::SequenceDiaRefCfg::SequenceDiaRefCfg(const ConstJsonObject& _jsonObject) 
	: SequenceDiaRefCfg()
{
	this->setFromJsonObject(_jsonObject);
}

void ot::SequenceDiaRefCfg::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_jsonObject.AddMember("Function", JsonString(m_function, _allocator), _allocator);
	_jsonObject.AddMember("FilePath", JsonString(m_filePath, _allocator), _allocator);
	_jsonObject.AddMember("LineNr", m_lineNr, _allocator);
}

void ot::SequenceDiaRefCfg::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_name = json::getString(_jsonObject, "Name");
	m_function = json::getString(_jsonObject, "Function");
	m_filePath = json::getString(_jsonObject, "FilePath");
	m_lineNr = json::getUInt64(_jsonObject, "LineNr");
}
