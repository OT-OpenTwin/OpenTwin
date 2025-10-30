// @otlicense
// File: GraphicsInvisibleItemCfg.cpp
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
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsInvisibleItemCfg.h"

#define OT_JSON_MEMBER_Size "Size"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsInvisibleItemCfg> rectItemCfg(ot::GraphicsInvisibleItemCfg::className());

ot::GraphicsInvisibleItemCfg::GraphicsInvisibleItemCfg(const ot::Size2DD& _size)
	: m_size(_size)
{}

ot::GraphicsInvisibleItemCfg::GraphicsInvisibleItemCfg(const GraphicsInvisibleItemCfg& _other) 
	: ot::GraphicsItemCfg(_other), m_size(_other.m_size)
{}

ot::GraphicsInvisibleItemCfg::~GraphicsInvisibleItemCfg() {}

void ot::GraphicsInvisibleItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject sizeObj;
	m_size.addToJsonObject(sizeObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Size, sizeObj, _allocator);
}

void ot::GraphicsInvisibleItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	m_size.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Size));
}
