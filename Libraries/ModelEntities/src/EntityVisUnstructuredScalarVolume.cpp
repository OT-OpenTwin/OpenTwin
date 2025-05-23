
#include "EntityVisUnstructuredScalarVolume.h"
#include "OldTreeIcon.h"
#include "Database.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityVisUnstructuredScalarVolume::EntityVisUnstructuredScalarVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner) :
	EntityVis2D3D(ID, parent, obs, ms, factory, owner)
{
}

EntityVisUnstructuredScalarVolume::~EntityVisUnstructuredScalarVolume()
{

}

void EntityVisUnstructuredScalarVolume::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityVis2D3D::AddStorageData(storage);

}

void EntityVisUnstructuredScalarVolume::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	resetModified();
}

void EntityVisUnstructuredScalarVolume::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundleVisUnstructuredScalar.setProperties(this);
	propertyBundlePlane.setProperties(this);
	propertyBundleScaling.setProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVisUnstructuredScalarVolume::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;
	bool is2dType = false;

	updatePropertiesGrid |= propertyBundleVisUnstructuredScalar.updatePropertyVisibility(this);

	if (propertyBundleVisUnstructuredScalar.is2dType(this))
	{
		updatePropertiesGrid |= propertyBundlePlane.updatePropertyVisibility(this);
	}
	else
	{
		updatePropertiesGrid |= propertyBundlePlane.hidePlane(this);
	}

	updatePropertiesGrid |= propertyBundleScaling.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}


bool EntityVisUnstructuredScalarVolume::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}
