#include "PropertyBundleVisUnstructuredVector.h"

void PropertyBundleVisUnstructuredVector:: SetProperties(EntityBase * thisObject)
{
	EntityPropertiesSelection::createProperty(groupNameGeneral, properties.GetNameVisType(),
		{
			properties.GetValueArrows3D(),
			properties.GetValueArrows2D(),
			properties.GetValueContour2D()
		}, properties.GetValueArrows3D(), defaultCategory, thisObject->getProperties());
	
	EntityPropertiesSelection::createProperty(groupNameContour, properties.GetNameVisComponent(),
		{
			properties.GetValueComponentX(),
			properties.GetValueComponentY(),
			properties.GetValueComponentZ(),
			properties.GetValueAbsolute()
		}, properties.GetValueAbsolute(), defaultCategory, thisObject->getProperties());

	EntityPropertiesSelection::createProperty(groupNameArrows, properties.GetNameArrowType(),
		{
			properties.GetValueArrowTypeShaded(),
			properties.GetValueArrowTypeFlat(),
			properties.GetValueArrowTypeHedgehog(),
		}, properties.GetValueArrowTypeShaded(), defaultCategory, thisObject->getProperties());

	EntityPropertiesDouble::createProperty(groupNameArrows, properties.GetNameArrowScale(), 1., defaultCategory, thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(groupNameGeneral, properties.GetNameShow2DMesh(), false, defaultCategory, thisObject->getProperties());
	EntityPropertiesColor::createProperty(groupNameGeneral, properties.GetName2DMeshColor(), { 255, 255, 255 }, defaultCategory, thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(groupNameContour, properties.GetNameShow2DIsolines(), true, defaultCategory, thisObject->getProperties());
	EntityPropertiesColor::createProperty(groupNameContour, properties.GetName2DIsolineColor(), { 0, 0, 0 }, defaultCategory, thisObject->getProperties());

	EntityPropertiesInteger::createProperty(groupNameArrows, properties.GetNameMaxArrows(), 1000, defaultCategory, thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredVector::UpdatePropertyVisibility(EntityBase * thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	EntityPropertiesSelection* visComp = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisComponent()));
	EntityPropertiesSelection* arrowType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameArrowType()));
	EntityPropertiesDouble* arrowScale = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetNameArrowScale()));
	EntityPropertiesBoolean* show2dMesh = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DMesh()));
	EntityPropertiesColor* color2dMesh = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DMeshColor()));
	EntityPropertiesBoolean* show2dIsolines = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DIsolines()));
	EntityPropertiesColor* color2dIsolines = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DIsolineColor()));
	EntityPropertiesInteger* maxArrows = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNameMaxArrows()));

	assert(visType != nullptr);
	assert(visComp != nullptr);
	assert(maxArrows != nullptr);

	PropertiesVisUnstructuredVector::VisualizationType type = properties.GetVisualizationType(visType->getValue());
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

bool PropertyBundleVisUnstructuredVector::is2dType(EntityBase* thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	assert(visType != nullptr);

	PropertiesVisUnstructuredVector::VisualizationType type = properties.GetVisualizationType(visType->getValue());
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

