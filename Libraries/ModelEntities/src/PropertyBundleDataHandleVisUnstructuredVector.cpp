// @otlicense

#include "PropertyBundleDataHandle.h"
#include "..\include\PropertyBundleDataHandleVisUnstructuredVector.h"

PropertyBundleDataHandleVisUnstructuredVector::PropertyBundleDataHandleVisUnstructuredVector(EntityBase * thisObject)
{
	LoadCurrentData(thisObject);
}

void PropertyBundleDataHandleVisUnstructuredVector::LoadCurrentData(EntityBase * thisObject)
{
	auto visType = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisType()));
	assert(visType != nullptr);
	selectedVisType = properties.GetVisualizationType(visType->getValue());

	auto visComp = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameVisComponent()));
	assert(visComp != nullptr);
	selectedVisComp = properties.GetVisualizationComponent(visComp->getValue());

	auto maxArrowsEnt = dynamic_cast<EntityPropertiesInteger*>(thisObject->getProperties().getProperty(properties.GetNameMaxArrows()));
	assert(maxArrowsEnt != nullptr);
	maxArrows = maxArrowsEnt->getValue();

	auto arrowTypeEnt = dynamic_cast<EntityPropertiesSelection*>(thisObject->getProperties().getProperty(properties.GetNameArrowType()));
	assert(arrowTypeEnt != nullptr);
	selectedArrowType = properties.GetArrowType(arrowTypeEnt->getValue());

	auto arrowScaleEnt = dynamic_cast<EntityPropertiesDouble*>(thisObject->getProperties().getProperty(properties.GetNameArrowScale()));
	assert(arrowScaleEnt != nullptr);
	arrowScale = arrowScaleEnt->getValue();

	auto show2dMeshEnt = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DMesh()));
	assert(show2dMeshEnt != nullptr);
	show2dMesh = show2dMeshEnt->getValue();

	auto color2dMeshEnt = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DMeshColor()));
	assert(color2dMeshEnt != nullptr);
	color2dMesh[0] = color2dMeshEnt->getColorR();
	color2dMesh[1] = color2dMeshEnt->getColorG();
	color2dMesh[2] = color2dMeshEnt->getColorB();

	auto show2dIsolinesEnt = dynamic_cast<EntityPropertiesBoolean*>(thisObject->getProperties().getProperty(properties.GetNameShow2DIsolines()));
	assert(show2dIsolinesEnt != nullptr);
	show2dIsolines = show2dIsolinesEnt->getValue();
	
	auto color2dIsolinesEnt = dynamic_cast<EntityPropertiesColor*>(thisObject->getProperties().getProperty(properties.GetName2DIsolineColor()));
	assert(color2dIsolinesEnt != nullptr);
	color2dIsolines[0] = color2dIsolinesEnt->getColorR();
	color2dIsolines[1] = color2dIsolinesEnt->getColorG();
	color2dIsolines[2] = color2dIsolinesEnt->getColorB();
}
