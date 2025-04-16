#include "PropertyBundleVisUnstructuredScalar.h"

void PropertyBundleVisUnstructuredScalar:: setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupNameGeneral, m_properties.GetNameVisType(),
		{
			m_properties.GetValueIsosurface(),
			m_properties.GetValuePoints(),
			m_properties.GetValueContour2D()
		}, m_properties.GetValueContour2D(), m_defaultCategory, _thisObject->getProperties());
	
	EntityPropertiesInteger::createProperty(m_groupNameIsosurfaces, m_properties.GetNameNumberIsosurfaces(), 1, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(m_groupNameGeneral, m_properties.GetNameShow2DMesh(), false, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesColor::createProperty(m_groupNameGeneral, m_properties.GetName2DMeshColor(), { 255, 255, 255 }, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesBoolean::createProperty(m_groupNameContour, m_properties.GetNameShow2DIsolines(), true, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesColor::createProperty(m_groupNameContour, m_properties.GetName2DIsolineColor(), { 0, 0, 0 }, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupNamePoints, m_properties.GetNamePointScale(), 1., m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesInteger::createProperty(m_groupNamePoints, m_properties.GetNameMaxPoints(), 1000, m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredScalar::updatePropertyVisibility(EntityBase * _thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	EntityPropertiesDouble* pointScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetNamePointScale()));
	EntityPropertiesInteger* numberIsosurfaces = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNameNumberIsosurfaces()));
	EntityPropertiesBoolean* show2dMesh = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DMesh()));
	EntityPropertiesColor* color2dMesh = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DMeshColor()));
	EntityPropertiesBoolean* show2dIsolines = dynamic_cast<EntityPropertiesBoolean*>(_thisObject->getProperties().getProperty(m_properties.GetNameShow2DIsolines()));
	EntityPropertiesColor* color2dIsolines = dynamic_cast<EntityPropertiesColor*>(_thisObject->getProperties().getProperty(m_properties.GetName2DIsolineColor()));
	EntityPropertiesInteger* maxPoints = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNameMaxPoints()));

	assert(visType != nullptr);

	PropertiesVisUnstructuredScalar::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVisUnstructuredScalar::VisualizationType::UNKNOWN);

	if (type == PropertiesVisUnstructuredScalar::VisualizationType::Isosurface)
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

bool PropertyBundleVisUnstructuredScalar::is2dType(EntityBase* _thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameVisType()));
	assert(visType != nullptr);

	PropertiesVisUnstructuredScalar::VisualizationType type = m_properties.GetVisualizationType(visType->getValue());
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

