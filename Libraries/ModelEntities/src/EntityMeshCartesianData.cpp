// @otlicense
// File: EntityMeshCartesianData.cpp
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
#include "EntityCartesianVector.h"
#include "EntityMeshCartesianData.h"
#include "EntityMeshCartesianNodes.h"
#include "EntityMeshCartesianFaceList.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshCartesianData> registrar("EntityMeshCartesianData");

EntityMeshCartesianData::EntityMeshCartesianData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityContainer(ID, parent, obs, ms),
	meshFaces(nullptr),
	meshFacesStorageId(-1),
	meshFacesStorageVersion(-1),
	meshNodes(nullptr),
	meshNodesStorageId(-1),
	meshNodesStorageVersion(-1),
	matrixDs(nullptr),
	matrixDualDs(nullptr),
	matrixDa(nullptr),
	matrixDualDa(nullptr),
	matrixDeps(nullptr),
	matrixDmu(nullptr),
	matrixDsigma(nullptr),
	matrixDsStorageId(-1),
	matrixDsStorageVersion(-1),
	matrixDualDsStorageId(-1),
	matrixDualDsStorageVersion(-1),
	matrixDaStorageId(-1),
	matrixDaStorageVersion(-1),
	matrixDualDaStorageId(-1),
	matrixDualDaStorageVersion(-1),
	matrixDepsStorageId(-1),
	matrixDepsStorageVersion(-1),
	matrixDmuStorageId(-1),
	matrixDmuStorageVersion(-1),
	matrixDsigmaStorageId(-1),
	matrixDsigmaStorageVersion(-1)
{
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("Default/CartesianMeshDataVisible");
	treeItem.setHiddenIcon("Default/CartesianMeshDataHidden");
	this->setTreeItem(treeItem, true);
}

EntityMeshCartesianData::~EntityMeshCartesianData()
{
	if (meshFaces != nullptr) delete meshFaces;
	meshFaces = nullptr;

	if (meshNodes != nullptr) delete meshNodes;
	meshNodes = nullptr;

	if (matrixDs != nullptr) delete matrixDs;
	matrixDs = nullptr;	
	
	if (matrixDualDs != nullptr) delete matrixDualDs;
	matrixDualDs = nullptr;

	if (matrixDa != nullptr) delete matrixDa;
	matrixDa = nullptr;	

	if (matrixDualDa != nullptr) delete matrixDualDa;
	matrixDualDa = nullptr;	

	if (matrixDeps != nullptr) delete matrixDeps;
	matrixDeps = nullptr;	

	if (matrixDmu != nullptr) delete matrixDmu;
	matrixDmu = nullptr;	

	if (matrixDsigma != nullptr) delete matrixDsigma;
	matrixDsigma = nullptr;	
}

bool EntityMeshCartesianData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

EntityMeshCartesianFaceList *EntityMeshCartesianData::getMeshFaces(void)
{ 
	EnsureFacesLoaded();

	return meshFaces; 
}

EntityMeshCartesianNodes *EntityMeshCartesianData::getMeshNodes(void)
{ 
	EnsureNodesLoaded();

	return meshNodes; 
}

