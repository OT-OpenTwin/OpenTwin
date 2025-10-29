// @otlicense

#include "PropertyBundleScaling.h"

void PropertyBundleScaling::setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupName, m_properties.GetPropertyNameScalingMethod(), 
		{ 
			m_properties.GetPropertyValueAutoscaling(), 
			m_properties.GetPropertyValueRangeScaling() 
		}, m_properties.GetPropertyValueAutoscaling(), m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesSelection::createProperty(m_groupName, m_properties.GetPropertyNameScalingFunction(),
		{
			m_properties.GetPropertyValueLinScaling(),
			m_properties.GetPropertyValueLogscaling()
		}, m_properties.GetPropertyValueLinScaling(), m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupName, m_properties.GetPropertyNameRangeMin(), 0.0, m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesDouble::createProperty(m_groupName, m_properties.GetPropertyNameRangeMax(), 0.0, m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesInteger::createProperty(m_groupName, m_properties.GetPropertyNameColourResolution(), 30., m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleScaling::updatePropertyVisibility(EntityBase * _thisObject)
{
	auto scalingMethod = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameScalingMethod()));

	auto minScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameRangeMin()));
	auto maxScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameRangeMax()));

	bool updatePropertiesGrid = false;
	if (scalingMethod->getValue() == m_properties.GetPropertyValueRangeScaling())
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

