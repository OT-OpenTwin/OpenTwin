// @otlicense
// File: EntityBlockHierarchicalBase.h
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

#pragma once

// OpenTwin header
#include "OTCore/ProjectInformation.h"
#include "OTBlockEntities/EntityBlock.h"

class EntityBinaryData;

namespace ot {

	class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalBase : public EntityBlock
	{
	public:
		EntityBlockHierarchicalBase() : EntityBlockHierarchicalBase(0, nullptr, nullptr, nullptr) {};
		EntityBlockHierarchicalBase(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

		virtual entityType getEntityType(void) const override { return TOPOLOGY; }

		virtual bool updateFromProperties() override;
		virtual void createProperties() override;

		//! @brief Fills the context menu config for this entity.
		//! Adds:
		//!    -> Open
		//!    -> EntityBlock::fillContextMenu()
		//! @param _requestData Data related to the context menu request. Caller keeps ownership of the pointer.
		//! @param _menuCfg Context menu config to fill.
		virtual void fillContextMenu(const MenuRequestData* _requestData, MenuCfg& _menu) override;

	protected:
		virtual void createNavigationTreeEntry() override;
		virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
		virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

	};

}