void EntityMeshCartesianData::EnsureFacesLoaded(void)
{
	if (meshFaces == nullptr)
	{
		if (meshFacesStorageId == -1)
		{
			meshFaces = new EntityMeshCartesianFaceList(0, this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshFaces = dynamic_cast<EntityMeshCartesianFaceList *>(readEntityFromEntityIDAndVersion(this, meshFacesStorageId, meshFacesStorageVersion, entityMap));
		}
	}
}

void EntityMeshCartesianData::storeMeshFaces(void)
{
	assert(meshFaces != nullptr);

	// Assign an entity id if needed
	if (meshFaces->getEntityID() == 0)
	{
		meshFaces->setEntityID(createEntityUID());
	}

	meshFaces->storeToDataBase();
	meshFacesStorageId = meshFaces->getEntityID();
	meshFacesStorageVersion = meshFaces->getEntityStorageVersion();
}

void EntityMeshCartesianData::releaseMeshFaces(void)
{
	if (meshFaces == nullptr) return;

	storeMeshFaces();

	delete meshFaces;
	meshFaces = nullptr;
}

void EntityMeshCartesianData::EnsureNodesLoaded(void)
{
	if (meshNodes == nullptr)
	{
		if (meshNodesStorageId == -1)
		{
			meshNodes = new EntityMeshCartesianNodes(0, this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshNodes = dynamic_cast<EntityMeshCartesianNodes *>(readEntityFromEntityIDAndVersion(this, meshNodesStorageId, meshNodesStorageVersion, entityMap));
		}
	}
}

void EntityMeshCartesianData::storeMeshNodes(void)
{
	assert(meshNodes != nullptr);

	// Assign an entity id if needed
	if (meshNodes->getEntityID() == 0)
	{
		meshNodes->setEntityID(createEntityUID());
	}

	meshNodes->storeToDataBase();
	meshNodesStorageId = meshNodes->getEntityID();
	meshNodesStorageVersion = meshNodes->getEntityStorageVersion();
}

void EntityMeshCartesianData::releaseMeshNodes(void)
{
	if (meshNodes == nullptr) return;

	storeMeshNodes();

	delete meshNodes;
	meshNodes = nullptr;
}

void EntityMeshCartesianData::storeToDataBase(void)
{
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

void EntityMeshCartesianData::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::addStorageData(storage);

	auto coordX = bsoncxx::builder::basic::array();
	auto coordY = bsoncxx::builder::basic::array();
	auto coordZ = bsoncxx::builder::basic::array();

	for (size_t i = 0; i < meshCoords[0].size(); i++)
	{
		coordX.append(meshCoords[0][i]);
	}

	for (size_t i = 0; i < meshCoords[1].size(); i++)
	{
		coordY.append(meshCoords[1][i]);
	}

	for (size_t i = 0; i < meshCoords[2].size(); i++)
	{
		coordZ.append(meshCoords[2][i]);
	}

	storage.append(bsoncxx::builder::basic::kvp("MeshCoordX", coordX));
	storage.append(bsoncxx::builder::basic::kvp("MeshCoordY", coordY));
	storage.append(bsoncxx::builder::basic::kvp("MeshCoordZ", coordZ));
	storage.append(bsoncxx::builder::basic::kvp("MeshFacesID", meshFacesStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MeshFacesVersion", meshFacesStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MeshNodesID", meshNodesStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MeshNodesVersion", meshNodesStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDsID", matrixDsStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDsVersion", matrixDsStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDualDsID", matrixDualDsStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDualDsVersion", matrixDualDsStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDaID", matrixDaStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDaVersion", matrixDaStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDualDaID", matrixDualDaStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDualDaVersion", matrixDualDaStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDepsID", matrixDepsStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDepsVersion", matrixDepsStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDmuID", matrixDmuStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDmuVersion", matrixDmuStorageVersion));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDsigmaID", matrixDsigmaStorageId));
	storage.append(bsoncxx::builder::basic::kvp("MatrixDsigmaVersion", matrixDsigmaStorageVersion));
}

void EntityMeshCartesianData::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	if (meshFaces != nullptr)
	{
		delete meshFaces;
		meshFaces = nullptr;
	}

	// Now load the data from the storage
		// Now we read the information about the coordinates
	auto coordX = doc_view["MeshCoordX"].get_array().value;
	auto coordY = doc_view["MeshCoordY"].get_array().value;
	auto coordZ = doc_view["MeshCoordZ"].get_array().value;

	size_t nX = std::distance(coordX.begin(), coordX.end());
	size_t nY = std::distance(coordY.begin(), coordY.end());
	size_t nZ = std::distance(coordZ.begin(), coordZ.end());

	auto cx = coordX.begin();
	auto cy = coordY.begin();
	auto cz = coordZ.begin();

	meshCoords[0].clear();
	meshCoords[1].clear();
	meshCoords[2].clear();

	meshCoords[0].resize(nX);
	meshCoords[1].resize(nY);
	meshCoords[2].resize(nZ);

	for (unsigned long index = 0; index < nX; index++)
	{
		meshCoords[0][index] = cx->get_double();
		cx++;
	}

	for (unsigned long index = 0; index < nY; index++)
	{
		meshCoords[1][index] = cy->get_double();
		cy++;
	}

	for (unsigned long index = 0; index < nZ; index++)
	{
		meshCoords[2][index] = cz->get_double();
		cz++;
	}

	meshFacesStorageId = -1;
	try
	{
		meshFacesStorageId = doc_view["MeshFacesID"].get_int64();
		meshFacesStorageVersion = doc_view["MeshFacesVersion"].get_int64();
	}
	catch (std::exception)
	{

	}

	meshNodesStorageId = -1;
	try
	{
		meshNodesStorageId = doc_view["MeshNodesID"].get_int64();
		meshNodesStorageVersion = doc_view["MeshNodesVersion"].get_int64();
	}
	catch (std::exception)
	{

	}

	matrixDsStorageId		    = getStorageId(doc_view, "MatrixDsID");
	matrixDsStorageVersion		= getStorageId(doc_view, "MatrixDsVersion");
	matrixDualDsStorageId	    = getStorageId(doc_view, "MatrixDualDsID");
	matrixDualDsStorageVersion	= getStorageId(doc_view, "MatrixDualDsVersion");
	matrixDaStorageId		    = getStorageId(doc_view, "MatrixDaID");
	matrixDaStorageVersion		= getStorageId(doc_view, "MatrixDaVersion");
	matrixDualDaStorageId	    = getStorageId(doc_view, "MatrixDualDaID");
	matrixDualDaStorageVersion	= getStorageId(doc_view, "MatrixDualDaVersion");
	matrixDepsStorageId		    = getStorageId(doc_view, "MatrixDepsID");
	matrixDepsStorageVersion	= getStorageId(doc_view, "MatrixDepsVersion");
	matrixDmuStorageId		    = getStorageId(doc_view, "MatrixDmuID");
	matrixDmuStorageVersion		= getStorageId(doc_view, "MatrixDmuVersion");
	matrixDsigmaStorageId	    = getStorageId(doc_view, "MatrixDsigmaID");
	matrixDsigmaStorageVersion	= getStorageId(doc_view, "MatrixDsigmaVersion");

	resetModified();
}

long long EntityMeshCartesianData::getStorageId(const bsoncxx::document::view &doc_view, const std::string dataName)
{
	long long storageId = -1;
	try
	{
		storageId = doc_view[dataName].get_int64();
	}
	catch (std::exception)
	{

	}

	return storageId;
}

bool EntityMeshCartesianData::updateFromProperties(void)
{
	EntityPropertiesBoolean *showMeshLines = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Show mesh lines"));
	if (showMeshLines != nullptr)
	{
		if (showMeshLines->needsUpdate())
		{
			// Since there is a change now, we need to set the modified flag
			setModified();

			bool showMesh = showMeshLines->getValue();

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_CartesianMeshNodeShowLines, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_UI_UID, this->getEntityID(), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MESH_ShowMeshLines, showMesh, doc.GetAllocator());

			getObserver()->sendMessageToViewer(doc);
		}
	}

	getProperties().forceResetUpdateForAllProperties();

	return false; // No property grid update necessary
}

