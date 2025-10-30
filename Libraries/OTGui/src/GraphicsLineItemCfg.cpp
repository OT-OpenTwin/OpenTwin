// @otlicense
// File: GraphicsLineItemCfg.cpp
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
#include "OTGui/GraphicsLineItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsLineItemCfg> lineItemCfg(ot::GraphicsLineItemCfg::className());

ot::GraphicsLineItemCfg::GraphicsLineItemCfg() {}

ot::GraphicsLineItemCfg::GraphicsLineItemCfg(const GraphicsLineItemCfg& _other) 
	: ot::GraphicsItemCfg(_other), m_from(_other.m_from), m_to(_other.m_to), m_lineStyle(_other.m_lineStyle)
{}

ot::GraphicsLineItemCfg::~GraphicsLineItemCfg() {}

void ot::GraphicsLineItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject fromObj;
	m_from.addToJsonObject(fromObj, _allocator);
	_object.AddMember("From", fromObj, _allocator);

	JsonObject toObj;
	m_to.addToJsonObject(toObj, _allocator);
	_object.AddMember("To", toObj, _allocator);

	JsonObject lineObj;
	m_lineStyle.addToJsonObject(lineObj, _allocator);
	_object.AddMember("LineStyle", lineObj, _allocator);
}

void ot::GraphicsLineItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_from.setFromJsonObject(json::getObject(_object, "From"));
	m_to.setFromJsonObject(json::getObject(_object, "To"));
	m_lineStyle.setFromJsonObject(json::getObject(_object, "LineStyle"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter
