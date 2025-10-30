// @otlicense
// File: EntityAPI.cpp
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
#include "DataBase.h"
#include "EntityAPI.h"
#include "EntityFactory.h"

EntityBase* ot::EntityAPI::readEntityFromEntityIDandVersion(UID _entityID, UID _version) {
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(_entityID, _version, doc)) {
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase* entity = EntityFactory::instance().create(entityType);

	if (entity != nullptr) {
		std::map<UID, EntityBase*> entityMap;
		entity->restoreFromDataBase(nullptr, nullptr, nullptr, doc_view, entityMap);
	}

	return entity;
}

