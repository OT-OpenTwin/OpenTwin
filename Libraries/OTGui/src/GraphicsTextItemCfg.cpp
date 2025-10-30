// @otlicense
// File: GraphicsTextItemCfg.cpp
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
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(ot::GraphicsTextItemCfg::className());

ot::GraphicsTextItemCfg::GraphicsTextItemCfg(const std::string& _text, const ot::Color& _textColor)
	: m_text(_text), m_textIsReference(false)
{
	m_textStyle.setColor(_textColor);
}

ot::GraphicsTextItemCfg::GraphicsTextItemCfg(const GraphicsTextItemCfg& _other) 
	: ot::GraphicsItemCfg(_other), m_text(_other.m_text), m_textFont(_other.m_textFont), m_textStyle(_other.m_textStyle), 
	m_textIsReference(_other.m_textIsReference)
{

}

ot::GraphicsTextItemCfg::~GraphicsTextItemCfg() {}

void ot::GraphicsTextItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("IsReference", m_textIsReference, _allocator);
	
	JsonObject fontObj;
	m_textFont.addToJsonObject(fontObj, _allocator);
	_object.AddMember("Font", fontObj, _allocator);

	JsonObject styleObj;
	m_textStyle.addToJsonObject(styleObj, _allocator);
	_object.AddMember("Style", styleObj, _allocator);
}

void ot::GraphicsTextItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_text = json::getString(_object, "Text");
	m_textFont.setFromJsonObject(json::getObject(_object, "Font"));
	m_textStyle.setFromJsonObject(json::getObject(_object, "Style"));
	m_textIsReference = json::getBool(_object, "IsReference");
}