void EntityMeshCartesianData::addVisualizationItem(bool isHidden)
{
	if (getObserver() == nullptr) return;

	bool showMesh = true;
	EntityPropertiesBoolean *showMeshLines = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Show mesh lines"));
	if (showMeshLines != nullptr)
	{
		showMesh = showMeshLines->getValue();
	}

	double edgeColorRGB[3] = { 1.0, 1.0, 1.0 };
	double meshLineColorRGB[3] = { 0.5, 0.5, 0.5 };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshNode, doc.GetAllocator()), doc.GetAllocator());
	
	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EDGE_COLOR_R, edgeColorRGB[0], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EDGE_COLOR_G, edgeColorRGB[1], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EDGE_COLOR_B, edgeColorRGB[2], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_R, meshLineColorRGB[0], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_G, meshLineColorRGB[1], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_B, meshLineColorRGB[2], doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_CartesianCoordX, ot::JsonArray(meshCoords[0], doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_CartesianCoordY, ot::JsonArray(meshCoords[1], doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_CartesianCoordZ, ot::JsonArray(meshCoords[2], doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_ShowMeshLines, showMesh, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(DataBase::instance().getCollectionName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, meshFacesStorageId == -1 ? 0 : (unsigned long long) meshFacesStorageId, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, meshFacesStorageVersion == -1 ? 0 : (unsigned long long) meshFacesStorageVersion, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_NODE_ID, meshNodesStorageId == -1 ? 0 : (unsigned long long) meshNodesStorageId, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESH_NODE_VERSION, meshNodesStorageVersion == -1 ? 0 : (unsigned long long) meshNodesStorageVersion, doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

void EntityMeshCartesianData::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityContainer::addVisualizationNodes();
}

void EntityMeshCartesianData::removeChild(EntityBase *child)
{
	if (child == meshFaces)
	{
		meshFaces = nullptr;
	}
	else if (child == meshNodes)
	{
		meshNodes = nullptr;
	}
	else if (child == matrixDs)
	{
		matrixDs = nullptr;
	}
	else if (child == matrixDualDs)
	{
		matrixDualDs = nullptr;
	}	
	else if (child == matrixDa)
	{
		matrixDa = nullptr;
	}	
	else if (child == matrixDualDa)
	{
		matrixDualDa = nullptr;
	}	
	else if (child == matrixDeps)
	{
		matrixDeps = nullptr;
	}	
	else if (child == matrixDmu)
	{
		matrixDmu = nullptr;
	}	
	else if (child == matrixDsigma)
	{
		matrixDsigma = nullptr;
	}	
	else
	{
		EntityContainer::removeChild(child);
	}
}

double EntityMeshCartesianData::getSmallestStepWidth(void)
{
	double minStepWidth = DBL_MAX;

	for (int direction = 0; direction < 3; direction++)
	{
		for (int step = 0; step < meshCoords[direction].size() - 1; step++)
		{
			double stepWidth = meshCoords[direction][step + 1] - meshCoords[direction][step];
			minStepWidth = std::min(minStepWidth, stepWidth);
		}
	}

	return minStepWidth;
}

double EntityMeshCartesianData::getLargestStepWidth(void)
{
	double maxStepWidth = 0.0;

	for (int direction = 0; direction < 3; direction++)
	{
		for (int step = 0; step < meshCoords[direction].size() - 1; step++)
		{
			double stepWidth = meshCoords[direction][step + 1] - meshCoords[direction][step];
			maxStepWidth = std::max(maxStepWidth, stepWidth);
		}
	}

	return maxStepWidth;
}

double EntityMeshCartesianData::getStepRatio(void)
{
	double ratio = getLargestStepWidth() / getSmallestStepWidth();
	return ratio;
}

size_t EntityMeshCartesianData::findLowerIndex(int direction, double value)
{
	assert(direction >= 0 && direction < 3);

	// Perform an interval search
	size_t start = 0;
	size_t end = meshCoords[direction].size() - 1;

	//assert(meshCoords[direction][start] <= value && meshCoords[direction][end] >= value);  // Here we would need to introduce a tolerance

	while (end - start > 1)
	{
		size_t center = (start + end) / 2;
		if (value == meshCoords[direction][center])
		{
			return center;
		}
		else if (value < meshCoords[direction][center])
		{
			end = center;
		}
		else if (value > meshCoords[direction][center])
		{
			start = center;
		}
	}

	return start;
}

size_t EntityMeshCartesianData::findUpperIndex(int direction, double value)
{
	assert(direction >= 0 && direction < 3);

	// Perform an interval search
	size_t start = 0;
	size_t end = meshCoords[direction].size() - 1;

	if (value <= meshCoords[direction][start]) return start;
	if (value >= meshCoords[direction][end]) return end;

	//assert(meshCoords[direction][start] <= value && meshCoords[direction][end] >= value);  // Here we would need to introduce a tolerance

	while (end - start > 1)
	{
		size_t center = (start + end) / 2;
		if (value == meshCoords[direction][center])
		{
			return center;
		}
		else if (value < meshCoords[direction][center])
		{
			end = center;
		}
		else if (value > meshCoords[direction][center])
		{
			start = center;
		}
	}

	return end;
}

void EntityMeshCartesianData::setDsMatrix(EntityCartesianVector *matrix)
{
	assert(matrixDs == nullptr);

	if (matrix == nullptr) return;

	matrix->storeToDataBase();
	
	matrixDs = matrix;
	matrixDsStorageId = matrix->getEntityID();
	matrixDsStorageVersion = matrix->getEntityStorageVersion();

	matrix->setParent(this);
}

void EntityMeshCartesianData::setDualDsMatrix(EntityCartesianVector *matrix)
{
	assert(matrixDualDs == nullptr);

	if (matrix == nullptr) return;

	matrix->storeToDataBase();

	matrixDualDs = matrix;
	matrixDualDsStorageId = matrix->getEntityID();
	matrixDualDsStorageVersion = matrix->getEntityStorageVersion();

	matrix->setParent(this);
}

void EntityMeshCartesianData::setDaMatrix(EntityCartesianVector *matrix)
{
	assert(matrixDa == nullptr);

	if (matrix == nullptr) return;

	matrix->storeToDataBase();

	matrixDa = matrix;
	matrixDaStorageId = matrix->getEntityID();
	matrixDaStorageVersion = matrix->getEntityStorageVersion();

	matrix->setParent(this);
}

void EntityMeshCartesianData::setDualDaMatrix(EntityCartesianVector *matrix)
{
	assert(matrixDualDa == nullptr);

	if (matrix == nullptr) return;

	matrix->storeToDataBase();

	matrixDualDs = matrix;
	matrixDualDaStorageId = matrix->getEntityID();
	matrixDualDaStorageVersion = matrix->getEntityStorageVersion();

	matrix->setParent(this);
}

void EntityMeshCartesianData::setDepsMatrix(EntityCartesianVector *matrix)
{
	assert(matrixDeps == nullptr);

	if (matrix == nullptr) return;

	matrix->storeToDataBase();

	matrixDeps = matrix;
	matrixDepsStorageId = matrix->getEntityID();
	matrixDepsStorageVersion = matrix->getEntityStorageVersion();

	matrix->setParent(this);
}

void EntityMeshCartesianData::setDmuMatrix(EntityCartesianVector *matrix)
{
	assert(matrixDmu == nullptr);

	if (matrix == nullptr) return;

	matrix->storeToDataBase();

	matrixDmu = matrix;
	matrixDmuStorageId = matrix->getEntityID();
	matrixDmuStorageVersion = matrix->getEntityStorageVersion();

	matrix->setParent(this);
}

void EntityMeshCartesianData::setDsigmaMatrix(EntityCartesianVector *matrix)
{
	assert(matrixDsigma == nullptr);

	if (matrix == nullptr) return;

	matrix->storeToDataBase();

	matrixDsigma = matrix;
	matrixDsigmaStorageId = matrix->getEntityID();
	matrixDsigmaStorageVersion = matrix->getEntityStorageVersion();

	matrix->setParent(this);
}

EntityCartesianVector* EntityMeshCartesianData::getDsMatrix(void)
{
	EnsureMatrixLoaded(matrixDs, matrixDsStorageId, matrixDsStorageVersion);

	return matrixDs;
}

EntityCartesianVector* EntityMeshCartesianData::getDualDsMatrix(void)
{
	EnsureMatrixLoaded(matrixDualDs, matrixDualDsStorageId, matrixDualDsStorageVersion);

	return matrixDualDs;
}

EntityCartesianVector* EntityMeshCartesianData::getDaMatrix(void)
{
	EnsureMatrixLoaded(matrixDa, matrixDaStorageId, matrixDaStorageVersion);

	return matrixDa;
}

EntityCartesianVector* EntityMeshCartesianData::getDualDaMatrix(void)
{
	EnsureMatrixLoaded(matrixDualDa, matrixDualDaStorageId, matrixDualDaStorageVersion);

	return matrixDualDa;
}

EntityCartesianVector* EntityMeshCartesianData::getDepsMatrix(void)
{
	EnsureMatrixLoaded(matrixDeps, matrixDepsStorageId, matrixDepsStorageVersion);

	return matrixDeps;
}

EntityCartesianVector* EntityMeshCartesianData::getDmuMatrix(void)
{
	EnsureMatrixLoaded(matrixDmu, matrixDmuStorageId, matrixDmuStorageVersion);

	return matrixDmu;
}

EntityCartesianVector* EntityMeshCartesianData::getDsigmaMatrix(void)
{
	EnsureMatrixLoaded(matrixDsigma, matrixDsigmaStorageId, matrixDsigmaStorageVersion);

	return matrixDsigma;
}

void EntityMeshCartesianData::EnsureMatrixLoaded(EntityCartesianVector *&matrix, long long storageId, long long storageVersion)
{
	if (matrix == nullptr)
	{
		if (storageId == -1)
		{
			matrix = new EntityCartesianVector(0, this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			// The matrices are always stored with version 1 (a new entity ID is generated all the time)
			matrix = dynamic_cast<EntityCartesianVector *>(readEntityFromEntityIDAndVersion(this, storageId, storageVersion, entityMap));
		}
	}
}

bool EntityMeshCartesianData::prepareMatrixApplication(std::vector<double> &vector, bool keepMatrixInMemory, EntityCartesianVector *matrix, EntityCartesianVector *&loadedMatrix)
{
	if (matrix == nullptr) return false;  // Matrix does not exist

	size_t matrixDim = matrix->getUncompressedLength();
	if (matrixDim != 3 * getNumberCells()) return false;  // Matrix does not have the right dimension

	if (vector.empty())
	{
		vector.resize(matrixDim, 1.0);
	}

	if (vector.size() != matrixDim) return false; // Vector has the wrong dimension

	// Now we have a vector of the right length and the matrix.
	// First we apply the matrix to the vector

	matrix->multiply(vector);

	// If the matrix should not be kept, remove it now
	if (!keepMatrixInMemory)
	{
		delete loadedMatrix;
		loadedMatrix = nullptr;
	}

	return true;
}

bool EntityMeshCartesianData::applyDsMatrix(std::vector<double> &vector, bool keepMatrixInMemory)
{
	if (!prepareMatrixApplication(vector, keepMatrixInMemory, getDsMatrix(), matrixDs))
	{
		return false;
	}

	// Now we need to apply the mesh sizes dx, dy, dz to the matrix

	size_t my = getNumberLinesX();
	size_t mz = getNumberLinesX() * getNumberLinesY();

	// Step width in x-direction
	for (size_t ix = 0; ix < getNumberLinesX()-1; ix++)
	{
		double dx = getMeshLinesX()[ix + 1] - getMeshLinesX()[ix];

		for (size_t iy = 0; iy < getNumberLinesY(); iy++)
		{
			for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index] *= dx;
			}
		}
	}

	// Step width in y-direction
	for (size_t iy = 0; iy < getNumberLinesY()-1; iy++)
	{
		double dy = getMeshLinesY()[iy + 1] - getMeshLinesY()[iy];

		for (size_t ix = 0; ix < getNumberLinesX(); ix++)
		{
			for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + getNumberCells()] *= dy;
			}
		}
	}

	// Step width in z-direction
	for (size_t iz = 0; iz < getNumberLinesZ()-1; iz++)
	{
		double dz = getMeshLinesZ()[iz + 1] - getMeshLinesZ()[iz];

		for (size_t ix = 0; ix < getNumberLinesX(); ix++)
		{
			for (size_t iy = 0; iy < getNumberLinesY(); iy++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + 2 * getNumberCells()] *= dz;
			}
		}
	}

	return true;
}

