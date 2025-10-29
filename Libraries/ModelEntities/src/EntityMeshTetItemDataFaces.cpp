// @otlicense


#include "EntityMeshTetItemDataFaces.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshTetItemDataFaces> registrar("EntityMeshTetItemDataFaces");

EntityMeshTetItemDataFaces::EntityMeshTetItemDataFaces(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, owner)
{
	
}

EntityMeshTetItemDataFaces::~EntityMeshTetItemDataFaces()
{

}

bool EntityMeshTetItemDataFaces::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityMeshTetItemDataFaces::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Add the specific data of the mesh item (face list)

	auto f = bsoncxx::builder::basic::array();

	for (auto face : faces)
	{
		f.append((int) face);
	}

	storage.append(bsoncxx::builder::basic::kvp("Faces", f));
}

void EntityMeshTetItemDataFaces::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the mesh item (face list)

	auto face = doc_view["Faces"].get_array().value;

	size_t numberFaces = std::distance(face.begin(), face.end());

	auto f = face.begin();

	setNumberFaces(numberFaces);

	for (unsigned long findex = 0; findex < numberFaces; findex++)
	{
		setFace(findex, (int) DataBase::getIntFromArrayViewIterator(f));
		f++;
	}

	resetModified();
}
