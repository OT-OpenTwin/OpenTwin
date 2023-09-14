
#include "EntityVisUnstructuredScalarSurface.h"
#include "Types.h"
#include "Database.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityVisUnstructuredScalarSurface::EntityVisUnstructuredScalarSurface(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner) :
	EntityVis2D3D(ID, parent, obs, ms, factory, owner)
{
}

EntityVisUnstructuredScalarSurface::~EntityVisUnstructuredScalarSurface()
{

}

void EntityVisUnstructuredScalarSurface::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityVis2D3D::AddStorageData(storage);
}

void EntityVisUnstructuredScalarSurface::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);
}

void EntityVisUnstructuredScalarSurface::createProperties(void)
{
	assert(getResultType() != EntityResultBase::UNKNOWN);

	// Plane
	EntityPropertiesSelection::createProperty("Plane", "Normal", {"X", "Y", "Z"}, "Z", "2D Visualization Cartesian Face Scalar", getProperties());
	EntityPropertiesInteger::createProperty(   "Plane", "Plane index X", 0, "2D Visualization Cartesian Face Scalar", getProperties());
	EntityPropertiesInteger::createProperty(   "Plane", "Plane index Y", 0, "2D Visualization Cartesian Face Scalar", getProperties());
	EntityPropertiesInteger::createProperty(   "Plane", "Plane index Z", 0, "2D Visualization Cartesian Face Scalar", getProperties());

	// Scaling
	EntityPropertiesBoolean::createProperty("Scaling", "Autoscale",	true, "2D Visualization Cartesian Face Scalar", getProperties());
	EntityPropertiesDouble::createProperty( "Scaling", "Min", 0.0, "2D Visualization Cartesian Face Scalar", getProperties());
	EntityPropertiesDouble::createProperty( "Scaling", "Max", 0.0, "2D Visualization Cartesian Face Scalar", getProperties());
	EntityPropertiesBoolean::createProperty("Scaling", "Logscale",	false, "2D Visualization Cartesian Face Scalar", getProperties());

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVisUnstructuredScalarSurface::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesSelection *normalDirection = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Normal"));

	EntityPropertiesInteger *centerX = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Plane index X"));
	EntityPropertiesInteger *centerY = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Plane index Y"));
	EntityPropertiesInteger *centerZ = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Plane index Z"));

	bool centerVisibleX = normalDirection->getValue() == "X";
	bool centerVisibleY = normalDirection->getValue() == "Y";
	bool centerVisibleZ = normalDirection->getValue() == "Z";

	if (centerVisibleX != centerX->getVisible()) updatePropertiesGrid = true;
	if (centerVisibleY != centerY->getVisible()) updatePropertiesGrid = true;
	if (centerVisibleZ != centerZ->getVisible()) updatePropertiesGrid = true;

	centerX->setVisible(centerVisibleX);
	centerY->setVisible(centerVisibleY);
	centerZ->setVisible(centerVisibleZ);

	EntityPropertiesBoolean *autoscale = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Autoscale"));

	EntityPropertiesDouble *minScale = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Min"));
	EntityPropertiesDouble *maxScale = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Max"));

	if (autoscale->getValue())
	{
		minScale->setVisible(false);
		maxScale->setVisible(false);
		updatePropertiesGrid = true;
	}
	else
	{
		minScale->setVisible(true);
		maxScale->setVisible(true);
		updatePropertiesGrid = true;
	}

	return updatePropertiesGrid;
}

