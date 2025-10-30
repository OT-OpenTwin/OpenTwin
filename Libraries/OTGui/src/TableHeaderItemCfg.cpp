// @otlicense
// File: TableHeaderItemCfg.cpp
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
#include "OTGui/TableHeaderItemCfg.h"

ot::TableHeaderItemCfg::TableHeaderItemCfg() {}

ot::TableHeaderItemCfg::TableHeaderItemCfg(const std::string& _text) 
	: m_text(_text)
{

}

ot::TableHeaderItemCfg::TableHeaderItemCfg(const TableHeaderItemCfg& _other) 
	: m_text(_other.m_text)
{

}

ot::TableHeaderItemCfg::~TableHeaderItemCfg() {

}

ot::TableHeaderItemCfg& ot::TableHeaderItemCfg::operator = (const TableHeaderItemCfg& _other) {
	if (this == &_other) return *this;

	m_text = _other.m_text;
	
	return *this;
}

void ot::TableHeaderItemCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
}

void ot::TableHeaderItemCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_text = json::getString(_object, "Text");
}
