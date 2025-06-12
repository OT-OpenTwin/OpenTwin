#include "PropertyBundleVisUnstructuredVectorVolume.h"

void PropertyBundleVisUnstructuredVectorVolume:: setProperties(EntityBase * _thisObject)
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
}

bool PropertyBundleVisUnstructuredVectorVolume::updatePropertyVisibility(EntityBase * _thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	EntityPropertiesSelection* visComp = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisComponent()));
	EntityPropertiesSelection* arrowType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowType()));
	EntityPropertiesDouble* arrowScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowScale()));
	EntityPropertiesBoolean* show2dMesh = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DMesh()));
	EntityPropertiesColor* color2dMesh = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DMeshColor()));
	EntityPropertiesBoolean* show2dIsolines = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DIsolines()));
	EntityPropertiesColor* color2dIsolines = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DIsolineColor()));
	EntityPropertiesInteger* maxArrows = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNameMaxArrows()));

	assert(visType != nullptr);
	assert(visComp != nullptr);
	assert(maxArrows != nullptr);

	PropertiesVisUnstructuredVector::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisUnstructuredVector::VisualizationType::UNKNOWN);

	if (type == PropertiesVisUnstructuredVector::VisualizationType::Arrows3D)
	{
		bool changes = false;
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
		return changes;
	}
	else if (type == PropertiesVisUnstructuredVector::VisualizationType::Arrows2D)
	{
		bool changes = false;
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
		return changes;
	}
	else if(type == PropertiesVisUnstructuredVector::VisualizationType::Contour2D)
	{
		bool changes = false;
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
		return changes;
	}
	else
	{
		assert(0); // Unknown type
	}

	return false;
}

bool PropertyBundleVisUnstructuredVectorVolume::is2dType(EntityBase* _thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	assert(visType != nullptr);

	PropertiesVisUnstructuredVector::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisUnstructuredVector::VisualizationType::UNKNOWN);

	if (type == PropertiesVisUnstructuredVector::VisualizationType::Arrows3D)
	{
		return false;
	}
	else if (type == PropertiesVisUnstructuredVector::VisualizationType::Arrows2D)
	{
		return true;
	}
	else if (type == PropertiesVisUnstructuredVector::VisualizationType::Contour2D)
	{
		return true;
	}
	else
	{
		assert(0); // Unknown type
	}

	return false;
}

