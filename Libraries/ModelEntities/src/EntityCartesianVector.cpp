
#include "EntityCartesianVector.h"
#include "EntityMeshCartesianData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityCartesianVector::EntityCartesianVector(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityCompressedVector(ID, parent, obs, ms, factory, owner)
{
}

EntityCartesianVector::~EntityCartesianVector()
{
}

bool EntityCartesianVector::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityCartesianVector::StoreToDataBase(void)
{
	EntityCompressedVector::StoreToDataBase();
}

void EntityCartesianVector::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityCompressedVector::AddStorageData(storage);

	// Now add the actual cartesian vector data

	storage.append(
		bsoncxx::builder::basic::kvp("resultType", (long long)getResultType())
	);
}

void EntityCartesianVector::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityCompressedVector::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the cartesian vector data

	setResultType(static_cast<tResultType>((long long) doc_view["resultType"].get_int64()));

	resetModified();
}

void EntityCartesianVector::removeChild(EntityBase *child)
{
	EntityCompressedVector::removeChild(child);
}

