// @otlicense
// File: GraphicsItemFileCfg.cpp
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
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsItemFileCfg> fileItemCfg(ot::GraphicsItemFileCfg::className());

ot::GraphicsItemFileCfg::GraphicsItemFileCfg()
{}

ot::GraphicsItemFileCfg::GraphicsItemFileCfg(const std::string& _subPath)
	: m_file(_subPath)
{}

ot::GraphicsItemFileCfg::GraphicsItemFileCfg(const GraphicsItemFileCfg & _other)
	: ot::GraphicsItemCfg(_other), m_file(_other.m_file)
{}

ot::GraphicsItemFileCfg::~GraphicsItemFileCfg() {
	
}

void ot::GraphicsItemFileCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);
	_object.AddMember("File", JsonString(m_file, _allocator), _allocator);
}

void ot::GraphicsItemFileCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	m_file = json::getString(_object, "File");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter
