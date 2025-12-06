// @otlicense
// File: EntityBlockHierarchicalContainerItem.cpp
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
#include "EntityFile.h"
#include "PropertyHelper.h"
#include "EntityProperties.h"
#include "EntityBlockHierarchicalContainerItem.h"

#include "OTGui/GraphicsHierarchicalProjectItemBuilder.h"

static EntityFactoryRegistrar<EntityBlockHierarchicalContainerItem> registrar(EntityBlockHierarchicalContainerItem::className());

EntityBlockHierarchicalContainerItem::EntityBlockHierarchicalContainerItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_ID, _parent, _obs, _ms) 
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Hierarchical/Container");
	treeItem.setHiddenIcon("Hierarchical/Container");
	this->setDefaultTreeItem(treeItem);
	
	setBlockTitle("Container");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockHierarchicalContainerItem::createBlockCfg() {
	ot::GraphicsHierarchicalProjectItemBuilder builder;

	// Mandatory settings
	builder.setName(this->getName());
	builder.setTitle(this->createBlockHeadline());
	builder.setLeftTitleCornerImagePath("Hierarchical/Container.png");
	builder.setTitleBackgroundGradientColor(ot::Green);
	builder.setPreviewImagePath("Hierarchical/ContainerBackground.png");

	// Create the item
	return builder.createGraphicsItem();
}
