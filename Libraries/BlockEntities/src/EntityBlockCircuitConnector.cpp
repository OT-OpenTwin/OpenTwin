// @otlicense
// File: EntityBlockCircuitConnector.cpp
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

#include "EntityBlockCircuitConnector.h"
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/BasicGraphicsIntersectionItem.h"
#include "OTCore/LogDispatcher.h"

static EntityFactoryRegistrar<EntityBlockCircuitConnector> registrar(EntityBlockCircuitConnector::className());

EntityBlockCircuitConnector::EntityBlockCircuitConnector(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBlock(ID, parent, obs, ms)
{
	OldTreeIcon icon;
	icon.visibleIcon = "Diod2";
	icon.hiddenIcon = "Diod2";
	setNavigationTreeIcon(icon);

	setBlockTitle("Connector");

	resetModified();
}

#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitConnector::createBlockCfg() {
#if TEST_ITEM_LOADER==true
	ot::BasicGraphicsIntersectionItem* newConfig = new ot::BasicGraphicsIntersectionItem;
	newConfig->setUid(this->getEntityID());
	newConfig->setName(this->getName());

	return newConfig;
#endif
}

void EntityBlockCircuitConnector::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitConnector::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}