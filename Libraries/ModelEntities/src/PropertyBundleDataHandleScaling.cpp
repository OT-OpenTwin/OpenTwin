// @otlicense
// File: PropertyBundleDataHandleScaling.cpp
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

#include "..\include\PropertyBundleDataHandleScaling.h"

PropertyBundleDataHandleScaling::PropertyBundleDataHandleScaling(EntityBase * thisObject) : thisObject(thisObject)
{
	LoadCurrentData(thisObject);
}

void PropertyBundleDataHandleScaling::LoadCurrentData(EntityBase * thisObject)
{

	auto scaleMethodProperty = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetPropertyNameScalingMethod()));
	assert(scaleMethodProperty != nullptr);
	scalingMethod = properties.GetScalingMethod(scaleMethodProperty->getValue());

	auto scaleFunctionProperty = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetPropertyNameScalingFunction()));
	assert(scaleFunctionProperty != nullptr);
	scalingFunction = properties.GetScalingFunction(scaleFunctionProperty->getValue());

	if (scalingMethod == ScalingProperties::ScalingMethod::rangeScale)
	{
		auto minProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameRangeMin()));
		assert(minProperty != nullptr);
		rangeMin = minProperty->getValue();
		auto maxProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameRangeMax()));
		assert(maxProperty != nullptr);
		rangeMax = maxProperty->getValue();
	}
	auto colourResolutionEnt = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetPropertyNameColourResolution()));
	colourResolution = colourResolutionEnt->getValue();

}

bool PropertyBundleDataHandleScaling::UpdateMinMaxProperties(double minValue, double maxValue)
{
	auto minScale = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameRangeMin()));
	auto maxScale = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameRangeMax()));
	double oldMinValue = minScale->getValue();
	double oldMaxValue = maxScale->getValue();

	if (minValue != oldMinValue || maxValue != oldMaxValue)
	{
		minScale->setValue(minValue);
		maxScale->setValue(maxValue);
		thisObject->storeToDataBase();
		return true;
	}
	else
	{
		return false;
	}
	
}

void PropertyBundleDataHandleScaling::GetEntityProperties(long long & uID, long long & version)
{
	uID = thisObject->getEntityID();
	version = thisObject->getEntityStorageVersion();
}

