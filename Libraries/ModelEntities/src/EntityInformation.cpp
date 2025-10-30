// @otlicense
// File: EntityInformation.cpp
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
#include "OTSystem/OTAssert.h"
#include "EntityBase.h"
#include "EntityInformation.h"

ot::EntityInformation::EntityInformation() {}

ot::EntityInformation::EntityInformation(EntityBase* _entity) {
	OTAssertNullptr(_entity);
	this->setEntityID(_entity->getEntityID());
	this->setEntityVersion(_entity->getEntityStorageVersion());
	this->setEntityName(_entity->getName());
	m_type = _entity->getClassName();
}

void ot::EntityInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	BasicEntityInformation::addToJsonObject(_object, _allocator);
	_object.AddMember("EntityType", JsonString(m_type, _allocator), _allocator);
}

void ot::EntityInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	BasicEntityInformation::setFromJsonObject(_object);
	m_type = json::getString(_object, "EntityType");
}

ot::EntityInformation::~EntityInformation() {}