#include "PropertyBundleVisUnstructuredScalarSurface.h"

void PropertyBundleVisUnstructuredScalarSurface:: SetProperties(EntityBase * thisObject)
{
	EntityPropertiesSelection::createProperty(groupNameGeneral, properties.GetNameVisType(),
		{
			properties.GetValuePoints(),
			properties.GetValueContour2D()
		}, properties.GetValueContour2D(), defaultCategory, thisObject->getProperties());
	
	EntityPropertiesBoolean::createProperty(groupNameGeneral, properties.GetNameShow2DMesh(), false, defaultCategory, thisObject->getProperties());
	EntityPropertiesColor::createProperty(groupNameGeneral, properties.GetName2DMeshColor(), { 255, 255, 255 }, defaultCategory, thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(groupNameContour, properties.GetNameShow2DIsolines(), true, defaultCategory, thisObject->getProperties());
	EntityPropertiesColor::createProperty(groupNameContour, properties.GetName2DIsolineColor(), { 0, 0, 0 }, defaultCategory, thisObject->getProperties());

	EntityPropertiesDouble::createProperty(groupNamePoints, properties.GetNamePointScale(), 1., defaultCategory, thisObject->getProperties());
	EntityPropertiesInteger::createProperty(groupNamePoints, properties.GetNameMaxPoints(), 1000, defaultCategory, thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredScalarSurface::UpdatePropertyVisibility(EntityBase * thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	EntityPropertiesDouble* pointScale = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetNamePointScale()));
	EntityPropertiesBoolean* show2dMesh = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DMesh()));
	EntityPropertiesColor* color2dMesh = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DMeshColor()));
	EntityPropertiesBoolean* show2dIsolines = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DIsolines()));
	EntityPropertiesColor* color2dIsolines = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DIsolineColor()));
	EntityPropertiesInteger* maxPoints = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNameMaxPoints()));

	assert(visType != nullptr);

	PropertiesVisUnstructuredScalarSurface::VisualizationType type = properties.GetVisualizationType(visType->getValue());
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

