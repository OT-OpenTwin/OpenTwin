#include "PropertyBundleVis2D3D.h"

void PropertyBundleVis2D3D:: SetProperties(EntityBase * thisObject)
{
	EntityPropertiesSelection::createProperty(groupName, properties.GetNameVisType(),
		{
			properties.GetValueArrows(),
			properties.GetValueContour()
		}, properties.GetValueArrows(), defaultCategory, thisObject->getProperties());
	
	EntityPropertiesSelection::createProperty(groupName, properties.GetNameVisComponent(),
		{
			properties.GetValueComponentX(),
			properties.GetValueComponentY(),
			properties.GetValueComponentZ(),
			properties.GetValueAbsolute()
		}, properties.GetValueAbsolute(), defaultCategory, thisObject->getProperties());

	//Not used yet
	/*EntityPropertiesDouble::createProperty(groupName, properties.GetNamePhase(), 0.0, defaultCategory, thisObject->getProperties());
	EntityPropertiesDouble::createProperty(groupName, properties.GetNameTime(), 0.0, defaultCategory, thisObject->getProperties());*/
	EntityPropertiesInteger::createProperty(groupName, properties.GetNamePlotDownSampling(), 0., defaultCategory, thisObject->getProperties());
}

bool PropertyBundleVis2D3D::UpdatePropertyVisibility(EntityBase * thisObject)
{
	EntityPropertiesSelection* visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	EntityPropertiesSelection* visComp = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisComponent()));
	EntityPropertiesInteger* downSampling = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNamePlotDownSampling()));

	assert(visType != nullptr);
	assert(visComp != nullptr);
	assert(downSampling != nullptr);

	PropertiesVis2D3D::VisualizationType type = properties.GetVisualizationType(visType->getValue());
	assert(type != PropertiesVis2D3D::VisualizationType::UNKNOWN);

	if (type == PropertiesVis2D3D::VisualizationType::Arrows)
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
	else
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

	return false;
}
