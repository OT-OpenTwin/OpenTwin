// @otlicense
// File: GraphicsDecoratedLineItemCfg.cpp
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
#include "OTGui/Graphics/GraphicsItemCfgFactory.h"
#include "OTGui/Graphics/GraphicsDecoratedLineItemCfg.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsDecoratedLineItemCfg> registrar(ot::GraphicsDecoratedLineItemCfg::className());

ot::GraphicsDecoratedLineItemCfg::GraphicsDecoratedLineItemCfg() {

}

ot::GraphicsDecoratedLineItemCfg::GraphicsDecoratedLineItemCfg(const GraphicsDecoratedLineItemCfg& _other)
	: GraphicsLineItemCfg(_other), m_fromDecoration(_other.m_fromDecoration), m_toDecoration(_other.m_toDecoration)
{}

ot::GraphicsDecoratedLineItemCfg::~GraphicsDecoratedLineItemCfg() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::GraphicsDecoratedLineItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsLineItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("FromDecoration", JsonObject(m_fromDecoration, _allocator), _allocator);
	_object.AddMember("ToDecoration", JsonObject(m_toDecoration, _allocator), _allocator);
}

void ot::GraphicsDecoratedLineItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsLineItemCfg::setFromJsonObject(_object);

	m_fromDecoration.setFromJsonObject(json::getObject(_object, "FromDecoration"));
	m_toDecoration.setFromJsonObject(json::getObject(_object, "ToDecoration"));
}
