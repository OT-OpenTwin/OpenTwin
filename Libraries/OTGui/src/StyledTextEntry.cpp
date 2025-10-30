// @otlicense
// File: StyledTextEntry.cpp
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
#include "OTGui/StyledTextEntry.h"

ot::StyledTextEntry::StyledTextEntry(const StyledTextStyle& _style)
	: m_style(_style) 
{

}

ot::StyledTextEntry::StyledTextEntry(const ot::ConstJsonObject& _jsonObject) {
	this->setFromJsonObject(_jsonObject);
}

void ot::StyledTextEntry::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject styleObj;
	m_style.addToJsonObject(styleObj, _allocator);
	_object.AddMember("Style", styleObj, _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
}

void ot::StyledTextEntry::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_style.setFromJsonObject(json::getObject(_object, "Style"));
	m_text = json::getString(_object, "Text");
}
