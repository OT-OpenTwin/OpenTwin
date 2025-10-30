// @otlicense
// File: SelectEntitiesDialogCfg.cpp
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
#include "OTGui/SelectEntitiesDialogCfg.h"

ot::SelectEntitiesDialogCfg::SelectEntitiesDialogCfg() {

}

ot::SelectEntitiesDialogCfg::~SelectEntitiesDialogCfg() {

}

void ot::SelectEntitiesDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	NavigationTreePackage::addToJsonObject(_object, _allocator);
}

void ot::SelectEntitiesDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_title = json::getString(_object, "Title");
	NavigationTreePackage::setFromJsonObject(_object);
}