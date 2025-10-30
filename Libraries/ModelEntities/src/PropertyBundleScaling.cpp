// @otlicense
// File: PropertyBundleScaling.cpp
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

#include "PropertyBundleScaling.h"

void PropertyBundleScaling::setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupName, m_properties.GetPropertyNameScalingMethod(), 
		{ 
			m_properties.GetPropertyValueAutoscaling(), 
			m_properties.GetPropertyValueRangeScaling() 
		}, m_properties.GetPropertyValueAutoscaling(), m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesSelection::createProperty(m_groupName, m_properties.GetPropertyNameScalingFunction(),
		{
			m_properties.GetPropertyValueLinScaling(),
			m_properties.GetPropertyValueLogscaling()
		}, m_properties.GetPropertyValueLinScaling(), m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupName, m_properties.GetPropertyNameRangeMin(), 0.0, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesDouble::createProperty(m_groupName, m_properties.GetPropertyNameRangeMax(), 0.0, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesInteger::createProperty(m_groupName, m_properties.GetPropertyNameColourResolution(), 30., m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleScaling::updatePropertyVisibility(EntityBase * _thisObject)
{
	auto scalingMethod = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameScalingMethod()));

	auto minScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameRangeMin()));
	auto maxScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameRangeMax()));

	bool updatePropertiesGrid = false;
	if (scalingMethod->getValue() == m_properties.GetPropertyValueRangeScaling())
	{
		if (!minScale->getVisible())
		{
			minScale->setVisible(true);
			updatePropertiesGrid = true;
		};
		if (!maxScale->getVisible())
		{
			maxScale->setVisible(true);
			updatePropertiesGrid = true;
		}
	}
	else
	{
		if (minScale->getVisible())
		{
			minScale->setVisible(false);
			updatePropertiesGrid = true;
		};
		if (maxScale->getVisible())
		{
			maxScale->setVisible(false);
			updatePropertiesGrid = true;
		}
	}
	return updatePropertiesGrid;
}

