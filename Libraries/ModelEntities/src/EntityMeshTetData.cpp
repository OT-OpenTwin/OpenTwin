// @otlicense
// File: EntityMeshTetData.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end


#include "DataBase.h"
#include "EntityMeshTetData.h"
#include "EntityMeshTetNodes.h"
#include "EntityMeshTetFace.h"
#include "EntityMeshTetFaceData.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshTetData> registrar("EntityMeshTetData");

EntityMeshTetData::EntityMeshTetData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityContainer(ID, parent, obs, ms),
	meshNodes(nullptr),
	meshNodesStorageId(-1),
	meshFaces(nullptr),
	meshFacesStorageId(-1),
	gmshDataStorageId(-1)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/ContainerVisible");
	treeItem.setHiddenIcon("Default/ContainerHidden");
	this->setDefaultTreeItem(treeItem);
}

EntityMeshTetData::~EntityMeshTetData()
{
	if (meshNodes != nullptr) delete meshNodes;
	meshNodes = nullptr;

	if (meshFaces != nullptr) delete meshFaces;
	meshFaces = nullptr;
}

bool EntityMeshTetData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

EntityMeshTetFace *EntityMeshTetData::getFace(int faceId)
{
	EnsureFacesLoaded();
	assert(meshFaces != nullptr);

	return meshFaces->getFace(faceId);
}

void EntityMeshTetData::setFace(int faceId, EntityMeshTetFace *face)
{
	EnsureFacesLoaded();
	assert(meshFaces != nullptr);

	return meshFaces->setFace(faceId, face);
}

void EntityMeshTetData::EnsureFacesLoaded(void)
{
	if (meshFaces == nullptr)
	{
		if (meshFacesStorageId == -1)
		{
			meshFaces = new EntityMeshTetFaceData(0, this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshFaces = dynamic_cast<EntityMeshTetFaceData *>(readEntityFromEntityID(this, meshFacesStorageId, entityMap));
		}
	}
}

void EntityMeshTetData::storeMeshFaces(void)
{
	assert(meshFaces != nullptr);

	// Assign an entity id if needed
	if (meshFaces->getEntityID() == 0)
	{
		meshFaces->setEntityID(createEntityUID());
	}

	meshFaces->storeToDataBase();
	meshFacesStorageId = meshFaces->getEntityID();
}

void EntityMeshTetData::releaseMeshFaces(void)
{
	if (meshFaces == nullptr) return;

	storeMeshFaces();

	delete meshFaces;
	meshFaces = nullptr;
}

void EntityMeshTetData::getNodeCoords(size_t node, double coord[3])
{
	EnsureNodesLoaded();

	assert(meshNodes != nullptr);

	coord[0] = meshNodes->getNodeCoordX(node);
	coord[1] = meshNodes->getNodeCoordY(node);
	coord[2] = meshNodes->getNodeCoordZ(node);
}

double EntityMeshTetData::getNodeCoordX(size_t node)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	return meshNodes->getNodeCoordX(node);
}

double EntityMeshTetData::getNodeCoordY(size_t node)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	return meshNodes->getNodeCoordY(node);
}

double EntityMeshTetData::getNodeCoordZ(size_t node)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	return meshNodes->getNodeCoordZ(node);
}

void EntityMeshTetData::setNodeCoordX(size_t node, double x)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNodeCoordX(node, x);
}

void EntityMeshTetData::setNodeCoordY(size_t node, double y)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNodeCoordY(node, y);
}

void EntityMeshTetData::setNodeCoordZ(size_t node, double z)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNodeCoordZ(node, z);
}

void EntityMeshTetData::setNumberOfNodes(size_t n)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNumberOfNodes(n);
}

