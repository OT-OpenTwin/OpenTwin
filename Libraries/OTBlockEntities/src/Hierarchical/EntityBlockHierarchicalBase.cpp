// @otlicense
// File: EntityBlockHierarchicalBase.cpp
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
#include "OTBlockEntities/Hierarchical/EntityBlockHierarchicalBase.h"

ot::EntityBlockHierarchicalBase::EntityBlockHierarchicalBase(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_ID, _parent, _obs, _ms)
{}

bool ot::EntityBlockHierarchicalBase::updateFromProperties() {
	bool needsUpdate = EntityBlock::updateFromProperties();
	return needsUpdate;
}

void ot::EntityBlockHierarchicalBase::createProperties()
{
	EntityBlock::createProperties();
}

void ot::EntityBlockHierarchicalBase::fillContextMenu(const MenuRequestData* _requestData, MenuCfg& _menu)
{
	_menu.addButton("Open", "Open", "ContextMenu/Open.png", MenuButtonCfg::ButtonAction::TriggerButton)
		->setTriggerButton("Project/Selection/Open");

	EntityBlock::fillContextMenu(_requestData, _menu);
}

void ot::EntityBlockHierarchicalBase::createNavigationTreeEntry()
{
	EntityBlock::createNavigationTreeEntry();
}

void ot::EntityBlockHierarchicalBase::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBlock::addStorageData(_storage);
}

void ot::EntityBlockHierarchicalBase::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);
}
