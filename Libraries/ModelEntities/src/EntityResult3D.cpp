#include "../include/EntityResult3D.h"

EntityResult3D::EntityResult3D(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactory * factory, const std::string & owner)
	: EntityVis2D3D(ID, parent, obs, ms, factory, owner) {}

void EntityResult3D::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundlePlane.SetProperties(this);
	propertyBundleScaling.SetProperties(this);
	propertyBundleVis2D3D.SetProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityResult3D::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	updatePropertiesGrid = propertyBundlePlane.UpdatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleScaling.UpdatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleVis2D3D.UpdatePropertyVisibility(this);

	return updatePropertiesGrid;
}