void EntityMeshTetData::EnsureNodesLoaded(void)
{
	if (meshNodes == nullptr)
	{
		if (meshNodesStorageId == -1)
		{
			meshNodes = new EntityMeshTetNodes(0, this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshNodes = dynamic_cast<EntityMeshTetNodes *>(readEntityFromEntityID(this, meshNodesStorageId, entityMap));
		}
	}
}

void EntityMeshTetData::storeMeshNodes(void)
{
	assert(meshNodes != nullptr);

	// Assign an entity id if needed
	if (meshNodes->getEntityID() == 0)
	{
		meshNodes->setEntityID(createEntityUID());
	}

	meshNodes->storeToDataBase();
	meshNodesStorageId = meshNodes->getEntityID();
}

void EntityMeshTetData::releaseMeshNodes(void)
{
	if (meshNodes == nullptr) return;

	storeMeshNodes();

	delete meshNodes;
	meshNodes = nullptr;
}

void EntityMeshTetData::storeToDataBase(void)
{
	// If the pointers to faces or nodes are 0, then the objects are stored in the data storage and the storage IDs are up to date
	if (meshFaces != nullptr)
	{
		storeMeshFaces();
	}

	if (meshNodes != nullptr)
	{
		storeMeshNodes();
	}

	// Afterward, we store the container itself
	EntityContainer::storeToDataBase();
}

void EntityMeshTetData::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::addStorageData(storage);

	// Now check whether the geometry is modified and we need to create a new entry

	long long meshNodesVersion = -1;
	long long meshFacesVersion = -1;

	if (meshNodesStorageId != -1)
	{
		meshNodesVersion = getMeshNodes() != nullptr ? getMeshNodes()->getEntityStorageVersion() : getCurrentEntityVersion(meshNodesStorageId);
	}

	if (meshFacesStorageId != -1)
	{
		meshFacesVersion = getMeshFaces() != nullptr ? getMeshFaces()->getEntityStorageVersion() : getCurrentEntityVersion(meshFacesStorageId);
	}

	storage.append(
		bsoncxx::builder::basic::kvp("MeshNodesID", meshNodesStorageId),
		bsoncxx::builder::basic::kvp("MeshNodesVersion", meshNodesVersion),
		bsoncxx::builder::basic::kvp("MeshFacesID", meshFacesStorageId),
		bsoncxx::builder::basic::kvp("MeshFacesVersion", meshFacesVersion),
		bsoncxx::builder::basic::kvp("GmshDataStorageID", gmshDataStorageId)
	);
}

void EntityMeshTetData::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the coordinates

	// First reset the current mesh data
	if (meshNodes != nullptr)
	{
		delete meshNodes;
		meshNodes = nullptr;
	}

	if (meshFaces != nullptr)
	{
		delete meshFaces;
		meshFaces = nullptr;
	}

	// Now load the data from the storage

	meshNodesStorageId = doc_view["MeshNodesID"].get_int64();
	meshFacesStorageId = doc_view["MeshFacesID"].get_int64();

	gmshDataStorageId = -1;
	try
	{
		gmshDataStorageId = doc_view["GmshDataStorageID"].get_int64();
	}
	catch (std::exception)
	{
	}

	resetModified();
}

void EntityMeshTetData::releaseMeshData(void)
{
	releaseMeshFaces();
	releaseMeshNodes();
}

void EntityMeshTetData::addVisualizationItem(void)
{
	if (getObserver() == nullptr) return;

	double edgeColorRGB[3] = { 1.0, 1.0, 1.0 };

	bool displayTetEdges = false;
	EntityPropertiesBoolean *showVolumeMesh = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Show volume mesh"));
	if (showVolumeMesh != nullptr)
	{
		displayTetEdges = showVolumeMesh->getValue();
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshNodeFromFacetDatabase, doc.GetAllocator()), doc.GetAllocator());
	
	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_EDGE_COLOR_R, edgeColorRGB[0], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EDGE_COLOR_G, edgeColorRGB[1], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EDGE_COLOR_B, edgeColorRGB[2], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(DataBase::instance().getCollectionName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, this->getCurrentEntityVersion(this->getEntityID()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES, displayTetEdges, doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

void EntityMeshTetData::addVisualizationNodes(void)
{
	addVisualizationItem();

	EntityContainer::addVisualizationNodes();
}

void EntityMeshTetData::removeChild(EntityBase *child)
{
	if (child == meshNodes)
	{
		meshNodes = nullptr;
	}
	else if (child == meshFaces)
	{
		meshFaces = nullptr;
	}
	else
	{
		EntityContainer::removeChild(child);
	}
}

bool EntityMeshTetData::updateFromProperties(void)
{
	EntityPropertiesBoolean *showVolumeMesh = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Show volume mesh"));
	if (showVolumeMesh != nullptr)
	{
		if (showVolumeMesh->needsUpdate())
		{
			// Since there is a change now, we need to set the modified flag
			setModified();

			bool displayTetEdges = showVolumeMesh->getValue();

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_TetMeshNodeTetEdges, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_UI_UID, this->getEntityID(), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES, displayTetEdges, doc.GetAllocator());

			getObserver()->sendMessageToViewer(doc);
		}
	}
	
	getProperties().forceResetUpdateForAllProperties();

	return false; // No property grid update necessary
}