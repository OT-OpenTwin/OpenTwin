// @otlicense
// File: EntityMeshTet.cpp
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


#include "EntityMeshTet.h"
#include "EntityMeshTetData.h"
#include "EntityMeshTetItem.h"

#include "DataBase.h"
#include "OldTreeIcon.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshTet> registrar("EntityMeshTet");

EntityMeshTet::EntityMeshTet(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityMesh(ID, parent, obs, ms),
	meshData(nullptr),
	meshDataStorageId(-1),
	meshValid(false)
{
}

EntityMeshTet::~EntityMeshTet()
{
	deleteMeshData();
}

void EntityMeshTet::deleteMeshData(void)
{
	meshData = nullptr;
	meshDataStorageId = -1;
}

bool EntityMeshTet::updateFromProperties(void)
{
	return false; // No property grid update necessary
}

void EntityMeshTet::createProperties(const std::string materialsFolder, ot::UID materialsFolderID)
{
	// Background properties
	EntityPropertiesSelection::createProperty("Background", "Bounding sphere mode", {"Relative", "Absolute"}, "Relative", "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty(   "Background", "Bounding sphere radius",							     0.0, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty(   "Background", "Steps on bounding sphere circumference",				20.0, "Tetrahedral Meshing", getProperties());
	EntityPropertiesEntityList::createProperty("Background", "Bounding sphere material", materialsFolder, materialsFolderID, "", -1, "Tetrahedral Meshing", getProperties());

	// Base step width
	EntityPropertiesDouble::createProperty("Base step width", "Maximum edge length",			1e22, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty("Base step width", "Number of steps along diagonal", 10.0, "Tetrahedral Meshing", getProperties());
	
	// Curvature refinement
	EntityPropertiesBoolean::createProperty("Curvature refinement", "Curvature refinement",					true, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty( "Curvature refinement", "Maximum deviation of mesh from shape",  0.0, "Tetrahedral Meshing", getProperties());
	EntityPropertiesInteger::createProperty("Curvature refinement", "Number of steps per circle",			   6, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty( "Curvature refinement", "Minimum curvature refinement radius",   0.0, "Tetrahedral Meshing", getProperties());

	// Additional refinements
	EntityPropertiesString::createProperty("Additional refinement", "List of refinements",					 "", "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty("Additional refinement", "Radius for point refinements",			0.0, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty("Additional refinement", "Maximum edge length for refinements",  0.0, "Tetrahedral Meshing", getProperties());

	// Model creation properties
	EntityPropertiesBoolean::createProperty("Model creation", "Check for missing materials", true, "Tetrahedral Meshing", getProperties());
	EntityPropertiesBoolean::createProperty("Model creation", "Use mesh priorities", true, "Tetrahedral Meshing", getProperties());
	EntityPropertiesBoolean::createProperty("Model creation", "Merge shapes", true, "Tetrahedral Meshing", getProperties());
	EntityPropertiesDouble::createProperty("Model creation", "Geometric tolerance", 0.0, "Tetrahedral Meshing", getProperties());

	// Algorithm properties
	std::list<std::string> order = { "Low (1)", "Medium (2)", "High (3)" };
	std::list<std::string> algorithms = { "MeshAdapt (most robust)", "Automatic", "Delauney (fastest)","Frontal-Delauney (high quality)" };
	std::list<std::string> mesh3d     = { "Delauney", "HXT" };
	std::list<std::string> selfintersections = { "None", "Check Self-intersections", "Fix Self-intersections" };

	EntityPropertiesSelection::createProperty("Algorithm", "Element order", order, "Low (1)", "Tetrahedral Meshing", getProperties());
	EntityPropertiesSelection::createProperty("Algorithm", "2D mesh algorithm", algorithms, "Automatic", "Tetrahedral Meshing", getProperties());
	EntityPropertiesSelection::createProperty("Algorithm", "3D mesh algorithm", mesh3d,		"Delauney",		 "Tetrahedral Meshing", getProperties());
	EntityPropertiesBoolean::createProperty("Algorithm", "Optimization", false, "Tetrahedral Meshing", getProperties());
	EntityPropertiesSelection::createProperty("Algorithm", "Proximity meshing", selfintersections, "None", "Tetrahedral Meshing", getProperties());
	EntityPropertiesBoolean::createProperty("Algorithm", "Distance based refinement", true, "Tetrahedral Meshing", getProperties());
	EntityPropertiesBoolean::createProperty("Algorithm", "Verbose", false, "Tetrahedral Meshing", getProperties());

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityMeshTet::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesBoolean *curvatureRefinment = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Curvature refinement"));

	EntityPropertiesInteger *numStepsPerCircle = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Number of steps per circle"));
	EntityPropertiesDouble *minCurvRefinementRadius = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Minimum curvature refinement radius"));
	EntityPropertiesDouble *maximumDeviation = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Maximum deviation of mesh from shape"));

	bool showNumStepsPerCircle = true;
	bool showMinCurvRefinementRadius = true;
	bool showMaximumDeviation = true;

	if (curvatureRefinment != nullptr)
	{
		if (!curvatureRefinment->getValue())
		{
			// We do not use local curvature refinement 
			showNumStepsPerCircle = false;
			showMinCurvRefinementRadius = false;
			showMaximumDeviation = false;
		}
	}

	if (numStepsPerCircle != nullptr)
	{
		if (numStepsPerCircle->getVisible() != showNumStepsPerCircle)
		{
			numStepsPerCircle->setVisible(showNumStepsPerCircle);
			updatePropertiesGrid = true;
		}
	}

	if (minCurvRefinementRadius != nullptr)
	{
		if (minCurvRefinementRadius->getVisible() != showMinCurvRefinementRadius)
		{
			minCurvRefinementRadius->setVisible(showMinCurvRefinementRadius);
			updatePropertiesGrid = true;
		}
	}

	if (maximumDeviation != nullptr)
	{
		if (maximumDeviation->getVisible() != showMaximumDeviation)
		{
			maximumDeviation->setVisible(showMaximumDeviation);
			updatePropertiesGrid = true;
		}
	}

	if (numStepsPerCircle != nullptr) numStepsPerCircle->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (minCurvRefinementRadius != nullptr) minCurvRefinementRadius->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.
	if (maximumDeviation != nullptr) maximumDeviation->resetNeedsUpdate(); // This will be used for meshing only, no action necessary here.

	return updatePropertiesGrid;
}


EntityMeshTetFace *EntityMeshTet::getFace(int faceId)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	return meshData->getFace(faceId);
}

void EntityMeshTet::setFace(int faceId, EntityMeshTetFace *face)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	meshData->setFace(faceId, face);
}

EntityMeshTetData *EntityMeshTet::getMeshData(void)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	return meshData;
}

void EntityMeshTet::getNodeCoords(size_t node, double coord[3])
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	coord[0] = meshData->getNodeCoordX(node);
	coord[1] = meshData->getNodeCoordY(node);
	coord[2] = meshData->getNodeCoordZ(node);
}

double EntityMeshTet::getNodeCoordX(size_t node)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	return meshData->getNodeCoordX(node);
}

double EntityMeshTet::getNodeCoordY(size_t node)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	return meshData->getNodeCoordY(node);
}

