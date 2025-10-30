// @otlicense
// File: PropertyBundleDataHandleVis2D3D.cpp
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

#include "PropertyBundleDataHandle.h"
#include "..\include\PropertyBundleDataHandleVis2D3D.h"

PropertyBundleDataHandleVis2D3D::PropertyBundleDataHandleVis2D3D(EntityBase * thisObject)
{
	LoadCurrentData(thisObject);
}

void PropertyBundleDataHandleVis2D3D::LoadCurrentData(EntityBase * thisObject)
{
	auto visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	assert(visType != nullptr);
	selectedVisType = properties.GetVisualizationType(visType->getValue());

	auto visComp = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisComponent()));
	assert(visComp != nullptr);
	selectedVisComp = properties.GetVisualizationComponent(visComp->getValue());

	auto downSamplingEnt = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNamePlotDownSampling()));
	assert(downSamplingEnt != nullptr);
	downsamplingRate = downSamplingEnt->getValue();
}
