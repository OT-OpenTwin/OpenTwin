// @otlicense
// File: StyleValue.cpp
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
#include "OTGui/StyleValue.h"

#define OT_JSON_MEMBER_Key "Key"
#define OT_JSON_MEMBER_Qss "Qss"
#define OT_JSON_MEMBER_Color "Color"

ot::StyleValue::StyleValue() {}

ot::StyleValue::StyleValue(const StyleValue& _other) 
	: m_key(_other.m_key), m_qss(_other.m_qss), m_color(_other.m_color)
{}

ot::StyleValue::~StyleValue() {}

ot::StyleValue& ot::StyleValue::operator = (const StyleValue& _other) {
	m_key = _other.m_key;
	m_qss = _other.m_qss;
	m_color = _other.m_color;
	return *this;
}

void ot::StyleValue::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Key, JsonString(m_key, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Qss, JsonString(m_qss, _allocator), _allocator);

	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Color, colorObj, _allocator);
}

void ot::StyleValue::setFromJsonObject(const ConstJsonObject& _object) {
	m_key = json::getString(_object, OT_JSON_MEMBER_Key);
	m_qss = json::getString(_object, OT_JSON_MEMBER_Qss);
	
	m_color.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Color));
}