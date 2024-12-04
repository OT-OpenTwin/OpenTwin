#include "..\include\PropertyBundleDataHandlePlane.h"

PropertyBundleDataHandlePlane::PropertyBundleDataHandlePlane(EntityBase * thisObject)
{
	LoadCurrentData(thisObject);
}

void PropertyBundleDataHandlePlane::LoadCurrentData(EntityBase * thisObject)
{
	PlaneProperties properties;

	auto normalProperty = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormal()));
	assert(normalProperty != nullptr);
	normalDescription = properties.GetNormalDescription(normalProperty->getValue());

	auto centerXProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameCenterX()));
	assert(centerXProperty != nullptr);
	centerX = centerXProperty->getValue();
	auto centerYProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameCenterY()));
	assert(centerYProperty != nullptr);
	centerY = centerYProperty->getValue();
	auto centerZProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameCenterZ()));
	assert(centerZProperty != nullptr);
	centerZ = centerZProperty->getValue();

	auto normalXProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormalCooX()));
	assert(normalXProperty != nullptr);
	normalValueX = normalXProperty->getValue();
	auto normalYProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormalCooY()));
	assert(normalYProperty != nullptr);
	normalValueY = normalYProperty->getValue();
	auto normalZProperty = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameNormalCooZ()));
	assert(normalZProperty != nullptr);
	normalValueZ = normalZProperty->getValue();

}
