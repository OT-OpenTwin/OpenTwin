#include "PropertyBundleDataHandle.h"
#include "..\include\PropertyBundleDataHandleVis2D3D.h"

PropertyBundleDataHandleVis2D3D::PropertyBundleDataHandleVis2D3D(EntityBase * thisObject)
{
	LoadCurrentData(thisObject);
}

void PropertyBundleDataHandleVis2D3D::LoadCurrentData(EntityBase * thisObject)
{
	auto visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	assert(visType != nullptr);
	selectedVisType = properties.GetVisualizationType(visType->getValue());

	auto visComp = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisComponent()));
	assert(visComp != nullptr);
	selectedVisComp = properties.GetVisualizationComponent(visComp->getValue());

	auto downSamplingEnt = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNamePlotDownSampling()));
	assert(downSamplingEnt != nullptr);
	downsamplingRate = downSamplingEnt->getValue();
}
