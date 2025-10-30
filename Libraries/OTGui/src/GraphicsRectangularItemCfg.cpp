// @otlicense
// File: GraphicsRectangularItemCfg.cpp
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
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsRectangularItemCfg.h"

#define OT_JSON_MEMBER_Size "Size"
#define OT_JSON_MEMBER_Outline "Outline"
#define OT_JSON_MEMBER_CornerRadius "CornerRadius"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg(ot::GraphicsRectangularItemCfg::className());

ot::GraphicsRectangularItemCfg::GraphicsRectangularItemCfg(ot::Painter2D* _backgroundPainter, int _cornerRadius)
	: m_backgroundPainter(_backgroundPainter), m_cornerRadius(_cornerRadius)
{
	if (m_backgroundPainter == nullptr) {
		// If no background painter was provided, we set a transparent background as default
		m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
	}
}

ot::GraphicsRectangularItemCfg::GraphicsRectangularItemCfg(const GraphicsRectangularItemCfg& _other)
	: ot::GraphicsItemCfg(_other), m_cornerRadius(_other.m_cornerRadius), m_outline(_other.m_outline), m_size(_other.m_size)
{
	if (_other.m_backgroundPainter) {
		m_backgroundPainter = _other.m_backgroundPainter->createCopy();
	}
	else {
		m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
	}
}

ot::GraphicsRectangularItemCfg::~GraphicsRectangularItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsRectangularItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_MEMBER_CornerRadius, m_cornerRadius, _allocator);

	JsonObject sizeObj;
	m_size.addToJsonObject(sizeObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Size, sizeObj, _allocator);

	JsonObject backgroundPainterObj;
	m_backgroundPainter->addToJsonObject(backgroundPainterObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj, _allocator);

	JsonObject outlineObj;
	m_outline.addToJsonObject(outlineObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Outline, outlineObj, _allocator);
}

void ot::GraphicsRectangularItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_cornerRadius = json::getInt(_object, OT_JSON_MEMBER_CornerRadius);

	ConstJsonObject backgroundPainterObj = json::getObject(_object, OT_JSON_MEMBER_BackgroundPainter);
	ot::Painter2D* p = Painter2DFactory::create(backgroundPainterObj);
	if (p) {
		this->setBackgroundPainer(p);
	}

	m_outline.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Outline));
	m_size.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Size));
}

void ot::GraphicsRectangularItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
