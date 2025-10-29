// @otlicense


#include "EntityVisUnstructuredScalarSurface.h"
#include "OldTreeIcon.h"
#include "Database.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityVisUnstructuredScalarSurface> registrar("EntityVisUnstructuredScalarSurface");

EntityVisUnstructuredScalarSurface::EntityVisUnstructuredScalarSurface(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner) :
	EntityVis2D3D(ID, parent, obs, ms, owner)
{
}

EntityVisUnstructuredScalarSurface::~EntityVisUnstructuredScalarSurface()
{

}

void EntityVisUnstructuredScalarSurface::addStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityVis2D3D::addStorageData(storage);

}

void EntityVisUnstructuredScalarSurface::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	resetModified();
}

void EntityVisUnstructuredScalarSurface::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundleVisUnstructuredScalarSurface.setProperties(this);
	propertyBundleScaling.setProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVisUnstructuredScalarSurface::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;
	bool is2dType = false;

	updatePropertiesGrid |= propertyBundleVisUnstructuredScalarSurface.updatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleScaling.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}


bool EntityVisUnstructuredScalarSurface::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}