double EntityMeshTet::getNodeCoordZ(size_t node)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	return meshData->getNodeCoordZ(node);
}

void EntityMeshTet::setNodeCoordX(size_t node, double x)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	meshData->setNodeCoordX(node, x);
}

void EntityMeshTet::setNodeCoordY(size_t node, double y)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	meshData->setNodeCoordY(node, y);
}

void EntityMeshTet::setNodeCoordZ(size_t node, double z)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	meshData->setNodeCoordZ(node, z);
}

void EntityMeshTet::setNumberOfNodes(size_t n)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	meshData->setNumberOfNodes(n);
}

void EntityMeshTet::EnsureMeshDataLoaded(void)
{
	if (meshData == nullptr)
	{
		if (meshDataStorageId == -1)
		{
			meshData = new EntityMeshTetData(0, nullptr, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshData = dynamic_cast<EntityMeshTetData *>(readEntityFromEntityID(this, meshDataStorageId, entityMap));
		}
	}
}

void EntityMeshTet::storeMeshData(void)
{
	if (meshData == nullptr) return;
	assert(meshData != nullptr);

	meshData->storeToDataBase();
	meshDataStorageId = meshData->getEntityID();
}

void EntityMeshTet::releaseMeshData(void)
{
	if (meshData == nullptr) return;
	if (meshData->getEntityID() == 0) return; // This mesh entity has not been save (e.g. the mesh is completely empty)

	storeMeshData();

	meshData->releaseMeshData();
}

void EntityMeshTet::storeToDataBase(void)
{
	EntityContainer::storeToDataBase();
}

void EntityMeshTet::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::addStorageData(storage);

	// Now we store the particular information about the current object

	storage.append(
		bsoncxx::builder::basic::kvp("MeshValid", meshValid),
		bsoncxx::builder::basic::kvp("MeshData", meshDataStorageId)
	);
}

void EntityMeshTet::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the mesh, its nodes and faces
	
	// First reset the current mesh data
	if (meshData != nullptr)
	{
		delete meshData;
		meshData = nullptr;
	}

	// Now load the data from the storage

	meshValid = doc_view["MeshValid"].get_bool();
	meshDataStorageId = doc_view["MeshData"].get_int64();

	// The child items have already been loaded now. We need to ensure that they have a valid pointer to this particular mesh object
	recursivelySetMesh(getChildrenList());

	resetModified();
}

void EntityMeshTet::recursivelySetMesh(const std::list<EntityBase *> &childList)
{
	for (auto child : childList)
	{
		if (dynamic_cast<EntityContainer *>(child) != nullptr)
		{
			// Recursively set the children of the container
			recursivelySetMesh(dynamic_cast<EntityContainer *>(child)->getChildrenList());
		}
		else if (dynamic_cast<EntityMeshTetItem *>(child) != nullptr)
		{
			dynamic_cast<EntityMeshTetItem *>(child)->setMesh(this);
		}
	}
}

void EntityMeshTet::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		OldTreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "TetrahedralMeshVisible";
		treeIcons.hiddenIcon  = "TetrahedralMeshHidden";

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}

	auto childList = getChildrenList();
	for (auto child : childList)
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityMeshTet::removeChild(EntityBase *child)
{
	if (child == meshData)
	{
		meshData = nullptr;
	}

	EntityContainer::removeChild(child);
}
