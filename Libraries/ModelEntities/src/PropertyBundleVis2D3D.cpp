// @otlicense
// File: PropertyBundleVis2D3D.cpp
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

#include "PropertyBundleVis2D3D.h"

void PropertyBundleVis2D3D::setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupName, m_properties.GetNameVisType(),
		{
			m_properties.GetValueArrows(),
			m_properties.GetValueContour()
		}, m_properties.GetValueArrows(), m_defaultCategory, _thisObject->getProperties());
	
	EntityPropertiesSelection::createProperty(m_groupName, m_properties.GetNameVisComponent(),
		{
			m_properties.GetValueComponentX(),
			m_properties.GetValueComponentY(),
			m_properties.GetValueComponentZ(),
			m_properties.GetValueAbsolute()
		}, m_properties.GetValueAbsolute(), m_defaultCategory, _thisObject->getProperties());

	//Not used yet
	/*EntityPropertiesDouble::createProperty(groupName, properties.GetNamePhase(), 0.0, defaultCategory, thisObject->getProperties());
	EntityPropertiesDouble::createProperty(groupName, properties.GetNameTime(), 0.0, defaultCategory, thisObject->getProperties());*/
	EntityPropertiesInteger::createProperty(m_groupName, m_properties.GetNamePlotDownSampling(), 0, m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleVis2D3D::updatePropertyVisibility(EntityBase * _thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	EntityPropertiesSelection* visComp = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisComponent()));
	EntityPropertiesInteger* downSampling = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNamePlotDownSampling()));

	assert(visType != nullptr);
	assert(visComp != nullptr);
	assert(downSampling != nullptr);

	PropertiesVis2D3D::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVis2D3D::VisualizationType::UNKNOWN);

	if (type == PropertiesVis2D3D::VisualizationType::Arrows)
	{
		bool changes = false;
		if (visComp->getVisible())
		{
			visComp->setVisible(false);
			changes = true;
		}
		if (!downSampling->getVisible())
		{
			downSampling->setVisible(true);
			changes |= true;
		}
		return changes;
	}
	else
	{
		bool changes = false;
		if (!visComp->getVisible())
		{
			visComp->setVisible(true);
			changes = true;
		}
		if (downSampling->getVisible())
		{
			downSampling->setVisible(false);
			changes |= true;
		}
		return changes;
	}

	return false;
}
