
#include "EntityMeshTetFaceData.h"
#include "EntityMeshTetFace.h"
#include "EntityMeshTetInfo.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityMeshTetFaceData::EntityMeshTetFaceData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms,  factory, owner)
{
	
}

EntityMeshTetFaceData::~EntityMeshTetFaceData()
{
	// We clear the indices first, such that the deletion of the child entity will not cause a removal of the face from the 
	// map (which would cause a failure of the loop)
	meshFacesIndex.clear();
	meshFaceStorageIds.clear();

	for (auto fp : meshFaces)
	{
		if (fp.second != nullptr) delete fp.second;
	}

	meshFaces.clear();
}

bool EntityMeshTetFaceData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

EntityMeshTetFace *EntityMeshTetFaceData::getFace(int faceId)
{
	EnsureFacesLoaded();
	return meshFaces[faceId];
}

void EntityMeshTetFaceData::setFace(int faceId, EntityMeshTetFace *face)
{
	EnsureFacesLoaded();

	face->setParent(this);
	meshFaces[faceId] = face;
	meshFacesIndex[face] = faceId;
}

void EntityMeshTetFaceData::EnsureFacesLoaded(void)
{
	if (meshFaces.empty() && !meshFaceStorageIds.empty())
	{
		// We need to restore the faces from the data base

		for (auto fp : meshFaceStorageIds)
		{
			std::map<ot::UID, EntityBase *> entityMap;
			EntityMeshTetFace *face = dynamic_cast<EntityMeshTetFace *>(readEntityFromEntityID(this, fp.second.first, entityMap));
			meshFaces[fp.first] = face;
			meshFacesIndex[face] = fp.first;
		}
	}
}

void EntityMeshTetFaceData::storeMeshFaces(void)
{
	for (auto fp : meshFaces)
	{
		if (fp.second != nullptr)
		{
			fp.second->StoreToDataBase();
			meshFaceStorageIds[fp.first] = std::pair<ot::UID, ot::UID>(fp.second->getEntityID(), fp.second->getEntityStorageVersion());
		}
	}
}

void EntityMeshTetFaceData::releaseMeshFaces(void)
{
	storeMeshFaces();

	// We clear the indices first, such that the deletion of the child entity will not cause a removal of the face from the 
	// map (which would cause a failure of the loop)
	meshFacesIndex.clear();

	for (auto fp : meshFaces)
	{
		if (fp.second != nullptr) delete fp.second;
	}

	meshFaces.clear();
}

void EntityMeshTetFaceData::StoreToDataBase(void)
{
	// If the pointers to faces, then the objects are stored in the data storage and the storage IDs are up to date
	storeMeshFaces();

	// Afterward, we store the container itself
	EntityBase::StoreToDataBase();
}

void EntityMeshTetFaceData::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now check whether the geometry is modified and we need to create a new entry

	auto faces_id = bsoncxx::builder::basic::array();
	auto faces_storageID = bsoncxx::builder::basic::array();
	auto faces_storageVersion = bsoncxx::builder::basic::array();

	for (auto face : meshFaceStorageIds)
	{
		faces_id.append((int)face.first);
		faces_storageID.append((long long)face.second.first);
		faces_storageVersion.append((long long)face.second.second);
	}

	storage.append(
		bsoncxx::builder::basic::kvp("MeshFacesIndex", faces_id),
		bsoncxx::builder::basic::kvp("MeshFacesID", faces_storageID),
		bsoncxx::builder::basic::kvp("MeshFacesVersion", faces_storageVersion)
	);
}

void EntityMeshTetFaceData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the coordinates

	// First reset the current mesh data

	// We clear the indices first, such that the deletion of the child entity will not cause a removal of the face from the 
	// map (which would cause a failure of the loop)
	meshFacesIndex.clear();
	meshFaceStorageIds.clear();

	for (auto fp : meshFaces)
	{
		if (fp.second != nullptr) delete fp.second;
	}

	meshFaces.clear();

	// Now load the data from the storage

	auto faces_id = doc_view["MeshFacesIndex"].get_array().value;
	auto faces_storageID = doc_view["MeshFacesID"].get_array().value;
	auto faces_storageVersion = doc_view["MeshFacesVersion"].get_array().value;

	size_t numberFaces = std::distance(faces_id.begin(), faces_id.end());
	assert(numberFaces == std::distance(faces_storageID.begin(), faces_storageID.end()));
	assert(numberFaces == std::distance(faces_storageVersion.begin(), faces_storageVersion.end()));

	auto id = faces_id.begin();
	auto faceID = faces_storageID.begin();
	auto faceVersion = faces_storageVersion.begin();

	for (unsigned long index = 0; index < numberFaces; index++)
	{
		meshFaceStorageIds[(int)DataBase::GetIntFromArrayViewIterator(id)] = std::pair<ot::UID, ot::UID>(faceID->get_int64(), faceVersion->get_int64());

		id++;
		faceID++;
		faceVersion++;
	}

	resetModified();
}

void EntityMeshTetFaceData::addMeshFaceStorageIds(EntityMeshTetInfo *meshInfo)
{
	for (auto item : meshFaceStorageIds)
	{
		meshInfo->addFacesData(item.second.first, item.second.second);
	}
}

void EntityMeshTetFaceData::removeChild(EntityBase *child)
{
	if (meshFacesIndex.count(child) > 0)
	{
		int index = meshFacesIndex[child];
		meshFacesIndex.erase(child);
		meshFaces.erase(index);
	}
	else
	{
		EntityBase::removeChild(child);
	}
}
