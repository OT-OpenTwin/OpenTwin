// @otlicense
// File: GraphicsArcItemCfg.cpp
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
#include "OTGui/GraphicsArcItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsArcItemCfg> arcItemCfg(ot::GraphicsArcItemCfg::className());

ot::GraphicsArcItemCfg::GraphicsArcItemCfg()
	: m_startAngle(0.), m_spanAngle(180. * 16.)
{}

ot::GraphicsArcItemCfg::GraphicsArcItemCfg(const GraphicsArcItemCfg & _other) 
	: GraphicsItemCfg(_other), m_rect(_other.m_rect), m_startAngle(_other.m_startAngle),
	m_spanAngle(_other.m_spanAngle), m_lineStyle(_other.m_lineStyle)
{}

ot::GraphicsArcItemCfg::~GraphicsArcItemCfg() {}

void ot::GraphicsArcItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject rectObj;
	m_rect.addToJsonObject(rectObj, _allocator);
	_object.AddMember("Rect", rectObj, _allocator);

	_object.AddMember("Start", m_startAngle, _allocator);
	_object.AddMember("Span", m_spanAngle, _allocator);

	JsonObject lineObj;
	m_lineStyle.addToJsonObject(lineObj, _allocator);
	_object.AddMember("LineStyle", lineObj, _allocator);
}

void ot::GraphicsArcItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_rect.setFromJsonObject(json::getObject(_object, "Rect"));
	m_startAngle = json::getDouble(_object, "Start");
	m_spanAngle = json::getDouble(_object, "Span");
	m_lineStyle.setFromJsonObject(json::getObject(_object, "LineStyle"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter
