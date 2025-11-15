// @otlicense
// File: EntityMetadataCampaign.cpp
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

#include "EntityMetadataCampaign.h"

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityMetadataCampaign> registrar("EntityMetadataCampaign");

EntityMetadataCampaign::EntityMetadataCampaign(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms)
	: EntityWithDynamicFields(ID, parent, mdl, ms)
{
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("RMD");
	treeItem.setHiddenIcon("RMD");
	this->setTreeItem(treeItem, true);
}

bool EntityMetadataCampaign::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}
