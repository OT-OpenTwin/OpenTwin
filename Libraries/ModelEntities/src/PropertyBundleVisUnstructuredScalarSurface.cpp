// @otlicense
// File: PropertyBundleVisUnstructuredScalarSurface.cpp
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

#include "PropertyBundleVisUnstructuredScalarSurface.h"

void PropertyBundleVisUnstructuredScalarSurface:: setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupNameGeneral, m_properties.GetNameVisType(),
		{
			m_properties.GetValuePoints(),
			m_properties.GetValueContour2D()
		}, m_properties.GetValueContour2D(), m_defaultCategory, _thisObject->getProperties());
	
	EntityPropertiesBoolean::createProperty(m_groupNameGeneral, m_properties.GetNameShow2DMesh(), false, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesColor::createProperty(m_groupNameGeneral, m_properties.GetName2DMeshColor(), { 255, 255, 255 }, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(m_groupNameContour, m_properties.GetNameShow2DIsolines(), true, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesColor::createProperty(m_groupNameContour, m_properties.GetName2DIsolineColor(), { 0, 0, 0 }, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupNamePoints, m_properties.GetNamePointScale(), 1., m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesInteger::createProperty(m_groupNamePoints, m_properties.GetNameMaxPoints(), 1000, m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredScalarSurface::updatePropertyVisibility(EntityBase * _thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	EntityPropertiesDouble* pointScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetNamePointScale()));
	EntityPropertiesBoolean* show2dMesh = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DMesh()));
	EntityPropertiesColor* color2dMesh = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DMeshColor()));
	EntityPropertiesBoolean* show2dIsolines = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DIsolines()));
	EntityPropertiesColor* color2dIsolines = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DIsolineColor()));
	EntityPropertiesInteger* maxPoints = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNameMaxPoints()));

	assert(visType != nullptr);

	PropertiesVisUnstructuredScalarSurface::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisUnstructuredScalarSurface::VisualizationType::UNKNOWN);

	if (type == PropertiesVisUnstructuredScalarSurface::VisualizationType::Points)
	{
		bool changes = false;
		if (!maxPoints->getVisible())
		{
			maxPoints->setVisible(true);
			changes |= true;
		}
		if (!pointScale->getVisible())
		{
			pointScale->setVisible(true);
			changes |= true;
		}
		if (show2dMesh->getVisible())
		{
			show2dMesh->setVisible(false);
			changes |= true;
		}
		if (color2dMesh->getVisible())
		{
			color2dMesh->setVisible(false);
			changes |= true;
		}
		if (show2dIsolines->getVisible())
		{
			show2dIsolines->setVisible(false);
			changes |= true;
		}
		if (color2dIsolines->getVisible())
		{
			color2dIsolines->setVisible(false);
			changes |= true;
		}
		return changes;
	}
	else if(type == PropertiesVisUnstructuredScalarSurface::VisualizationType::Contour2D)
	{
		bool changes = false;
		if (maxPoints->getVisible())
		{
			maxPoints->setVisible(false);
			changes |= true;
		}
		if (pointScale->getVisible())
		{
			pointScale->setVisible(false);
			changes |= true;
		}
		if (!show2dMesh->getVisible())
		{
			show2dMesh->setVisible(true);
			changes |= true;
		}
		if (!color2dMesh->getVisible())
		{
			color2dMesh->setVisible(true);
			changes |= true;
		}
		if (!show2dIsolines->getVisible())
		{
			show2dIsolines->setVisible(true);
			changes |= true;
		}
		if (!color2dIsolines->getVisible())
		{
			color2dIsolines->setVisible(true);
			changes |= true;
		}
		return changes;
	}
	else
	{
		assert(0); // Unknown type
	}

	return false;
}

