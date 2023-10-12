
#include "EntityVisUnstructuredVectorVolume.h"
#include "Types.h"
#include "Database.h"

#include <OpenTwinCommunication/ActionTypes.h>

#include <bsoncxx/builder/basic/array.hpp>

EntityVisUnstructuredVectorVolume::EntityVisUnstructuredVectorVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner) :
	EntityVis2D3D(ID, parent, obs, ms, factory, owner)
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

	propertyBundleVisUnstructuredVector.SetProperties(this);
	propertyBundlePlane.SetProperties(this);
	propertyBundleScaling.SetProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVisUnstructuredVectorVolume::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;
	bool is2dType = false;

	updatePropertiesGrid |= propertyBundleVisUnstructuredVector.UpdatePropertyVisibility(this);

	if (propertyBundleVisUnstructuredVector.is2dType(this))
	{
		updatePropertiesGrid |= propertyBundlePlane.UpdatePropertyVisibility(this);
	}
	else
	{
		updatePropertiesGrid |= propertyBundlePlane.HidePlane(this);
	}

	updatePropertiesGrid |= propertyBundleScaling.UpdatePropertyVisibility(this);

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