bool EntityMeshCartesianData::applyDualDsMatrix(std::vector<double> &vector, bool keepMatrixInMemory)
{
	if (!prepareMatrixApplication(vector, keepMatrixInMemory, getDualDsMatrix(), matrixDualDs))
	{
		return false;
	}

	// Now we need to apply the dual mesh sizes dx, dy, dz to the matrix

	size_t my = getNumberLinesX();
	size_t mz = getNumberLinesX() * getNumberLinesY();

	// Dual step width in x-direction
	for (size_t ix = 0; ix < getNumberLinesX(); ix++)
	{
		double dx = 0.0;

		if (ix > 0                  ) dx += 0.5 * (getMeshLinesX()[ix  ] - getMeshLinesX()[ix-1]);
		if (ix < getNumberLinesX()-1) dx += 0.5 * (getMeshLinesX()[ix+1] - getMeshLinesX()[ix  ]);

		for (size_t iy = 0; iy < getNumberLinesY(); iy++)
		{
			for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index] *= dx;
			}
		}
	}

	// Dual step width in y-direction
	for (size_t iy = 0; iy < getNumberLinesY(); iy++)
	{
		double dy = 0.0;

		if (iy > 0                  ) dy += 0.5 * (getMeshLinesY()[iy  ] - getMeshLinesY()[iy-1]);
		if (iy < getNumberLinesY()-1) dy += 0.5 * (getMeshLinesY()[iy+1] - getMeshLinesY()[iy  ]);

		for (size_t ix = 0; ix < getNumberLinesX(); ix++)
		{
			for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + getNumberCells()] *= dy;
			}
		}
	}

	// Dual step width in z-direction
	for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
	{
		double dz = 0.0;

		if (iz > 0                  ) dz += 0.5 * (getMeshLinesZ()[iz  ] - getMeshLinesZ()[iz-1]);
		if (iz < getNumberLinesZ()-1) dz += 0.5 * (getMeshLinesZ()[iz+1] - getMeshLinesZ()[iz  ]);

		for (size_t ix = 0; ix < getNumberLinesX(); ix++)
		{
			for (size_t iy = 0; iy < getNumberLinesY(); iy++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + 2 * getNumberCells()] *= dz;
			}
		}
	}

	return true;
}

