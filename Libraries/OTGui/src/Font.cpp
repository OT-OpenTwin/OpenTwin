// @otlicense
// File: Font.cpp
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
#include "OTGui/Font.h"

ot::Font::Font() : m_sizePx(12), m_isBold(false), m_isItalic(false), m_family("Consolas") {}

ot::Font::Font(FontFamily _fontFamily, int _sizePx, bool _isBold, bool _isItalic) : m_sizePx(_sizePx), m_isBold(_isBold), m_isItalic(_isItalic) {
	m_family = toString(_fontFamily);
}

ot::Font::Font(const std::string& _fontFamily, int _sizePx, bool _isBold, bool _isItalic) : m_family(_fontFamily), m_sizePx(_sizePx), m_isBold(_isBold), m_isItalic(_isItalic) {}

ot::Font::~Font() {}

bool ot::Font::operator == (const Font& _other) const {
	return m_family == _other.m_family && m_sizePx == _other.m_sizePx && m_isBold == _other.m_isBold && m_isItalic == _other.m_isItalic;
}

bool ot::Font::operator != (const Font& _other) const {
	return !(*this == _other);
}

void ot::Font::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Family", JsonString(m_family, _allocator), _allocator);
	_object.AddMember("SizePx", m_sizePx, _allocator);
	_object.AddMember("Bold", m_isBold, _allocator);
	_object.AddMember("Italic", m_isItalic, _allocator);
}

void ot::Font::setFromJsonObject(const ConstJsonObject& _object) {
	m_family = json::getString(_object, "Family");
	m_sizePx = json::getInt(_object, "SizePx");
	m_isBold = json::getBool(_object, "Bold");
	m_isItalic = json::getBool(_object, "Italic");
}

void ot::Font::setFamily(FontFamily _fontFamily) {
	m_family = toString(_fontFamily);
}