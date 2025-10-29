// @otlicense

#include "PropertyBundleDataHandle.h"
#include "..\include\PropertyBundleDataHandleVisUnstructuredVectorSurface.h"

PropertyBundleDataHandleVisUnstructuredVectorSurface::PropertyBundleDataHandleVisUnstructuredVectorSurface(EntityBase * thisObject)
{
	LoadCurrentData(thisObject);
}

void PropertyBundleDataHandleVisUnstructuredVectorSurface::LoadCurrentData(EntityBase * thisObject)
{
	auto maxArrowsEnt = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNameMaxArrows()));
	assert(maxArrowsEnt != nullptr);
	maxArrows = maxArrowsEnt->getValue();

	auto arrowTypeEnt = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameArrowType()));
	assert(arrowTypeEnt != nullptr);
	selectedArrowType = properties.GetArrowType(arrowTypeEnt->getValue());

	auto arrowScaleEnt = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetNameArrowScale()));
	assert(arrowScaleEnt != nullptr);
	arrowScale = arrowScaleEnt->getValue();
}
