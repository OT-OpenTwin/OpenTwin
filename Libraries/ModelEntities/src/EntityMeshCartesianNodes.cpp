
#include "EntityMeshCartesianNodes.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityMeshCartesianNodes::EntityMeshCartesianNodes(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner)
{
	
}

EntityMeshCartesianNodes::~EntityMeshCartesianNodes()
{

}

bool EntityMeshCartesianNodes::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityMeshCartesianNodes::setNumberOfNodes(size_t n)
{
	nodeCoordsX.resize(n);
	nodeCoordsY.resize(n);
	nodeCoordsZ.resize(n);

	setModified();
}

void EntityMeshCartesianNodes::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now check whether the geometry is modified and we need to create a new entry

	auto coordX = bsoncxx::builder::basic::array();
	auto coordY = bsoncxx::builder::basic::array();
	auto coordZ = bsoncxx::builder::basic::array();

	for (size_t i = 0; i < nodeCoordsX.size(); i++)
	{
		coordX.append(nodeCoordsX[i]);
		coordY.append(nodeCoordsY[i]);
		coordZ.append(nodeCoordsZ[i]);
	}

	storage.append(bsoncxx::builder::basic::kvp("CoordX", coordX));
	storage.append(bsoncxx::builder::basic::kvp("CoordY", coordY));
	storage.append(bsoncxx::builder::basic::kvp("CoordZ", coordZ));
}

void EntityMeshCartesianNodes::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the coordinates
	auto coordX = doc_view["CoordX"].get_array().value;
	auto coordY = doc_view["CoordY"].get_array().value;
	auto coordZ = doc_view["CoordZ"].get_array().value;

	size_t numberCoordinates = std::distance(coordX.begin(), coordX.end());
	assert(numberCoordinates == std::distance(coordY.begin(), coordY.end()));
	assert(numberCoordinates == std::distance(coordZ.begin(), coordZ.end()));

	auto cx = coordX.begin();
	auto cy = coordY.begin();
	auto cz = coordZ.begin();

	setNumberOfNodes(numberCoordinates);

	for (unsigned long index = 0; index < numberCoordinates; index++)
	{
		nodeCoordsX[index] = cx->get_double();
		nodeCoordsY[index] = cy->get_double();
		nodeCoordsZ[index] = cz->get_double();

		cx++;
		cy++;
		cz++;
	}

	resetModified();
}
