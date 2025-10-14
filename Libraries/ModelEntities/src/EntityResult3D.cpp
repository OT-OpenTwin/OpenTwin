#include "../include/EntityResult3D.h"

static EntityFactoryRegistrar<EntityResult3D> registrar("EntityResult3D");

EntityResult3D::EntityResult3D(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, const std::string & owner)
	: EntityVis2D3D(ID, parent, obs, ms, owner) {}

void EntityResult3D::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundlePlane.setProperties(this);
	propertyBundleScaling.setProperties(this);
	propertyBundleVis2D3D.setProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityResult3D::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	updatePropertiesGrid = propertyBundlePlane.updatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleScaling.updatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleVis2D3D.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}
