// @otlicense
// File: PropertyDialogCfg.cpp
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
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyDialogCfg.h"

ot::PropertyDialogCfg::PropertyDialogCfg() {
	this->setMinSize(800, 600);
}

ot::PropertyDialogCfg::~PropertyDialogCfg() {
	
}

void ot::PropertyDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	DialogCfg::addToJsonObject(_object, _allocator);

	JsonObject cfgObj;
	m_gridConfig.addToJsonObject(cfgObj, _allocator);
	_object.AddMember("Config", cfgObj, _allocator);
}

void ot::PropertyDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	DialogCfg::setFromJsonObject(_object);

	ConstJsonObject cfgObj = json::getObject(_object, "Config");
	m_gridConfig.setFromJsonObject(cfgObj);
}
