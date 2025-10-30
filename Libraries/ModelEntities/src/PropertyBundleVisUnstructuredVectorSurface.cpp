// @otlicense
// File: PropertyBundleVisUnstructuredVectorSurface.cpp
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

#include "PropertyBundleVisUnstructuredVectorSurface.h"

void PropertyBundleVisUnstructuredVectorSurface:: setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupNameArrows, m_properties.GetNameArrowType(),
		{
			m_properties.GetValueArrowTypeShaded(),
			m_properties.GetValueArrowTypeFlat(),
			m_properties.GetValueArrowTypeHedgehog(),
		}, m_properties.GetValueArrowTypeShaded(), m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupNameArrows, m_properties.GetNameArrowScale(), 1., m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesInteger::createProperty(m_groupNameArrows, m_properties.GetNameMaxArrows(), 1000, m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredVectorSurface::updatePropertyVisibility(EntityBase * _thisObject)
{
	EntityPropertiesSelection* arrowType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowType()));
	EntityPropertiesDouble* arrowScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowScale()));
	EntityPropertiesInteger* maxArrows = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNameMaxArrows()));

	assert(arrowType != nullptr);
	assert(arrowScale != nullptr);
	assert(maxArrows != nullptr);

	bool changes = false;

	if (!maxArrows->getVisible())
	{
		maxArrows->setVisible(true);
		changes |= true;
	}

	if (!arrowType->getVisible())
	{
		arrowType->setVisible(true);
		changes |= true;
	}

	if (!arrowScale->getVisible())
	{
		arrowScale->setVisible(true);
		changes |= true;
	}

	return changes;
}

