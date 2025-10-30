// @otlicense
// File: Size2D.cpp
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
#include "OTCore/Size2D.h"

#define OT_JSON_MEMBER_Width "w"
#define OT_JSON_MEMBER_Height "h"

void ot::Size2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Width, m_w, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Height, m_h, _allocator);
}

void ot::Size2D::setFromJsonObject(const ConstJsonObject& _object) {
	m_w = json::getInt(_object, OT_JSON_MEMBER_Width);
	m_h = json::getInt(_object, OT_JSON_MEMBER_Height);
}

void ot::Size2DF::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Width, m_w, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Height, m_h, _allocator);
}

void ot::Size2DF::setFromJsonObject(const ConstJsonObject& _object) {
	m_w = json::getFloat(_object, OT_JSON_MEMBER_Width);
	m_h = json::getFloat(_object, OT_JSON_MEMBER_Height);
}

void ot::Size2DD::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Width, m_w, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Height, m_h, _allocator);
}

void ot::Size2DD::setFromJsonObject(const ConstJsonObject& _object) {
	m_w = json::getDouble(_object, OT_JSON_MEMBER_Width);
	m_h = json::getDouble(_object, OT_JSON_MEMBER_Height);
}