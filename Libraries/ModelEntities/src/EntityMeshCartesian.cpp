// @otlicense
// File: EntityMeshCartesian.cpp
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


#include "EntityMeshCartesian.h"
#include "EntityMeshCartesianData.h"

#include "DataBase.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshCartesian> registrar("EntityMeshCartesian");

EntityMeshCartesian::EntityMeshCartesian(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityMesh(ID, parent, obs, ms),
	meshData(nullptr),
	meshDataStorageId(-1),
	meshValid(false)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/CartesianMeshVisible");
	treeItem.setHiddenIcon("Default/CartesianMeshHidden");
	this->setDefaultTreeItem(treeItem);
}

EntityMeshCartesian::~EntityMeshCartesian()
{
	deleteMeshData();
}

void EntityMeshCartesian::deleteMeshData(void)
{
	meshData = nullptr;
	meshDataStorageId = -1;
}

bool EntityMeshCartesian::updateFromProperties(void)
{
	getProperties().forceResetUpdateForAllProperties();

	return false; // No property grid update necessary
}

void EntityMeshCartesian::createProperties(const std::string materialsFolder, ot::UID materialsFolderID)
{
	// Background properties
	EntityPropertiesSelection::createProperty( "Background", "Background mode", {"Field free", "Extend relative", "Extend absolute"}, "Field free", "Cartesian Meshing", getProperties());
	EntityPropertiesBoolean::createProperty(   "Background", "Extend in all directions", true, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance (abs)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at xmin (abs)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at xmax (abs)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at ymin (abs)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at ymax (abs)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at zmin (abs)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at zmax (abs)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance (rel)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at xmin (rel)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at xmax (rel)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at ymin (rel)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at ymax (rel)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at zmin (rel)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty(    "Background", "Boundary distance at zmax (rel)", 0.0, "Cartesian Meshing", getProperties());
	EntityPropertiesEntityList::createProperty("Background", "Background material", materialsFolder, materialsFolderID, "", -1, "Cartesian Meshing", getProperties());

	// Base step width
	EntityPropertiesDouble::createProperty("Base step width", "Maximum edge length",			1e22, "Cartesian Meshing", getProperties());
	EntityPropertiesDouble::createProperty("Base step width", "Number of steps along diagonal", 10.0, "Cartesian Meshing", getProperties());

	// Algorithm
	EntityPropertiesBoolean::createProperty("Algorithm", "Conformal meshing", false, "Cartesian Meshing", getProperties());
	EntityPropertiesBoolean::createProperty("Algorithm", "Visualize matrices", false, "Cartesian Meshing", getProperties());

	// Curvature refinement
	//EntityPropertiesBoolean::createProperty("Curvature refinement", "Curvature refinement",					true, "Cartesian Meshing", getProperties());
	//EntityPropertiesDouble::createProperty( "Curvature refinement", "Maximum deviation of mesh from shape",  0.0, "Cartesian Meshing", getProperties());
	//EntityPropertiesInteger::createProperty("Curvature refinement", "Number of steps per circle",			   6, "Cartesian Meshing", getProperties());
	//EntityPropertiesDouble::createProperty( "Curvature refinement", "Minimum curvature refinement radius",   0.0, "Cartesian Meshing", getProperties());

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityMeshCartesian::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesSelection *backgroundMode       = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Background mode"));
	EntityPropertiesBoolean *extendFlag             = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Extend in all directions"));
	EntityPropertiesEntityList *backgroundMaterial  = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Background material"));
	EntityPropertiesDouble *boundaryDistanceAllAbs  = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance (abs)"));
	EntityPropertiesDouble *boundaryDistanceAllRel  = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance (rel)"));
	EntityPropertiesDouble *boundaryDistanceAbsXmin = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at xmin (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsXmax = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at xmax (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsYmin = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at ymin (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsYmax = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at ymax (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsZmin = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at zmin (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsZmax = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at zmax (abs)"));
	EntityPropertiesDouble *boundaryDistanceRelXmin = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at xmin (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelXmax = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at xmax (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelYmin = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at ymin (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelYmax = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at ymax (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelZmin = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at zmin (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelZmax = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary distance at zmax (rel)"));

	if (backgroundMode != nullptr && extendFlag != nullptr && backgroundMaterial != nullptr && boundaryDistanceAllAbs != nullptr && boundaryDistanceAllRel != nullptr
		&& boundaryDistanceAbsXmin != nullptr && boundaryDistanceAbsXmax != nullptr && boundaryDistanceAbsYmin != nullptr && boundaryDistanceAbsYmax != nullptr 
		&& boundaryDistanceAbsZmin != nullptr && boundaryDistanceAbsZmax != nullptr  
		&& boundaryDistanceRelXmin != nullptr && boundaryDistanceRelXmax != nullptr && boundaryDistanceRelYmin != nullptr && boundaryDistanceRelYmax != nullptr 
		&& boundaryDistanceRelZmin != nullptr && boundaryDistanceRelZmax != nullptr  )
	{
		bool showExtendFlag						= true;
		bool showBoundaryDistanceAllAbs			= true;
		bool showBoundaryDistanceDirectionAbs	= true;
		bool showBoundaryDistanceAllRel			= true;
		bool showBoundaryDistanceDirectionRel	= true;
		bool showBackgroundMaterial				= true;

		if (backgroundMode->getValue() == "Field free")
		{
			showExtendFlag                   = false;
			showBackgroundMaterial           = false;
			showBoundaryDistanceAllAbs       = false;
			showBoundaryDistanceDirectionAbs = false;
			showBoundaryDistanceAllRel       = false;
			showBoundaryDistanceDirectionRel = false;
		}
		else if (backgroundMode->getValue() == "Extend relative")
		{ 
			showExtendFlag                   = true;
			showBackgroundMaterial           = true;
			showBoundaryDistanceAllAbs       = false;
			showBoundaryDistanceDirectionAbs = false;
			showBoundaryDistanceAllRel       = extendFlag->getValue();
			showBoundaryDistanceDirectionRel = !showBoundaryDistanceAllRel;
		}
		else if (backgroundMode->getValue() == "Extend absolute")
		{ 
			showExtendFlag                   = true;
			showBackgroundMaterial           = true;
			showBoundaryDistanceAllAbs       = extendFlag->getValue();
			showBoundaryDistanceDirectionAbs = !showBoundaryDistanceAllAbs;
			showBoundaryDistanceAllRel       = false;
			showBoundaryDistanceDirectionRel = false;
		}
		else
		{
			assert(0); // Unknown background mode
		}

		if (extendFlag->getVisible() != showExtendFlag)
		{
			extendFlag->setVisible(showExtendFlag);
			updatePropertiesGrid = true;
		}

		if (backgroundMaterial->getVisible() != showBackgroundMaterial)
		{
			backgroundMaterial->setVisible(showBackgroundMaterial);
			updatePropertiesGrid = true;
		}

		if (boundaryDistanceAllAbs->getVisible() != showBoundaryDistanceAllAbs)
		{
			boundaryDistanceAllAbs->setVisible(showBoundaryDistanceAllAbs);
			updatePropertiesGrid = true;
		}

		if (boundaryDistanceAllRel->getVisible() != showBoundaryDistanceAllRel)
		{
			boundaryDistanceAllRel->setVisible(showBoundaryDistanceAllRel);
			updatePropertiesGrid = true;
		}

		if (boundaryDistanceAbsXmin->getVisible() != showBoundaryDistanceDirectionAbs)
		{
			boundaryDistanceAbsXmin->setVisible(showBoundaryDistanceDirectionAbs);
			boundaryDistanceAbsXmax->setVisible(showBoundaryDistanceDirectionAbs);
			boundaryDistanceAbsYmin->setVisible(showBoundaryDistanceDirectionAbs);
			boundaryDistanceAbsYmax->setVisible(showBoundaryDistanceDirectionAbs);
			boundaryDistanceAbsZmin->setVisible(showBoundaryDistanceDirectionAbs);
			boundaryDistanceAbsZmax->setVisible(showBoundaryDistanceDirectionAbs);
			updatePropertiesGrid = true;
		}

		if (boundaryDistanceRelXmin->getVisible() != showBoundaryDistanceDirectionRel)
		{
			boundaryDistanceRelXmin->setVisible(showBoundaryDistanceDirectionRel);
			boundaryDistanceRelXmax->setVisible(showBoundaryDistanceDirectionRel);
			boundaryDistanceRelYmin->setVisible(showBoundaryDistanceDirectionRel);
			boundaryDistanceRelYmax->setVisible(showBoundaryDistanceDirectionRel);
			boundaryDistanceRelZmin->setVisible(showBoundaryDistanceDirectionRel);
			boundaryDistanceRelZmax->setVisible(showBoundaryDistanceDirectionRel);
			updatePropertiesGrid = true;
		}
	}

	/*
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
	*/

	return updatePropertiesGrid;
}

EntityMeshCartesianData *EntityMeshCartesian::getMeshData(void)
{
	EnsureMeshDataLoaded();
	assert(meshData != nullptr);

	return meshData;
}

void EntityMeshCartesian::setMeshDataID(long long id)
{
	if (meshData != nullptr)
	{
		delete meshData;
		meshData = nullptr;
	}

	meshDataStorageId = id;
	setModified();
}

void EntityMeshCartesian::EnsureMeshDataLoaded(void)
{
	if (meshData == nullptr)
	{
		if (meshDataStorageId == -1)
		{
			meshData = new EntityMeshCartesianData(0, nullptr, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshData = dynamic_cast<EntityMeshCartesianData *>(readEntityFromEntityID(this, meshDataStorageId, entityMap));
		}
	}
}

void EntityMeshCartesian::storeMeshData(void)
{
	if (meshData == nullptr) return;
	assert(meshData != nullptr);

	meshData->storeToDataBase();
	meshDataStorageId = meshData->getEntityID();
}

void EntityMeshCartesian::releaseMeshData(void)
{
	if (meshData == nullptr) return;
	if (meshData->getEntityID() == 0) return; // This mesh entity has not been save (e.g. the mesh is completely empty)

	storeMeshData();
}

void EntityMeshCartesian::storeToDataBase(void)
{
	EntityContainer::storeToDataBase();
}

void EntityMeshCartesian::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::addStorageData(storage);

	// Now we store the particular information about the current object

	storage.append(
		bsoncxx::builder::basic::kvp("MeshValid", meshValid),
		bsoncxx::builder::basic::kvp("MeshData", meshDataStorageId)
	);
}

void EntityMeshCartesian::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
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

	meshValid = false;
	try
	{
		meshValid = doc_view["MeshValid"].get_bool();
	}
	catch (std::exception)
	{

	}

	meshDataStorageId = doc_view["MeshData"].get_int64();

	resetModified();
}

void EntityMeshCartesian::removeChild(EntityBase *child)
{
	if (child == meshData)
	{
		meshData = nullptr;
	}

	EntityContainer::removeChild(child);
}
