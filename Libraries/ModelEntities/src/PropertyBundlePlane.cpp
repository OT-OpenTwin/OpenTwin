#include "..\include\PropertyBundlePlane.h"

void PropertyBundlePlane::SetProperties(EntityBase * thisObject)
{
	EntityPropertiesSelection::createProperty("Plane", properties.GetPropertyNameNormal(), 
		{ 
			properties.GetPropertyValueNormalX(), 
			properties.GetPropertyValueNormalY(), 
			properties.GetPropertyValueNormalZ(), 
			properties.GetPropertyValueNormalFree() 
		}, properties.GetPropertyValueNormalZ(), "2D Visualization",thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", properties.GetPropertyNameNormalCooX(), 0.0, "2D Visualization", thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", properties.GetPropertyNameNormalCooY(), 0.0, "2D Visualization", thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", properties.GetPropertyNameNormalCooZ(), 1.0, "2D Visualization", thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", properties.GetPropertyNameCenterX(), 0.0, "2D Visualization", thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", properties.GetPropertyNameCenterY(), 0.0, "2D Visualization", thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", properties.GetPropertyNameCenterZ(), 0.0, "2D Visualization", thisObject->getProperties());
}

bool PropertyBundlePlane::UpdatePropertyVisibility(EntityBase * thisObject)
{
	EntityPropertiesSelection *normalDirection = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormal()));

	EntityPropertiesDouble *normalX = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormalCooX()));
	EntityPropertiesDouble *normalY = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormalCooY()));
	EntityPropertiesDouble *normalZ = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormalCooZ()));

	EntityPropertiesDouble *centerX = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameCenterX()));
	EntityPropertiesDouble *centerY = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameCenterY()));
	EntityPropertiesDouble *centerZ = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameCenterZ()));

	bool updatePropertiesGrid = false;

	PlaneProperties::NormalDescription normal = properties.GetNormalDescription(normalDirection->getValue());
	assert(normal != PlaneProperties::NormalDescription::UNKNOWN);

	if (normal == PlaneProperties::Free)
	{
		if (!normalX->getVisible()) updatePropertiesGrid = true;
		if (!normalY->getVisible()) updatePropertiesGrid = true;
		if (!normalZ->getVisible()) updatePropertiesGrid = true;

		normalX->setVisible(true);
		normalY->setVisible(true);
		normalZ->setVisible(true);
	}
	else
	{
		if (normalX->getVisible()) updatePropertiesGrid = true;
		if (normalY->getVisible()) updatePropertiesGrid = true;
		if (normalZ->getVisible()) updatePropertiesGrid = true;

		normalX->setVisible(false);
		normalY->setVisible(false);
		normalZ->setVisible(false);
	}

	bool centerVisibleX = normal == PlaneProperties::Free || normal == PlaneProperties::X ;
	bool centerVisibleY = normal == PlaneProperties::Free || normal == PlaneProperties::Y;
	bool centerVisibleZ = normal == PlaneProperties::Free || normal == PlaneProperties::Z;

	if (centerVisibleX != centerX->getVisible()) updatePropertiesGrid = true;
	if (centerVisibleY != centerY->getVisible()) updatePropertiesGrid = true;
	if (centerVisibleZ != centerZ->getVisible()) updatePropertiesGrid = true;

	centerX->setVisible(centerVisibleX);
	centerY->setVisible(centerVisibleY);
	centerZ->setVisible(centerVisibleZ);

	return updatePropertiesGrid;
}
