#include "PropertyBundleScaling.h"

void PropertyBundleScaling::SetProperties(EntityBase * thisObject)
{
	EntityPropertiesSelection::createProperty(groupName, properties.GetPropertyNameScalingMethod(), 
		{ 
			properties.GetPropertyValueAutoscaling(), 
			properties.GetPropertyValueRangeScaling() 
		}, properties.GetPropertyValueAutoscaling(), defaultCategory, thisObject->getProperties());
	EntityPropertiesSelection::createProperty(groupName, properties.GetPropertyNameScalingFunction(),
		{
			properties.GetPropertyValueLinScaling(),
			properties.GetPropertyValueLogscaling()
		}, properties.GetPropertyValueLinScaling(), defaultCategory, thisObject->getProperties());

	EntityPropertiesDouble::createProperty(groupName, properties.GetPropertyNameRangeMin(), 0.0, defaultCategory, thisObject->getProperties());
	EntityPropertiesDouble::createProperty(groupName, properties.GetPropertyNameRangeMax(), 0.0, defaultCategory, thisObject->getProperties());

	EntityPropertiesInteger::createProperty(groupName, properties.GetPropertyNameColourResolution(), 30., defaultCategory, thisObject->getProperties());
}

bool PropertyBundleScaling::UpdatePropertyVisibility(EntityBase * thisObject)
{
	auto scalingMethod = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetPropertyNameScalingMethod()));

	auto minScale = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameRangeMin()));
	auto maxScale = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetPropertyNameRangeMax()));

	bool updatePropertiesGrid = false;
	if (scalingMethod->getValue() == properties.GetPropertyValueRangeScaling())
	{
		if (!minScale->getVisible())
		{
			minScale->setVisible(true);
			updatePropertiesGrid = true;
		};
		if (!maxScale->getVisible())
		{
			maxScale->setVisible(true);
			updatePropertiesGrid = true;
		}
	}
	else
	{
		if (minScale->getVisible())
		{
			minScale->setVisible(false);
			updatePropertiesGrid = true;
		};
		if (maxScale->getVisible())
		{
			maxScale->setVisible(false);
			updatePropertiesGrid = true;
		}
	}
	return updatePropertiesGrid;
}

