#include "PropertyBundleVisUnstructuredVector.h"

void PropertyBundleVisUnstructuredVector:: SetProperties(EntityBase * thisObject)
{
	EntityPropertiesSelection::createProperty(groupName, properties.GetNameVisType(),
		{
			properties.GetValueArrows3D(),
			properties.GetValueArrows2D(),
			properties.GetValueContour2D()
		}, properties.GetValueArrows3D(), defaultCategory, thisObject->getProperties());
	
	EntityPropertiesSelection::createProperty(groupName, properties.GetNameVisComponent(),
		{
			properties.GetValueComponentX(),
			properties.GetValueComponentY(),
			properties.GetValueComponentZ(),
			properties.GetValueAbsolute()
		}, properties.GetValueAbsolute(), defaultCategory, thisObject->getProperties());

	EntityPropertiesInteger::createProperty(groupName, properties.GetNamePlotDownSampling(), 0., defaultCategory, thisObject->getProperties());
}

bool PropertyBundleVisUnstructuredVector::UpdatePropertyVisibility(EntityBase * thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	EntityPropertiesSelection* visComp = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisComponent()));
	EntityPropertiesInteger* downSampling = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNamePlotDownSampling()));

	assert(visType != nullptr);
	assert(visComp != nullptr);
	assert(downSampling != nullptr);

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
		if (!downSampling->getVisible())
		{
			downSampling->setVisible(true);
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
		if (!downSampling->getVisible())
		{
			downSampling->setVisible(true);
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
		if (downSampling->getVisible())
		{
			downSampling->setVisible(false);
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