bool EntityMeshCartesianData::applyDaMatrix(std::vector<double> &vector, bool keepMatrixInMemory)
{
	if (!prepareMatrixApplication(vector, keepMatrixInMemory, getDaMatrix(), matrixDa))
	{
		return false;
	}

	// Now we need to apply the mesh areas dAx, dAy, dAz to the matrix

	size_t my = getNumberLinesX();
	size_t mz = getNumberLinesX() * getNumberLinesY();

	// Area in x-direction
	for (size_t iy = 0; iy < getNumberLinesY() - 1; iy++)
	{
		double dy = getMeshLinesY()[iy + 1] - getMeshLinesY()[iy];

		for (size_t iz = 0; iz < getNumberLinesZ() - 1; iz++)
		{
			double dz = getMeshLinesZ()[iz + 1] - getMeshLinesZ()[iz];

			for (size_t ix = 0; ix < getNumberLinesX(); ix++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index] *= dy * dz;
			}
		}
	}

	// Area in y-direction
	for (size_t ix = 0; ix < getNumberLinesX() - 1; ix++)
	{
		double dx = getMeshLinesX()[ix + 1] - getMeshLinesX()[ix];

		for (size_t iz = 0; iz < getNumberLinesZ() - 1; iz++)
		{
			double dz = getMeshLinesZ()[iz + 1] - getMeshLinesZ()[iz];

			for (size_t iy = 0; iy < getNumberLinesY(); iy++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + getNumberCells()] *= dx * dz;
			}
		}
	}

	// Area in z-direction
	for (size_t ix = 0; ix < getNumberLinesX() - 1; ix++)
	{
		double dx = getMeshLinesX()[ix + 1] - getMeshLinesX()[ix];

		for (size_t iy = 0; iy < getNumberLinesY() - 1; iy++)
		{
			double dy = getMeshLinesY()[iy + 1] - getMeshLinesY()[iy];

			for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + 2 * getNumberCells()] *= dx * dy;
			}
		}
	}

	return true;
}

