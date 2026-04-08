// @otlicense
// File: PropertyBundleVisCartesianVectorVolume.cpp
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

#include "OTModelEntities/PropertyBundleVisCartesianVectorVolume.h"

void PropertyBundleVisCartesianVectorVolume:: setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupNameGeneral, m_properties.GetNameVisType(),
		{
			m_properties.GetValueArrows3D(),
			m_properties.GetValueArrows2D(),
			m_properties.GetValueContour2D()
		}, m_properties.GetValueArrows3D(), m_defaultCategory, _thisObject->getProperties());
	
	EntityPropertiesSelection::createProperty(m_groupNameContour, m_properties.GetNameVisComponent(),
		{
			m_properties.GetValueComponentX(),
			m_properties.GetValueComponentY(),
			m_properties.GetValueComponentZ(),
			m_properties.GetValueAbsolute()
		}, m_properties.GetValueAbsolute(), m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesSelection::createProperty(m_groupNameArrows, m_properties.GetNameArrowType(),
		{
			m_properties.GetValueArrowTypeShaded(),
			m_properties.GetValueArrowTypeFlat(),
			m_properties.GetValueArrowTypeHedgehog(),
		}, m_properties.GetValueArrowTypeShaded(), m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupNameArrows, m_properties.GetNameArrowScale(), 1., m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(m_groupNameGeneral, m_properties.GetNameShow2DMesh(), false, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesColor::createProperty(m_groupNameGeneral, m_properties.GetName2DMeshColor(), { 255, 255, 255 }, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(m_groupNameContour, m_properties.GetNameShow2DIsolines(), true, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesColor::createProperty(m_groupNameContour, m_properties.GetName2DIsolineColor(), { 0, 0, 0 }, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesInteger::createProperty(m_groupNameArrows, m_properties.GetNameMaxArrows(), 1000, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesSelection::createProperty(m_groupNameComplex, m_properties.GetNameVisQuantity(),
		{
			m_properties.GetValueQuantityReal(),
			m_properties.GetValueQuantityImag(),
			m_properties.GetValueQuantityMag(),
			m_properties.GetValueQuantityPhase(),
			m_properties.GetValueQuantityPhaseProjected()
		}, m_properties.GetValueQuantityPhaseProjected(), m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupNameComplex, m_properties.GetNamePhase(), 0.0, m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleVisCartesianVectorVolume::updatePropertyVisibility(EntityBase * _thisObject)
{
	bool changes = false;

	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	EntityPropertiesSelection* visComp = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisComponent()));
	EntityPropertiesSelection* visQuantity = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisQuantity()));
	EntityPropertiesSelection* arrowType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowType()));
	EntityPropertiesDouble* arrowScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowScale()));
	EntityPropertiesBoolean* show2dMesh = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DMesh()));
	EntityPropertiesColor* color2dMesh = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DMeshColor()));
	EntityPropertiesBoolean* show2dIsolines = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DIsolines()));
	EntityPropertiesColor* color2dIsolines = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DIsolineColor()));
	EntityPropertiesInteger* maxArrows = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNameMaxArrows()));
	EntityPropertiesDouble* phase = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetNamePhase()));

	assert(visType != nullptr);
	assert(visComp != nullptr);
	assert(maxArrows != nullptr);

	PropertiesVisCartesianVector::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisCartesianVector::VisualizationType::UNKNOWN);

	if (type == PropertiesVisCartesianVector::VisualizationType::Arrows3D)
	{
		if (visComp->getVisible())
		{
			visComp->setVisible(false);
			changes = true;
		}
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
	}
	else if (type == PropertiesVisCartesianVector::VisualizationType::Arrows2D)
	{
		if (visComp->getVisible())
		{
			visComp->setVisible(false);
			changes = true;
		}
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
	}
	else if(type == PropertiesVisCartesianVector::VisualizationType::Contour2D)
	{
		if (!visComp->getVisible())
		{
			visComp->setVisible(true);
			changes = true;
		}
		if (maxArrows->getVisible())
		{
			maxArrows->setVisible(false);
			changes |= true;
		}
		if (arrowType->getVisible())
		{
			arrowType->setVisible(false);
			changes |= true;
		}
		if (arrowScale->getVisible())
		{
			arrowScale->setVisible(false);
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
	}
	else
	{
		assert(0); // Unknown type
	}

	assert(visQuantity != nullptr);
	assert(phase != nullptr);

	PropertiesVisCartesianVector::VisualizationQuantity quantity = m_properties.GetVisualizationQuantity(visQuantity->getValue());
	assert(quantity != PropertiesVisCartesianVector::VisualizationQuantity::UNKNOWN);

	if (quantity == PropertiesVisCartesianVector::VisualizationQuantity::PHASE_PROJECTION)
	{
		if (!phase->getVisible())
		{
			phase->setVisible(true);
			changes |= true;
		}
	}
	else
	{
		if (phase->getVisible())
		{
			phase->setVisible(false);
			changes |= true;
		}
	}

	return changes;
}

bool PropertyBundleVisCartesianVectorVolume::is2dType(EntityBase* _thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	assert(visType != nullptr);

	PropertiesVisCartesianVector::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisCartesianVector::VisualizationType::UNKNOWN);

	if (type == PropertiesVisCartesianVector::VisualizationType::Arrows3D)
	{
		return false;
	}
	else if (type == PropertiesVisCartesianVector::VisualizationType::Arrows2D)
	{
		return true;
	}
	else if (type == PropertiesVisCartesianVector::VisualizationType::Contour2D)
	{
		return true;
	}
	else
	{
		assert(0); // Unknown type
	}

	return false;
}

