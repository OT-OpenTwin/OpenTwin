// @otlicense
// File: EntityResult3D.cpp
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

#include "../include/EntityResult3D.h"

static EntityFactoryRegistrar<EntityResult3D> registrar(EntityResult3D::className());

EntityResult3D::EntityResult3D(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, const std::string & owner)
	: EntityVis2D3D(ID, parent, obs, ms, owner) {}

void EntityResult3D::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundlePlane.setProperties(this);
	propertyBundleScaling.setProperties(this);
	propertyBundleVis2D3D.setProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityResult3D::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	updatePropertiesGrid = propertyBundlePlane.updatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleScaling.updatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleVis2D3D.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}