bool EntityMeshCartesianData::applyDualDaMatrix(std::vector<double> &vector, bool keepMatrixInMemory)
{
	if (!prepareMatrixApplication(vector, keepMatrixInMemory, getDualDaMatrix(), matrixDualDa))
	{
		return false;
	}

	// Now we need to apply the dual mesh areas dAx, dAy, dAz to the matrix

	size_t my = getNumberLinesX();
	size_t mz = getNumberLinesX() * getNumberLinesY();

	// Dual area in x-direction
	for (size_t iy = 0; iy < getNumberLinesY(); iy++)
	{
		double dy = 0.0;

		if (iy > 0                  ) dy += 0.5 * (getMeshLinesY()[iy  ] - getMeshLinesY()[iy-1]);
		if (iy < getNumberLinesY()-1) dy += 0.5 * (getMeshLinesY()[iy+1] - getMeshLinesY()[iy  ]);

		for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
		{
			double dz = 0.0;

			if (iz > 0                  ) dz += 0.5 * (getMeshLinesZ()[iz  ] - getMeshLinesZ()[iz-1]);
			if (iz < getNumberLinesZ()-1) dz += 0.5 * (getMeshLinesZ()[iz+1] - getMeshLinesZ()[iz  ]);

			for (size_t ix = 0; ix < getNumberLinesX(); ix++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index] *= dy * dz;
			}
		}
	}

	// Dual area in y-direction
	for (size_t ix = 0; ix < getNumberLinesX(); ix++)
	{
		double dx = 0.0;

		if (ix > 0                  ) dx += 0.5 * (getMeshLinesX()[ix  ] - getMeshLinesX()[ix-1]);
		if (ix < getNumberLinesX()-1) dx += 0.5 * (getMeshLinesX()[ix+1] - getMeshLinesX()[ix  ]);

		for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
		{
			double dz = 0.0;

			if (iz > 0                  ) dz += 0.5 * (getMeshLinesZ()[iz  ] - getMeshLinesZ()[iz-1]);
			if (iz < getNumberLinesZ()-1) dz += 0.5 * (getMeshLinesZ()[iz+1] - getMeshLinesZ()[iz  ]);

			for (size_t iy = 0; iy < getNumberLinesY(); iy++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + getNumberCells()] *= dx * dz;
			}
		}
	}

	// Dual area in z-direction
	for (size_t ix = 0; ix < getNumberLinesX(); ix++)
	{
		double dx = 0.0;

		if (ix > 0                  ) dx += 0.5 * (getMeshLinesX()[ix  ] - getMeshLinesX()[ix-1]);
		if (ix < getNumberLinesX()-1) dx += 0.5 * (getMeshLinesX()[ix+1] - getMeshLinesX()[ix  ]);

		for (size_t iy = 0; iy < getNumberLinesY(); iy++)
		{
			double dy = 0.0;

			if (iy > 0                  ) dy += 0.5 * (getMeshLinesY()[iy  ] - getMeshLinesY()[iy-1]);
 			if (iy < getNumberLinesY()-1) dy += 0.5 * (getMeshLinesY()[iy+1] - getMeshLinesY()[iy  ]);

			for (size_t iz = 0; iz < getNumberLinesZ(); iz++)
			{
				size_t index = ix + iy * my + iz * mz;
				vector[index + 2 * getNumberCells()] *= dx * dy;
			}
		}
	}

	return true;
}

bool EntityMeshCartesianData::applyDepsMatrix(std::vector<double> &vector, bool keepMatrixInMemory)
{
	if (!prepareMatrixApplication(vector, keepMatrixInMemory, getDepsMatrix(), matrixDeps))
	{
		return false;
	}

	return true;
}

bool EntityMeshCartesianData::applyDmuMatrix(std::vector<double> &vector, bool keepMatrixInMemory)
{
	if (!prepareMatrixApplication(vector, keepMatrixInMemory, getDmuMatrix(), matrixDmu))
	{
		return false;
	}

	return true;
}

bool EntityMeshCartesianData::applyDsigmaMatrix(std::vector<double> &vector, bool keepMatrixInMemory)
{
	if (!prepareMatrixApplication(vector, keepMatrixInMemory, getDsigmaMatrix(), matrixDsigma))
	{
		return false;
	}

	return true;
}



