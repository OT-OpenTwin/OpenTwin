// @otlicense
// File: EntityBlockHierarchicalContainerItem.h
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

#include "EntityBlock.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalContainerItem : public EntityBlock {
public:
	EntityBlockHierarchicalContainerItem() : EntityBlockHierarchicalContainerItem(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockHierarchicalContainerItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string className() { return "EntityBlockHierarchicalContainerItem"; }
	virtual std::string getClassName(void) const override { return EntityBlockHierarchicalContainerItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;

};

