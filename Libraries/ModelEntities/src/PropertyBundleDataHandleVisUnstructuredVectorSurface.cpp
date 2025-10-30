// @otlicense
// File: PropertyBundleDataHandleVisUnstructuredVectorSurface.cpp
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
#include "..\include\PropertyBundleDataHandleVisUnstructuredVectorSurface.h"

PropertyBundleDataHandleVisUnstructuredVectorSurface::PropertyBundleDataHandleVisUnstructuredVectorSurface(EntityBase * thisObject)
{
	LoadCurrentData(thisObject);
}

void PropertyBundleDataHandleVisUnstructuredVectorSurface::LoadCurrentData(EntityBase * thisObject)
{
	auto maxArrowsEnt = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNameMaxArrows()));
	assert(maxArrowsEnt != nullptr);
	maxArrows = maxArrowsEnt->getValue();

	auto arrowTypeEnt = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameArrowType()));
	assert(arrowTypeEnt != nullptr);
	selectedArrowType = properties.GetArrowType(arrowTypeEnt->getValue());

	auto arrowScaleEnt = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetNameArrowScale()));
	assert(arrowScaleEnt != nullptr);
	arrowScale = arrowScaleEnt->getValue();
}
