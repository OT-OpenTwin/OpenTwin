// @otlicense
// File: DialogCfg.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/DialogCfg.h"

ot::DialogCfg::DialogCfg(DialogFlags _flags)
	: m_flags(_flags), m_initialSize(-1, -1), m_minSize(-1, -1), m_maxSize(-1, -1)
{}

ot::DialogCfg::DialogCfg(const std::string& _title, DialogFlags _flags)
	: m_flags(_flags), m_title(_title), m_initialSize(-1, -1), m_minSize(-1, -1), m_maxSize(-1, -1)
{}

void ot::DialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("Flags", static_cast<uint64_t>(m_flags), _allocator);

	JsonObject iniSizeObj;
	m_initialSize.addToJsonObject(iniSizeObj, _allocator);
	_object.AddMember("IniSize", iniSizeObj, _allocator);

	JsonObject minSizeObj;
	m_minSize.addToJsonObject(minSizeObj, _allocator);
	_object.AddMember("MinSize", minSizeObj, _allocator);

	JsonObject maxSizeObj;
	m_maxSize.addToJsonObject(maxSizeObj, _allocator);
	_object.AddMember("MaxSize", maxSizeObj, _allocator);
}

void ot::DialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_flags = DialogFlags(json::getUInt64(_object, "Flags"));
	m_initialSize.setFromJsonObject(json::getObject(_object, "IniSize"));
	m_minSize.setFromJsonObject(json::getObject(_object, "MinSize"));
	m_maxSize.setFromJsonObject(json::getObject(_object, "MaxSize"));
}