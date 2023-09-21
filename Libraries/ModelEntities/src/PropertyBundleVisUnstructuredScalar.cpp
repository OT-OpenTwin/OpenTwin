#include "PropertyBundleVisUnstructuredScalar.h"

void PropertyBundleVisUnstructuredScalar:: SetProperties(EntityBase * thisObject)
{
	EntityPropertiesSelection::createProperty(groupNameGeneral, properties.GetNameVisType(),
		{
			properties.GetValueIsosurface(),
			properties.GetValuePoints(),
			properties.GetValueContour2D()
		}, properties.GetValueContour2D(), defaultCategory, thisObject->getProperties());
	
	EntityPropertiesInteger::createProperty(groupNameIsosurfaces, properties.GetNameNumberIsosurfaces(), 1, defaultCategory, thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(groupNameGeneral, properties.GetNameShow2DMesh(), false, defaultCategory, thisObject->getProperties());
	EntityPropertiesColor::createProperty(groupNameGeneral, properties.GetName2DMeshColor(), { 255, 255, 255 }, defaultCategory, thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(groupNameContour, properties.GetNameShow2DIsolines(), true, defaultCategory, thisObject->getProperties());
	EntityPropertiesColor::createProperty(groupNameContour, properties.GetName2DIsolineColor(), { 0, 0, 0 }, defaultCategory, thisObject->getProperties());

	EntityPropertiesDouble::createProperty(groupNamePoints, properties.GetNamePointScale(), 1., defaultCategory, thisObject->getProperties());
	EntityPropertiesInteger::createProperty(groupNamePoints, properties.GetNamePlotDownSampling(), 0., defaultCategory, thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredScalar::UpdatePropertyVisibility(EntityBase * thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	EntityPropertiesDouble* pointScale = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetNamePointScale()));
	EntityPropertiesInteger* numberIsosurfaces = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNameNumberIsosurfaces()));
	EntityPropertiesBoolean* show2dMesh = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DMesh()));
	EntityPropertiesColor* color2dMesh = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DMeshColor()));
	EntityPropertiesBoolean* show2dIsolines = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DIsolines()));
	EntityPropertiesColor* color2dIsolines = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DIsolineColor()));
	EntityPropertiesInteger* downSampling = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNamePlotDownSampling()));

	assert(visType != nullptr);

	PropertiesVisUnstructuredScalar::VisualizationType type = properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisUnstructuredScalar::VisualizationType::UNKNOWN);

	if (type == PropertiesVisUnstructuredScalar::VisualizationType::Isosurface)
	{
		bool changes = false;
		if (downSampling->getVisible())
		{
			downSampling->setVisible(false);
			changes |= true;
		}
		if (pointScale->getVisible())
		{
			pointScale->setVisible(false);
			changes |= true;
		}
		if (!numberIsosurfaces->getVisible())
		{
			numberIsosurfaces->setVisible(true);
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
	else if (type == PropertiesVisUnstructuredScalar::VisualizationType::Points)
	{
		bool changes = false;
		if (!downSampling->getVisible())
		{
			downSampling->setVisible(true);
			changes |= true;
		}
		if (!pointScale->getVisible())
		{
			pointScale->setVisible(true);
			changes |= true;
		}
		if (numberIsosurfaces->getVisible())
		{
			numberIsosurfaces->setVisible(false);
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
	else if(type == PropertiesVisUnstructuredScalar::VisualizationType::Contour2D)
	{
		bool changes = false;
		if (downSampling->getVisible())
		{
			downSampling->setVisible(false);
			changes |= true;
		}
		if (pointScale->getVisible())
		{
			pointScale->setVisible(false);
			changes |= true;
		}
		if (numberIsosurfaces->getVisible())
		{
			numberIsosurfaces->setVisible(false);
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

bool PropertyBundleVisUnstructuredScalar::is2dType(EntityBase* thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	assert(visType != nullptr);

	PropertiesVisUnstructuredScalar::VisualizationType type = properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisUnstructuredScalar::VisualizationType::UNKNOWN);

	if (type == PropertiesVisUnstructuredScalar::VisualizationType::Isosurface)
	{
		return false;
	}
	else if (type == PropertiesVisUnstructuredScalar::VisualizationType::Points)
	{
		return false;
	}
	else if (type == PropertiesVisUnstructuredScalar::VisualizationType::Contour2D)
	{
		return true;
	}
	else
	{
		assert(0); // Unknown type
	}

	return false;
}

