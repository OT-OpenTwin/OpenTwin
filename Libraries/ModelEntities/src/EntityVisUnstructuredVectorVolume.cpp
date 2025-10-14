
#include "EntityVisUnstructuredVectorVolume.h"
#include "OldTreeIcon.h"
#include "Database.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityVisUnstructuredVectorVolume> registrar("EntityVisUnstructuredVectorVolume");

EntityVisUnstructuredVectorVolume::EntityVisUnstructuredVectorVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner) :
	EntityVis2D3D(ID, parent, obs, ms, owner)
{
}

EntityVisUnstructuredVectorVolume::~EntityVisUnstructuredVectorVolume()
{

}

void EntityVisUnstructuredVectorVolume::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityVis2D3D::AddStorageData(storage);

}

void EntityVisUnstructuredVectorVolume::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	resetModified();
}

void EntityVisUnstructuredVectorVolume::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundleVisUnstructuredVector.setProperties(this);
	propertyBundlePlane.setProperties(this);
	propertyBundleScaling.setProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVisUnstructuredVectorVolume::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;
	bool is2dType = false;

	updatePropertiesGrid |= propertyBundleVisUnstructuredVector.updatePropertyVisibility(this);

	if (propertyBundleVisUnstructuredVector.is2dType(this))
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


bool EntityVisUnstructuredVectorVolume::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}
