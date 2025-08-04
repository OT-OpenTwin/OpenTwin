#include "PropertyBundleVisUnstructuredVectorSurface.h"

void PropertyBundleVisUnstructuredVectorSurface:: setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty(m_groupNameArrows, m_properties.GetNameArrowType(),
		{
			m_properties.GetValueArrowTypeShaded(),
			m_properties.GetValueArrowTypeFlat(),
			m_properties.GetValueArrowTypeHedgehog(),
		}, m_properties.GetValueArrowTypeShaded(), m_defaultCategory, _thisObject->getProperties());

	EntityPropertiesDouble::createProperty(m_groupNameArrows, m_properties.GetNameArrowScale(), 1., m_defaultCategory, _thisObject->getProperties());
	EntityPropertiesInteger::createProperty(m_groupNameArrows, m_properties.GetNameMaxArrows(), 1000, m_defaultCategory, _thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredVectorSurface::updatePropertyVisibility(EntityBase * _thisObject)
{
	EntityPropertiesSelection* arrowType = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowType()));
	EntityPropertiesDouble* arrowScale = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetNameArrowScale()));
	EntityPropertiesInteger* maxArrows = dynamic_cast<EntityPropertiesInteger*>(_thisObject->getProperties().getProperty(m_properties.GetNameMaxArrows()));

	assert(arrowType != nullptr);
	assert(arrowScale != nullptr);
	assert(maxArrows != nullptr);

	bool changes = false;

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

	return changes;
}

