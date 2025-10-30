// @otlicense
// File: CartesianMeshCreation.cpp
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


#include "stdafx.h"

#include "CartesianMeshCreation.h"

#include "Application.h"

#include "EntityMeshCartesian.h"
#include "EntityMeshCartesianData.h"
#include "EntityGeometry.h"
#include "EntityMaterial.h"
#include "GeometryOperations.h"
#include "EntityMeshCartesianFace.h"
#include "EntityMeshCartesianFaceList.h"
#include "EntityMeshCartesianItem.h"
#include "EntityMeshCartesianNodes.h"
#include "EntityCartesianVector.h"
#include "CartesianMeshTree.h"
#include "TemplateDefaultManager.h"
#include "EntityVis2D3D.h"
#include "EntityVisCartesianFaceScalar.h"

#include "BRepPrimAPI_MakeBox.hxx"

#include "DataBase.h"

#include <chrono>
#include <thread>	
#include <limits>

#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#undef max
#undef min

vector3 operator*(float lhs, const vector3 &rhs)
{
	vector3 result;
	result.setX(lhs * rhs.getX());
	result.setY(lhs * rhs.getY());
	result.setZ(lhs * rhs.getZ());
	return result;
}

CartesianMeshCreation::CartesianMeshCreation() :
	application(nullptr),
	entityMesh(nullptr),
	numberPointInsideTests(0),
	matrixDs(nullptr),
	matrixDualDs(nullptr),
	matrixDa(nullptr),
	matrixDualDa(nullptr),
	matrixDeps(nullptr),
	matrixDmu(nullptr),
	matrixDsigma(nullptr)
{

}

CartesianMeshCreation::~CartesianMeshCreation()
{

}

std::string CartesianMeshCreation::getLargeNumberString(size_t number)
{
	struct dotted : std::numpunct<char> {
		char do_thousands_sep()   const { return '.'; }  // separate with dots
		std::string do_grouping() const { return "\3"; } // groups of 3 digits
		static void imbue(std::ostream &os) {
			os.imbue(std::locale(os.getloc(), new dotted));
		}
	};

	std::stringstream ss;
	dotted::imbue(ss);
	ss << number;

	return ss.str();
}

void CartesianMeshCreation::updateMesh(Application *app, EntityBase *meshEntity)
{
	assert(app != nullptr);
	setApplication(app);

	setUILock(true, MODEL_CHANGE);

	std::list<EntityGeometry *> geometryEntities;

	std::time_t timer = time(nullptr);
	//reportTime("Cartesian meshing started", timer);

	try
	{
		newTopologyEntities.clear();
		newDataEntities.clear();

		setEntityMesh(dynamic_cast<EntityMeshCartesian *>(meshEntity));

		// Delete the previous mesh data (if any)
		deleteMesh();

		// Read the setting from the mesh entity
		EntityPropertiesDouble *maximumEdgeLengthProperty = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Maximum edge length"));
		assert(maximumEdgeLengthProperty != nullptr);

		EntityPropertiesDouble *stepsAlongDiagonalProperty = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Number of steps along diagonal"));
		assert(stepsAlongDiagonalProperty != nullptr);

		EntityPropertiesBoolean *conformalMeshProperty = dynamic_cast<EntityPropertiesBoolean*>(getEntityMesh()->getProperties().getProperty("Conformal meshing"));
		EntityPropertiesBoolean *visualizeMatricesProperty = dynamic_cast<EntityPropertiesBoolean*>(getEntityMesh()->getProperties().getProperty("Visualize matrices"));

		double maximumEdgeLength = maximumEdgeLengthProperty->getValue();
		double stepsAlongDiagonal = stepsAlongDiagonalProperty->getValue();
		bool conformalMeshing = false;
		if (conformalMeshProperty != nullptr) conformalMeshing = conformalMeshProperty->getValue();
		bool visualizeMatrices = false;
		if (visualizeMatricesProperty != nullptr) visualizeMatrices = visualizeMatricesProperty->getValue();

		// Start the actual meshing process
		displayMessage("____________________________________________________________\n\n"
			"Cartesian mesh generation started.\n\n");

		// Get all geometry entities which need to be considered for meshing
		std::list<ot::UID> geometryEntitiesID = getAllGeometryEntitiesForMeshing();

		if (geometryEntitiesID.empty())
		{
			throw std::string("ERROR: Mesh cannot be created, because no shapes are selected for meshing.\n\n");
		}

		// Now load all geometry entities in the model
		getApplication()->prefetchDocumentsFromStorage(geometryEntitiesID);

		for (auto entityID : geometryEntitiesID)
		{
			ot::UID entityVersion = getApplication()->getPrefetchedEntityVersion(entityID);
			EntityGeometry* geom = dynamic_cast<EntityGeometry*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion));

			if (geom == nullptr)
			{
				std::string error = "ERROR: Unable to load geometry entity (" + std::to_string(entityID) + ", " + std::to_string(entityVersion) + ")\n\n";
				throw error;
			}
			else
			{
				geometryEntities.push_back(geom);
			}
		}

		EntityGeometry *backgroundCube = addBackgroundCubeTopology();
		if (backgroundCube != nullptr) geometryEntities.push_back(backgroundCube);

		// Now load the material information for each geometry object
		std::string error = readMaterialInformation(geometryEntities);
		if (!error.empty()) throw error;

		// In the following we need the Brep description for each object.
		// Now loop through every geometry entitity and check which data base objects will need to be loaded for pre-fetching
		std::list<ot::UID> prefetchIds;
		std::list<EntityBase *> allEntities;

		for (EntityGeometry *geomEntity : geometryEntities)
		{
			geomEntity->addPrefetchingRequirementsForBrep(prefetchIds);
			geomEntity->addPrefetchingRequirementsForFacets(prefetchIds);
			allEntities.push_back(geomEntity);
		}

		getApplication()->prefetchDocumentsFromStorage(prefetchIds);

		for (EntityGeometry *geomEntity : geometryEntities)
		{
			if (geomEntity != backgroundCube)
			{
				ot::UID facetID = geomEntity->getFacetsStorageObjectID();
				ot::UID facetVersion = application->getPrefetchedEntityVersion(facetID);

				ot::UID brepID = geomEntity->getBrepStorageObjectID();
				ot::UID brepVersion = application->getPrefetchedEntityVersion(brepID);

				EntityFacetData *facet = dynamic_cast<EntityFacetData *>(ot::EntityAPI::readEntityFromEntityIDandVersion(facetID, facetVersion));
				EntityBrep *brep = dynamic_cast<EntityBrep *>(ot::EntityAPI::readEntityFromEntityIDandVersion(brepID, brepVersion));

				geomEntity->setFacetEntity(facet);
				geomEntity->setBrepEntity(brep);
			}
		}

		// Now set the facet and brep entities for the geometry entities

		// In a first step, we need to determine the mesh line distribution. 
		setProgressInformation("Determine mesh line distribution", true);

		EntityMeshCartesianData *meshData = determineMeshLines(allEntities, maximumEdgeLength, stepsAlongDiagonal);
		newTopologyEntities.push_back(meshData);

		meshData->setName(getEntityMesh()->getName() + "/Mesh");
		meshData->setInitiallyHidden(true);

		// Add a background cube, if needed
		double backgroundStepWidth = 1000.0;
		addBackgroundCubeGeometry(backgroundCube, meshData, backgroundStepWidth);

		if (backgroundCube != nullptr)
		{
			backgroundShapeName = backgroundCube->getName();
		}

		// Display mesh line statistics
		displayMessage("Number of mesh lines:\n"
			"  x: " + std::to_string(meshData->getNumberLinesX()) + "\n"
			"  y: " + std::to_string(meshData->getNumberLinesY()) + "\n"
			"  z: " + std::to_string(meshData->getNumberLinesZ()) + "\n\n");
		displayMessage("Number of mesh cells: " + getLargeNumberString(meshData->getNumberCells()) + "\n\n");
		displayMessage("Smallest mesh step width: " + std::to_string(meshData->getSmallestStepWidth()) + "\n");
		displayMessage("Largest mesh step width: " + std::to_string(meshData->getLargestStepWidth()) + "\n");
		displayMessage("Mesh step ratio: " + std::to_string(meshData->getStepRatio()) + "\n\n");

		// Now we rasterize each object and determine the volumetric filling
		setProgressInformation("Cartesian meshing", false);

		determineVolumeFill(geometryEntities, meshData, conformalMeshing);

		// So far, we have created new mesh entities (topology and data), but no entity ID's have yet been assigned to them. 
		// In addition, the entities have not yet been stored. In a first step, assign new IDs to all newly created entities

		// First contatenate the lists of topology entities and data entities
		// (we store the data entities first such that the topology entities can refer to the data entities as children)
		std::list<EntityBase *> entityList = newDataEntities;
		entityList.insert(entityList.end(), newTopologyEntities.begin(), newTopologyEntities.end());

		// Assign new entity IDs to all newly created entities

		std::list<EntityBase*> currentEntityList = entityList;
		entityList.clear();

		std::map<ot::UID, bool> topologyEntityForceVisibleMap;

		for (auto entity : currentEntityList)
		{
			entity->setEntityID(getApplication()->getModelComponent()->createEntityUID());

			if (entity->getEntityType() == EntityBase::TOPOLOGY)
			{
				entityList.push_back(entity);
				topologyEntityForceVisibleMap[entity->getEntityID()] = (entity->getName() != backgroundMeshName);
			}
			else if (entity->getEntityType() == EntityBase::DATA)
			{
				entityList.push_back(entity);
			}
			else
			{
				assert(0); // Unknown entity type
			}
		}

		// Store all entities in the data base
		setProgressInformation("Storing mesh", true);

		DataBase::instance().setWritingQueueEnabled(true);

		if (getDsMatrix() != nullptr)
		{
			// Store the matrices such that we can use their storage versions later on
			getDsMatrix()->storeToDataBase();
			getDualDsMatrix()->storeToDataBase();
			getDaMatrix()->storeToDataBase();
			getDualDaMatrix()->storeToDataBase();
			getDepsMatrix()->storeToDataBase();
			getDmuMatrix()->storeToDataBase();
			getDsigmaMatrix()->storeToDataBase();

			// Now set the mesh matrices as child of the meshData entity
			meshData->setDsMatrix(getDsMatrix());
			meshData->setDualDsMatrix(getDualDsMatrix());
			meshData->setDaMatrix(getDaMatrix());
			meshData->setDualDaMatrix(getDualDaMatrix());
			meshData->setDepsMatrix(getDepsMatrix());
			meshData->setDmuMatrix(getDmuMatrix());
			meshData->setDsigmaMatrix(getDsigmaMatrix());
		}

		// Set the new mesh data object as data entity for the cartesian mesh
		getEntityMesh()->setMeshDataID(meshData->getEntityID());

		getEntityMesh()->storeToDataBase();

		meshData->storeToDataBase();

		// Now add the visualization for all matrices, if this debug option is turned on
		if (visualizeMatrices && getDsMatrix() != nullptr)
		{
			addMatrixPlot(EntityResultBase::CARTESIAN_EDGES_3D,	     meshData->getName() + "/Matrices/Ds",	   getDsMatrix(),		meshData, entityList, topologyEntityForceVisibleMap);
			addMatrixPlot(EntityResultBase::CARTESIAN_DUAL_EDGES_3D, meshData->getName() + "/Matrices/DualDs", getDualDsMatrix(),	meshData, entityList, topologyEntityForceVisibleMap);
			addMatrixPlot(EntityResultBase::CARTESIAN_FACES_3D,      meshData->getName() + "/Matrices/Da",	   getDaMatrix(),		meshData, entityList, topologyEntityForceVisibleMap);
			addMatrixPlot(EntityResultBase::CARTESIAN_DUAL_FACES_3D, meshData->getName() + "/Matrices/DualDa", getDualDaMatrix(),	meshData, entityList, topologyEntityForceVisibleMap);
			addMatrixPlot(EntityResultBase::CARTESIAN_DUAL_FACES_3D, meshData->getName() + "/Matrices/Deps",   getDepsMatrix(),	    meshData, entityList, topologyEntityForceVisibleMap);
			addMatrixPlot(EntityResultBase::CARTESIAN_FACES_3D,      meshData->getName() + "/Matrices/Dmu",	   getDmuMatrix(),	    meshData, entityList, topologyEntityForceVisibleMap);
			addMatrixPlot(EntityResultBase::CARTESIAN_DUAL_FACES_3D, meshData->getName() + "/Matrices/Dsigma", getDsigmaMatrix(),	meshData, entityList, topologyEntityForceVisibleMap);
		}

		for (auto entity : entityList)
		{
			entity->storeToDataBase();
		}
		DataBase::instance().setWritingQueueEnabled(false);

		// Now determine the id, version and parent lists of all entities 
		std::list<ot::UID> topologyEntityIDList;
		std::list<ot::UID> topologyEntityVersionList;
		std::list<bool> topologyEntityForceVisible;
		std::list<ot::UID> dataEntityIDList;
		std::list<ot::UID> dataEntityVersionList;
		std::list<ot::UID> dataEntityParentList;

		for (auto entity : entityList)
		{
			if (entity->getEntityType() == EntityBase::TOPOLOGY)
			{
				topologyEntityIDList.push_back(entity->getEntityID());
				topologyEntityVersionList.push_back(entity->getEntityStorageVersion());

				if (topologyEntityForceVisibleMap.count(entity->getEntityID()) != 0)
				{
					topologyEntityForceVisible.push_back(topologyEntityForceVisibleMap[entity->getEntityID()]);
				}
				else
				{
					topologyEntityForceVisible.push_back(false);
				}
			}
			else if (entity->getEntityType() == EntityBase::DATA)
			{
				dataEntityIDList.push_back(entity->getEntityID());
				dataEntityVersionList.push_back(entity->getEntityStorageVersion());
				dataEntityParentList.push_back(entity->getParent()->getEntityID());
			}
			else
			{
				assert(0); // Unknown entity type
			}
		}

		// Add the new mesh entity version to the topology
		topologyEntityIDList.push_front(getEntityMesh()->getEntityID());
		topologyEntityVersionList.push_front(getEntityMesh()->getEntityStorageVersion());
		topologyEntityForceVisible.push_front(false);

		// Add the newly created entities to the model service
		ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "update cartesian mesh");
	}
	catch (std::string &error)
	{
		displayMessage(error);
	}

	// Clean up by deleting the mesh entity from memory
	if (getEntityMesh() != nullptr)
	{
		delete getEntityMesh();
		setEntityMesh(nullptr);
	}

	// Delete the material information
	clearMaterialInformation(geometryEntities);

	// We delete all newly created topology entities (this will also remove the data entities as children)
	for (auto entity : newTopologyEntities)
	{
		delete entity;
	}
	newTopologyEntities.clear();

	// We also delete all the loaded geometry entities (this will also delete all their children)
	for (auto entity : geometryEntities)
	{
		delete entity;
	}
	geometryEntities.clear();

	// Now the meshing is completed and we remove the progress bar and the UI lock
	closeProgressInformation();
	setUILock(false, MODEL_CHANGE);
	reportTime("Cartesian meshing completed, total time", timer);
}

void CartesianMeshCreation::addMatrixPlot(EntityResultBase::tResultType resultType, const std::string &plotName, EntityCartesianVector *matrix, EntityMeshCartesianData *mesh,
										  std::list<EntityBase *> &entityList, std::map<ot::UID,bool> &topologyEntityForceVisible)
{
	EntityVis2D3D *visualizationEntity = nullptr;

	switch (resultType)
	{
	case EntityResultBase::tResultType::CARTESIAN_EDGES_3D:
	case EntityResultBase::tResultType::CARTESIAN_DUAL_EDGES_3D:
		visualizationEntity = new EntityVis2D3D(getApplication()->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
		break;
	case EntityResultBase::tResultType::CARTESIAN_FACES_3D:
	case EntityResultBase::tResultType::CARTESIAN_DUAL_FACES_3D:
		visualizationEntity = new EntityVisCartesianFaceScalar(getApplication()->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
		break;
	default:
		assert(0); // Unknown type
	}

	if (visualizationEntity == nullptr) return;

	visualizationEntity->setResultType(resultType);
	visualizationEntity->setName(plotName);
	visualizationEntity->setEditable(true);
	visualizationEntity->setInitiallyHidden(true);

	visualizationEntity->createProperties();

	visualizationEntity->setSource(matrix->getEntityID(), matrix->getEntityStorageVersion());
	visualizationEntity->setMesh(mesh->getEntityID(), mesh->getEntityStorageVersion());

	topologyEntityForceVisible[visualizationEntity->getEntityID()] = false;

	entityList.push_back(visualizationEntity);
}

void CartesianMeshCreation::clearMaterialInformation(const std::list<EntityGeometry *> &geometryEntities)
{
	for (auto entity : geometryEntities)
	{
		entity->setData(nullptr);
	}

	for (auto material : materialList)
	{
		delete material;
	}

	materialList.clear();
}

std::string CartesianMeshCreation::readMaterialInformation(const std::list<EntityGeometry *> &geometryEntities)
{
	std::string error;

	// First, we determine the materials which are assigned to the objects
	application->getModelComponent()->loadMaterialInformation();

	std::map<EntityGeometry *, std::string> objectMaterialNames;
	std::map<std::string, CartesianMeshMaterial *> materialMap;

	for (auto entity : geometryEntities)
	{
		EntityPropertiesEntityList *materialProperty = dynamic_cast<EntityPropertiesEntityList*>(entity->getProperties().getProperty("Material"));
		if (materialProperty == nullptr)
		{
			if (entity->getName().empty())
			{
				error = "ERROR: The background does not have a material assigned to it.\n\n";
			}
			else
			{
				error = "ERROR: Object " + entity->getName() + " does not have a material assigned to it.\n\n";
			}
			
			return error;
		}

		std::string materialName = application->getModelComponent()->getCurrentMaterialName(materialProperty);

		if (materialName.empty())
		{
			if (entity->getName().empty())
			{
				error = "ERROR: The background does not have a valid material assigned to it.\n\n";
			}
			else
			{
				error = "ERROR: Object " + entity->getName() + " does not have a valid material assigned to it.\n\n";
			}
			return error;
		}

		objectMaterialNames[entity] = materialName;
		materialMap[materialName] = nullptr;
	}

	// Now we have the material to object association in objectMaterialNames and a unique list of materials in materialNamesList;
	// We now create a list of materials in use and request the information about the corresponding ids from the model service

	std::list<std::string> materialNamesList;
	for (auto material : materialMap)
	{
		materialNamesList.push_back(material.first);
	}

	std::list<ot::EntityInformation> materialInfo;
	ot::ModelServiceAPI::getEntityInformation(materialNamesList, materialInfo);

	// Now we load the materials one by one and store them in the materials map

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds;
	for (auto info : materialInfo)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(info.getEntityID(), info.getEntityVersion()));
	}
	DataBase::instance().prefetchDocumentsFromStorage(prefetchIds);

	for (auto info : materialInfo)
	{
		EntityMaterial *material = dynamic_cast<EntityMaterial *>(ot::EntityAPI::readEntityFromEntityIDandVersion(info.getEntityID(), info.getEntityVersion()));

		if (material == nullptr)
		{
			error = "ERROR: Material " + info.getEntityName() + " does not exist.\n\n";
			return error;
		}

		EntityPropertiesSelection *materialType = dynamic_cast<EntityPropertiesSelection*>(material->getProperties().getProperty("Material type"));

		EntityPropertiesDouble *permittivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permittivity (relative)"));
		EntityPropertiesDouble *permeability = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permeability (relative)"));
		EntityPropertiesDouble *conductivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Conductivity"));

		EntityPropertiesDouble *priority = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Mesh priority"));

		assert(permittivity != nullptr);
		assert(permeability != nullptr);
		assert(conductivity != nullptr);
		assert(priority     != nullptr);

		CartesianMeshMaterial *materialData = new CartesianMeshMaterial;

		materialData->setIsPEC(materialType->getValue() == "PEC");

		materialData->setEpsilon(permittivity->getValue());
		materialData->setMu(permeability->getValue());
		materialData->setSigma(conductivity->getValue());

		materialData->setPriority(priority->getValue());

		delete material;

		materialList.push_back(materialData);
		materialMap[info.getEntityName()] = materialData;
	}

	// For faster access, we now need to store the pointer to the material in the objects temporary storage

	for (auto entity : objectMaterialNames)
	{
		CartesianMeshMaterial *material = materialMap[entity.second];
		assert(material != nullptr);

		entity.first->setData(material);
	}

	return error;
}

void CartesianMeshCreation::deleteMesh(void)
{
	getEntityMesh()->deleteMeshData();

	// Delete previous mesh data from the model (if it exsits)
	std::list<std::string> entityList{getEntityMesh()->getName() + "/Mesh"};
	ot::ModelServiceAPI::deleteEntitiesFromModel(entityList);
}

void CartesianMeshCreation::reportTime(const std::string &message, std::time_t &timer)
{
	double seconds = difftime(time(nullptr), timer);

	std::string text = message + ": " + std::to_string(seconds) + " sec.\n";
	displayMessage(text);

	timer = time(nullptr);
}

std::list<ot::UID> CartesianMeshCreation::getAllGeometryEntitiesForMeshing(void)
{
	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	std::string response;
	application->getModelComponent()->sendMessage(false, reqDoc, response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	return ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
}

EntityMeshCartesianData *CartesianMeshCreation::determineMeshLines(const std::list<EntityBase *> &meshEntities, double maximumEdgeLength, double stepsAlongDiagonalProperty)
{
	EntityMeshCartesianData *data = new EntityMeshCartesianData(0, nullptr, nullptr, nullptr, "Model");

	// Get the bounding box
	BoundingBox geometryBoundingBox;

	for (auto entity : meshEntities)
	{
		// Ignore the background shape which does not have a name assigned yet
		if (!entity->getName().empty())
		{
			if (entity->getClassName() != "EntityContainer")
			{
				double xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0, zmin = 0.0, zmax = 0.0;

				if (entity->getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax))
				{
					geometryBoundingBox.extend(xmin, xmax, ymin, ymax, zmin, zmax);
				}
			}
		}
	}

	// Now extend the bounding box by the settings if needed
	double offsetXmin = 0.0, offsetXmax = 0.0, offsetYmin = 0.0, offsetYmax = 0.0, offsetZmin = 0.0, offsetZmax = 0.0;

	double deltaX = geometryBoundingBox.getXmax() - geometryBoundingBox.getXmin();
	double deltaY = geometryBoundingBox.getYmax() - geometryBoundingBox.getYmin();
	double deltaZ = geometryBoundingBox.getZmax() - geometryBoundingBox.getZmin();

	determineBoundingBoxExtension(deltaX, deltaY, deltaZ, offsetXmin, offsetXmax, offsetYmin, offsetYmax, offsetZmin, offsetZmax);

	// Find the base step width
	double baseStepWidth = std::min(maximumEdgeLength, geometryBoundingBox.getDiagonal() / stepsAlongDiagonalProperty);

	// Now get the mesh lines in the coordinate directions
	determineMeshLinesOneDirection(geometryBoundingBox.getXmin() - offsetXmin, geometryBoundingBox.getXmax() + offsetXmax, baseStepWidth, data->getMeshLinesX());
	determineMeshLinesOneDirection(geometryBoundingBox.getYmin() - offsetYmin, geometryBoundingBox.getYmax() + offsetYmax, baseStepWidth, data->getMeshLinesY());
	determineMeshLinesOneDirection(geometryBoundingBox.getZmin() - offsetZmin, geometryBoundingBox.getZmax() + offsetZmax, baseStepWidth, data->getMeshLinesZ());

	// Store the mesh statistics in read-only properties
	EntityPropertiesBoolean::createProperty("Mesh Visualization", "Show mesh lines", false, "", data->getProperties());

	EntityPropertiesInteger::createProperty("Mesh Dimensions", "Number of mesh lines X", (long) data->getNumberLinesX(), "", data->getProperties());
	EntityPropertiesInteger::createProperty("Mesh Dimensions", "Number of mesh lines Y", (long) data->getNumberLinesY(), "", data->getProperties());
	EntityPropertiesInteger::createProperty("Mesh Dimensions", "Number of mesh lines Z", (long) data->getNumberLinesZ(), "", data->getProperties());
	EntityPropertiesString::createProperty("Mesh Dimensions", "Number of mesh cells",  getLargeNumberString(data->getNumberCells()) , "", data->getProperties());

	EntityPropertiesDouble::createProperty("Mesh Statistics", "Smallest mesh step width", data->getSmallestStepWidth(), "", data->getProperties());
	EntityPropertiesDouble::createProperty("Mesh Statistics", "Largest mesh step width", data->getLargestStepWidth(), "", data->getProperties());
	EntityPropertiesDouble::createProperty("Mesh Statistics", "Mesh step ratio", data->getStepRatio(), "", data->getProperties());

	data->getProperties().getProperty("Number of mesh lines X")->setReadOnly(true);
	data->getProperties().getProperty("Number of mesh lines Y")->setReadOnly(true);
	data->getProperties().getProperty("Number of mesh lines Z")->setReadOnly(true);
	data->getProperties().getProperty("Number of mesh cells")->setReadOnly(true);
	data->getProperties().getProperty("Smallest mesh step width")->setReadOnly(true);
	data->getProperties().getProperty("Largest mesh step width")->setReadOnly(true);
	data->getProperties().getProperty("Mesh step ratio")->setReadOnly(true);

	return data;
}

void CartesianMeshCreation::determineBoundingBoxExtension(double deltaX, double deltaY, double deltaZ,
														  double &offsetXmin, double &offsetXmax, double &offsetYmin, double &offsetYmax, double &offsetZmin, double &offsetZmax)
{
	EntityPropertiesSelection *backgroundMode = dynamic_cast<EntityPropertiesSelection*>(getEntityMesh()->getProperties().getProperty("Background mode"));
	EntityPropertiesBoolean *extendFlag = dynamic_cast<EntityPropertiesBoolean*>(getEntityMesh()->getProperties().getProperty("Extend in all directions"));
	EntityPropertiesEntityList *backgroundMaterial = dynamic_cast<EntityPropertiesEntityList*>(getEntityMesh()->getProperties().getProperty("Background material"));
	EntityPropertiesDouble *boundaryDistanceAllAbs = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance (abs)"));
	EntityPropertiesDouble *boundaryDistanceAllRel = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance (rel)"));
	EntityPropertiesDouble *boundaryDistanceAbsXmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmin (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsXmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmax (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsYmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymin (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsYmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymax (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsZmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmin (abs)"));
	EntityPropertiesDouble *boundaryDistanceAbsZmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmax (abs)"));
	EntityPropertiesDouble *boundaryDistanceRelXmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmin (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelXmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmax (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelYmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymin (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelYmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymax (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelZmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmin (rel)"));
	EntityPropertiesDouble *boundaryDistanceRelZmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmax (rel)"));

	if (backgroundMode != nullptr && extendFlag != nullptr && backgroundMaterial != nullptr && boundaryDistanceAllAbs != nullptr && boundaryDistanceAllRel != nullptr
		&& boundaryDistanceAbsXmin != nullptr && boundaryDistanceAbsXmax != nullptr && boundaryDistanceAbsYmin != nullptr && boundaryDistanceAbsYmax != nullptr
		&& boundaryDistanceAbsZmin != nullptr && boundaryDistanceAbsZmax != nullptr
		&& boundaryDistanceRelXmin != nullptr && boundaryDistanceRelXmax != nullptr && boundaryDistanceRelYmin != nullptr && boundaryDistanceRelYmax != nullptr
		&& boundaryDistanceRelZmin != nullptr && boundaryDistanceRelZmax != nullptr)
	{
		if (backgroundMode->getValue() == "Extend relative")
		{
			if (extendFlag->getValue())
			{
				// Same relative extension in all directions
				offsetXmin = boundaryDistanceAllRel->getValue() * deltaX;
				offsetXmax = boundaryDistanceAllRel->getValue() * deltaX;
				offsetYmin = boundaryDistanceAllRel->getValue() * deltaY;
				offsetYmax = boundaryDistanceAllRel->getValue() * deltaY;
				offsetZmin = boundaryDistanceAllRel->getValue() * deltaZ;
				offsetZmax = boundaryDistanceAllRel->getValue() * deltaZ;
			}
			else
			{
				// Direction specific relative extension
				offsetXmin = boundaryDistanceRelXmin->getValue() * deltaX;
				offsetXmax = boundaryDistanceRelXmax->getValue() * deltaX;
				offsetYmin = boundaryDistanceRelYmin->getValue() * deltaY;
				offsetYmax = boundaryDistanceRelYmax->getValue() * deltaY;
				offsetZmin = boundaryDistanceRelZmin->getValue() * deltaZ;
				offsetZmax = boundaryDistanceRelZmax->getValue() * deltaZ;
			}
		}
		else if (backgroundMode->getValue() == "Extend absolute")
		{
			if (extendFlag->getValue())
			{
				// Same absolute extension in all directions
				offsetXmin = boundaryDistanceAllAbs->getValue();
				offsetXmax = boundaryDistanceAllAbs->getValue();
				offsetYmin = boundaryDistanceAllAbs->getValue();
				offsetYmax = boundaryDistanceAllAbs->getValue();
				offsetZmin = boundaryDistanceAllAbs->getValue();
				offsetZmax = boundaryDistanceAllAbs->getValue();
			}
			else
			{
				// Direction specific absolute extension
				offsetXmin = boundaryDistanceAbsXmin->getValue();
				offsetXmax = boundaryDistanceAbsXmax->getValue();
				offsetYmin = boundaryDistanceAbsYmin->getValue();
				offsetYmax = boundaryDistanceAbsYmax->getValue();
				offsetZmin = boundaryDistanceAbsZmin->getValue();
				offsetZmax = boundaryDistanceAbsZmax->getValue();
			}
		}
	}
}

EntityGeometry *CartesianMeshCreation::addBackgroundCubeTopology(void)
{
	EntityPropertiesSelection *backgroundMode = dynamic_cast<EntityPropertiesSelection*>(getEntityMesh()->getProperties().getProperty("Background mode"));
	EntityPropertiesEntityList *backgroundMaterial = dynamic_cast<EntityPropertiesEntityList*>(getEntityMesh()->getProperties().getProperty("Background material"));

	if (backgroundMode != nullptr && backgroundMaterial != nullptr)
	{
		if (backgroundMode->getValue() != "Field free")
		{
			// We need to add a background cube. Its geometry will be added later

			EntityGeometry *backgroundEntity = new EntityGeometry(0, nullptr, nullptr, nullptr, "Model");

			if (backgroundMaterial != nullptr)
			{
				EntityPropertiesEntityList *material = new EntityPropertiesEntityList(*backgroundMaterial);
				material->setName("Material");
				backgroundEntity->getProperties().createProperty(material, "Material");
			}

			return backgroundEntity;
		}
	}

	return nullptr;
}

void CartesianMeshCreation::addBackgroundCubeGeometry(EntityGeometry *backgroundCube, EntityMeshCartesianData *meshData, double stepWidth)
{
	if (backgroundCube == nullptr) return; 

	EntityPropertiesEntityList *backgroundMaterial = dynamic_cast<EntityPropertiesEntityList*>(getEntityMesh()->getProperties().getProperty("Background material"));
	assert(backgroundMaterial != nullptr);

	// We need to add a background cube
	// Create the bounding sphere geometry and add it to the list of objects

	gp_Pnt pMin(meshData->getMeshLinesX()[0], meshData->getMeshLinesY()[0], meshData->getMeshLinesZ()[0]);
	gp_Pnt pMax(meshData->getMeshLinesX()[meshData->getNumberLinesX()-1], meshData->getMeshLinesY()[meshData->getNumberLinesY()-1], meshData->getMeshLinesZ()[meshData->getNumberLinesZ()-1]);
	BRepPrimAPI_MakeBox box(pMin, pMax);

	int valueR = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("Model", "Background cube color", 0, 204);
	int valueG = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("Model", "Background cube color", 1, 204);
	int valueB = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("Model", "Background cube color", 2, 204);

	backgroundCube->setName("Geometry/Background");
	backgroundCube->setBrep(box.Shape());

	EntityPropertiesDouble::createProperty( "Mesh",		"Mesh priority",						   -1e30, "", backgroundCube->getProperties());
	EntityPropertiesDouble::createProperty( "Mesh",		"Maximum edge length",				   stepWidth, "", backgroundCube->getProperties());
	EntityPropertiesColor::createProperty(  "Material", "Color",			  { valueR, valueG, valueB }, "", backgroundCube->getProperties());

	EntityPropertiesSelection::createProperty("Mesh", "Mesh refinement", { "Global setting", "Local setting" }, "Local setting", "", backgroundCube->getProperties());

	backgroundCube->facetEntity(false);
}


void CartesianMeshCreation::determineMeshLinesOneDirection(double min, double max, double step, std::vector<double> &coords)
{
	double numberSteps = (max - min) / step;
	size_t meshSteps = (size_t) numberSteps;
	if (numberSteps > meshSteps + 1e-3) meshSteps++;

	coords.clear();
	coords.resize(meshSteps + 1);

	for (size_t step = 0; step <= meshSteps; step++)
	{
		coords[step] = min + step * (max - min) / meshSteps;
	}
}

void CartesianMeshCreation::determineVolumeFill(std::list<EntityGeometry *> &geometryEntities, EntityMeshCartesianData *meshData, bool conformalMeshing)
{
	assert(meshData != nullptr);

	getApplication()->getUiComponent()->setProgress(0);

	std::time_t timer = time(nullptr);
	reportTime("Volume filling started", timer);

	// Allocate a mesh fill array for each mesh cell
	std::vector<EntityGeometry *> meshFill;
	meshFill.resize(meshData->getNumberCells(), nullptr);

	std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> *triangleInCellInformation = nullptr;

	if (conformalMeshing)
	{
		triangleInCellInformation = new std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>>;
		triangleInCellInformation->resize(meshData->getNumberCells());
	}

	// Cache the priorities
	std::map<EntityGeometry*, double> shapePriorities;

	for (auto shape : geometryEntities)
	{
		EntityPropertiesDouble *meshPriorityProperty = dynamic_cast<EntityPropertiesDouble*>(shape->getProperties().getProperty("Mesh priority"));
		double meshPriority = 0.0;
		if (meshPriorityProperty != nullptr) meshPriority = meshPriorityProperty->getValue();

		shapePriorities[shape] = meshPriority;
	}

	reportTime("Preparation completed", timer);

	// Create worker arrays for the shape fill
	std::vector<char> shapeFill;
	shapeFill.resize(meshData->getNumberCells());

	std::vector<CartesianMeshFillFront> fillFrontElements;
	fillFrontElements.resize(meshData->getNumberCells());

	reportTime("Field allocation completed", timer);

	// Now loop through the entities one by one and fill them into the meshFill

	std::map<std::pair<EntityGeometry*, EntityGeometry*>, bool> overlappingShapes;

	setProgress(5);

	double increment = 55.0 / geometryEntities.size();
	double percentage = 5.0;
	int lastPercent = 5;

	for (auto shape : geometryEntities)
	{
		numberPointInsideTests = 0;
		fillSingleShape(shape, meshFill, shapeFill, meshData, shapePriorities, overlappingShapes, fillFrontElements, triangleInCellInformation);
		reportTime("Filling shape: " + shape->getName() + " completed, (" + std::to_string(numberPointInsideTests) + " point inside tests)", timer);

		percentage += increment;
		if ((int)percentage > lastPercent)
		{
			lastPercent = (int)percentage;
			setProgress(lastPercent);
		}
	}

	// Extract the interfaces between different shapes from the meshFill

	if (conformalMeshing)
	{
		extractAndStoreMeshConformal(meshFill, meshData, timer, geometryEntities, shapePriorities, *triangleInCellInformation);

		// Clear the triangleInCellInformation
		delete triangleInCellInformation;
		triangleInCellInformation = nullptr;
	}
	else
	{
		extractAndStoreMesh(meshFill, meshData, timer, geometryEntities);
	}

	reportTime("All mesh entities created", timer);

	setProgress(100);
}

void CartesianMeshCreation::extractInterfaces(std::vector<EntityGeometry *> meshFill, EntityMeshCartesianData *meshData, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets)
{
	// Check for all facets with x-normal

	size_t nx = meshData->getNumberLinesX();
	size_t ny = meshData->getNumberLinesY();
	size_t nz = meshData->getNumberLinesZ();

	size_t mx = 1;
	size_t my = nx;
	size_t mz = nx * ny;

	extractInterfacesW(meshFill, boundaryFacets, ny, nz, nx, my, mz, mx, 0);
	extractInterfacesW(meshFill, boundaryFacets, nz, nx, ny, mz, mx, my, 1);
	extractInterfacesW(meshFill, boundaryFacets, nx, ny, nz, mx, my, mz, 2);
}

void CartesianMeshCreation::extractInterfacesW(std::vector<EntityGeometry *> meshFill, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets,
											   size_t nu, size_t nv, size_t nw, size_t mu, size_t mv, size_t mw, int dirW)
{
	// We have a local coordinate transform and extract the faces with a dirW normal

	// First slice in meshfill in w direction
	for (size_t iu = 0; iu < nu - 1; iu++)
	{
		for (size_t iv = 0; iv < nv - 1; iv++)
		{
			size_t index = iu * mu + iv * mv;
			if (meshFill[index] != nullptr)
			{
				// We have a boundary at this face
				addBoundaryFacet(dirW, index, nullptr, meshFill[index], boundaryFacets);
			}
		}
	}

	// All intermediate slices in the meshfill
	for (size_t iw = 0; iw < nw - 2; iw++)
	{
		for (size_t iu = 0; iu < nu - 1; iu++)
		{
			for (size_t iv = 0; iv < nv - 1; iv++)
			{
				size_t indexFrom = iu * mu + iv * mv + iw * mw;
				size_t indexTo = iu * mu + iv * mv + (iw+1) * mw;

				if (meshFill[indexFrom] != meshFill[indexTo])
				{
					addBoundaryFacet(dirW, indexTo, meshFill[indexFrom], meshFill[indexTo], boundaryFacets);
				}
			}
		}
	}

	// Last slice in meshfill in w direction
	for (size_t iu = 0; iu < nu - 1; iu++)
	{
		for (size_t iv = 0; iv < nv - 1; iv++)
		{
			size_t index = iu * mu + iv * mv + (nw-2) * mw;
			if (meshFill[index] != nullptr)
			{
				// We have a boundary at this face
				addBoundaryFacet(dirW, index + mw, meshFill[index], nullptr, boundaryFacets);
			}
		}
	}
}

void CartesianMeshCreation::addBoundaryFacet(int direction, size_t facetIndex, EntityGeometry *from, EntityGeometry *to, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets)
{
	auto itemFromTo = boundaryFacets.find(std::pair<EntityGeometry *, EntityGeometry *>(from, to));

	if (itemFromTo != boundaryFacets.end())
	{
		itemFromTo->second->addFacet(direction, facetIndex);
	}
	else
	{
		auto itemToFrom = boundaryFacets.find(std::pair<EntityGeometry *, EntityGeometry *>(to, from));

		if (itemToFrom != boundaryFacets.end())
		{
			itemToFrom->second->addFacet(direction, -((long long) facetIndex));
		}
		else
		{
			CartesianMeshBoundaryFacets *facets = new CartesianMeshBoundaryFacets;
			facets->addFacet(direction, facetIndex);
			boundaryFacets[std::pair<EntityGeometry *, EntityGeometry *>(from, to)] = facets;
		}
	}
}

void CartesianMeshCreation::storeBoundaryFaces(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets)
{
	int surfaceId = 1;

	for (auto item : boundaryFacets)
	{
		CartesianMeshBoundaryFacets *face = item.second;

		EntityMeshCartesianFace *entityFace = new EntityMeshCartesianFace(0 , nullptr, nullptr, nullptr, "Model");
		newDataEntities.push_back(entityFace);

		face->setFaceEntity(entityFace);

		entityFace->setSurfaceId(surfaceId);
		surfaceId++;

		for (int direction = 0; direction < 3; direction++)
		{
			std::list<long long> &facetList = face->getFacetList(direction);

			entityFace->setNumberCellFaces(direction, facetList.size());

			size_t faceIndex = 0;
			for (auto facet : facetList)
			{
				entityFace->setCellFace(direction, faceIndex, facet);
				faceIndex++;
			}
		}
	}
}

void CartesianMeshCreation::storeBoundaryFacesList(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> &boundaryFacets, EntityMeshCartesianData *meshData)
{
	EntityMeshCartesianFaceList *entityFaceList = meshData->getMeshFaces();
	newDataEntities.push_back(entityFaceList);

	for (auto item : boundaryFacets)
	{
		EntityMeshCartesianFace *entityFace = item.second->getFaceEntity();
		assert(entityFace != nullptr);

		entityFaceList->setFace(entityFace->getSurfaceId(), entityFace);
	}
}

void CartesianMeshCreation::fillCell(EntityGeometry *shape, std::vector<EntityGeometry *> &meshFill, size_t index, 
							         std::map<EntityGeometry*, double> &shapePriorities, std::map<std::pair<EntityGeometry*, EntityGeometry*>, bool> &overlappingShapes)
{
	if (meshFill[index] == nullptr)
	{
		meshFill[index] = shape;
	}
	else
	{
		if (shapePriorities[meshFill[index]] > shapePriorities[shape])
		{
			// The current shape has a lower priority -> do nothing
		}
		else if (shapePriorities[meshFill[index]] < shapePriorities[shape])
		{
			// The current shape has a higher priority -> update meshfill
			meshFill[index] = shape;
		}
		else
		{
			// The shapes have the same priority -> report overlap
			if (overlappingShapes.count(std::pair<EntityGeometry*, EntityGeometry*>(meshFill[index], shape)) > 0)
			{
				// This overlap was reported alreay -> do nothing
			}
			else if (overlappingShapes.count(std::pair<EntityGeometry*, EntityGeometry*>(shape, meshFill[index])) > 0)
			{
				// This overlap was reported alreay -> do nothing
			}
			else
			{
				// This overlap was not reported yet -> add it to the map
				overlappingShapes[std::pair<EntityGeometry*, EntityGeometry*>(meshFill[index], shape)] = true;
			}
		}
	}
}

void CartesianMeshCreation::fillSingleShape(EntityGeometry *shape, std::vector<EntityGeometry *> &meshFill, std::vector<char> &shapeFill, EntityMeshCartesianData *meshData, 
											std::map<EntityGeometry*, double> &shapePriorities, std::map<std::pair<EntityGeometry*, EntityGeometry*>, bool> &overlappingShapes,
										    std::vector<CartesianMeshFillFront> &fillFrontElements,
											std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> *triangleInCellInformation)
{
	EntityFacetData *facetData = shape->getFacets();
	assert(facetData != nullptr);

	size_t my = meshData->getNumberLinesX();
	size_t mz = meshData->getNumberLinesX() * meshData->getNumberLinesY();

	// Reset the shape fill field
	size_t ncells = meshData->getNumberCells();
	for (size_t index = 0; index < ncells; index++) shapeFill[index] = 0;

	// Now we render all the triangles one by one to get the cells filled along the object surface

	float shapeXmin = FLT_MAX;
	float shapeXmax = FLT_MIN;
	float shapeYmin = FLT_MAX;
	float shapeYmax = FLT_MIN;
	float shapeZmin = FLT_MAX;
	float shapeZmax = FLT_MIN;

	unsigned int nx = (unsigned int) meshData->getNumberLinesX();
	unsigned int ny = (unsigned int) meshData->getNumberLinesY();
	unsigned int nz = (unsigned int) meshData->getNumberLinesZ();

	double lengthX = meshData->getMeshLinesX()[meshData->getNumberLinesX() - 1] - meshData->getMeshLinesX()[0];
	double lengthY = meshData->getMeshLinesY()[meshData->getNumberLinesY() - 1] - meshData->getMeshLinesY()[0];
	double lengthZ = meshData->getMeshLinesZ()[meshData->getNumberLinesZ() - 1] - meshData->getMeshLinesZ()[0];

	double lengthDiag = sqrt(lengthX * lengthX + lengthY * lengthY + lengthZ * lengthZ);
	float delta = (float) (1e-6 * lengthDiag);

	CartesianMeshTree meshTree;

	for (auto &triangle : facetData->getTriangleList())
	{
		float nodes[3][3];

		nodes[0][0] = (float) facetData->getNodeVector()[triangle.getNode(0)].getCoord(0);
		nodes[0][1] = (float) facetData->getNodeVector()[triangle.getNode(0)].getCoord(1);
		nodes[0][2] = (float) facetData->getNodeVector()[triangle.getNode(0)].getCoord(2);

		nodes[1][0] = (float) facetData->getNodeVector()[triangle.getNode(1)].getCoord(0);
		nodes[1][1] = (float) facetData->getNodeVector()[triangle.getNode(1)].getCoord(1);
		nodes[1][2] = (float) facetData->getNodeVector()[triangle.getNode(1)].getCoord(2);

		nodes[2][0] = (float) facetData->getNodeVector()[triangle.getNode(2)].getCoord(0);
		nodes[2][1] = (float) facetData->getNodeVector()[triangle.getNode(2)].getCoord(1);
		nodes[2][2] = (float) facetData->getNodeVector()[triangle.getNode(2)].getCoord(2);

		float xmin = std::min(nodes[0][0], std::min(nodes[1][0], nodes[2][0]));
		float ymin = std::min(nodes[0][1], std::min(nodes[1][1], nodes[2][1]));
		float zmin = std::min(nodes[0][2], std::min(nodes[1][2], nodes[2][2]));

		float xmax = std::max(nodes[0][0], std::max(nodes[1][0], nodes[2][0]));
		float ymax = std::max(nodes[0][1], std::max(nodes[1][1], nodes[2][1]));
		float zmax = std::max(nodes[0][2], std::max(nodes[1][2], nodes[2][2]));

		meshTree.renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz,
							    meshData->getMeshLinesX(), meshData->getMeshLinesY(), meshData->getMeshLinesZ(),
								0, nx-1, 0, ny-1, 0, nz-1, delta, triangleInCellInformation, triangle, shape);

		shapeXmin = std::min(shapeXmin, xmin);
		shapeXmax = std::max(shapeXmax, xmax);
		shapeYmin = std::min(shapeYmin, ymin);
		shapeYmax = std::max(shapeYmax, ymax);
		shapeZmin = std::min(shapeZmin, zmin);
		shapeZmax = std::max(shapeZmax, zmax);
	}

	// We will now set all points outside the bounding box of the shape to code outside (2)

	// First initialize all unset point with outside (code 2)
	for (size_t index = 0; index < ncells; index++) if (shapeFill[index] == 0) shapeFill[index] = 2;

	// Now loop through all points inside the shape bounding box and turn the outside flags to unknown (code 0)
	size_t x1 = meshData->findLowerIndex(0, shapeXmin);
	size_t x2 = meshData->findUpperIndex(0, shapeXmax);
	size_t y1 = meshData->findLowerIndex(1, shapeYmin);
	size_t y2 = meshData->findUpperIndex(1, shapeYmax);
	size_t z1 = meshData->findLowerIndex(2, shapeZmin);
	size_t z2 = meshData->findUpperIndex(2, shapeZmax);

	if (x1 > 0) x1--;
	if (y1 > 0) y1--;
	if (z1 > 0) z1--;
	if (x2 < meshData->getNumberLinesX() - 2) x2++;
	if (y2 < meshData->getNumberLinesY() - 2) y2++;
	if (z2 < meshData->getNumberLinesZ() - 2) z2++;

	for (size_t iz = z1; iz < z2; iz++)
	{
		for (size_t iy = y1; iy < y2; iy++)
		{
			for (size_t ix = x1; ix < x2; ix++)
			{
				size_t index = ix + iy * my + iz * mz;

				if (shapeFill[index] == 2)
				{
					shapeFill[index] = 0;
				}
			}
		}
	}

	// Now fill all areas in the shapeFill by 1 (if inside the shape) or by 2 (if outside the shape)

	setupEmbree(facetData, true);

	fillSingleShapeVolume(meshData->getNumberLinesX(), meshData->getNumberLinesY(), meshData->getNumberLinesZ(), shapeFill, facetData,
						  meshData->getMeshLinesX(), meshData->getMeshLinesY(), meshData->getMeshLinesZ(), fillFrontElements);

	terminateEmbree();

	// Finally aggregate the shape fill information into the common meshFill

	for (size_t iz = z1; iz < z2; iz++)
	{
		for (size_t iy = y1; iy < y2; iy++)
		{
			for (size_t ix = x1; ix < x2; ix++)
			{
				size_t index = ix + iy * my + iz * mz;

				if (shapeFill[index] == 1)
				{
					fillCell(shape, meshFill, index, shapePriorities, overlappingShapes);
				}
			}
		}
	}
}

void CartesianMeshCreation::fillSingleShapeVolume(size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill, EntityFacetData *facetData,
												  std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
												  std::vector<CartesianMeshFillFront> &fillFrontElements)
{
	unsigned int ix = 0, iy = 0, iz = 0;

	while (findUndefinedCell(nx, ny, nz, shapeFill, ix, iy, iz))
	{
		double xpos = 0.5 * (linesX[ix] + linesX[ix + 1]);
		double ypos = 0.5 * (linesY[iy] + linesY[iy + 1]);
		double zpos = 0.5 * (linesZ[iz] + linesZ[iz + 1]);

		bool pointInside = testPointInside(xpos, ypos, zpos);
		
		// We need to start filling from the given cell. Determine the fill-code
		char fillCode = pointInside ? 1 : 2;

		fillNeighbouringVolume(ix, iy, iz, fillCode, nx, ny, nz, shapeFill, fillFrontElements);
	}
}

bool CartesianMeshCreation::findUndefinedCell(size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill, unsigned int &ix, unsigned int &iy, unsigned int &iz)
{
	size_t my = nx;
	size_t mz = nx * ny;

	while (iz < nz-1)
	{
		while (iy < ny-1)
		{
			while (ix < nx-1)
			{
				size_t index = ix + iy * my + iz * mz;

				if (shapeFill[index] == 0)
				{
					// We found an undefined cell
					return true;
				}

				ix++;
			}
			ix = 0;
			iy++;
		}
		iy = 0;
		iz++;
	}

	return false; // No more undefined cell found
}

void CartesianMeshCreation::fillNeighbouringVolume(unsigned int ix, unsigned int iy, unsigned int iz, char fillCode, size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill, std::vector<CartesianMeshFillFront> &fillFrontElements)
{
	fillFrontElements[0].setX(ix);
	fillFrontElements[0].setY(iy);
	fillFrontElements[0].setZ(iz);

	size_t nFrontElements = 1;

	while (nFrontElements > 0)
	{
		// Take the last element from the front, remove it from the array and potentially add unfilled neighbours
		updateFront(fillFrontElements, nFrontElements, fillCode, nx, ny, nz, shapeFill);

		//std::cout << "front size: " << nFrontElements << std::endl;
	}
}

void CartesianMeshCreation::updateFront(std::vector<CartesianMeshFillFront> &fillFrontElements, size_t &nFrontElements, char fillCode, size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill)
{
	unsigned int ix = fillFrontElements[nFrontElements - 1].getX();
	unsigned int iy = fillFrontElements[nFrontElements - 1].getY();
	unsigned int iz = fillFrontElements[nFrontElements - 1].getZ();

	nFrontElements--;

	// Fill the element itself with the fillCode
	size_t index = ix + iy * nx + iz * nx * ny;

	// Check if the item is empty or marked as a front element
	if (shapeFill[index] == 0 || shapeFill[index] == 3)
	{
		//std::cout << "filled: " << ix << ", " << iy << ", " << iz << std::endl;

		shapeFill[index] = fillCode;
	}	

	// Now check all neighbours of the element and add them to the front, if they are undefined
	expandFront(ix+1, iy, iz, fillFrontElements, nFrontElements, nx, ny, nz, shapeFill);
	expandFront(ix-1, iy, iz, fillFrontElements, nFrontElements, nx, ny, nz, shapeFill);
	expandFront(ix, iy+1, iz, fillFrontElements, nFrontElements, nx, ny, nz, shapeFill);
	expandFront(ix, iy-1, iz, fillFrontElements, nFrontElements, nx, ny, nz, shapeFill);
	expandFront(ix, iy, iz+1, fillFrontElements, nFrontElements, nx, ny, nz, shapeFill);
	expandFront(ix, iy, iz-1, fillFrontElements, nFrontElements, nx, ny, nz, shapeFill);
}

void CartesianMeshCreation::expandFront(unsigned int ix, unsigned int iy, unsigned int iz, std::vector<CartesianMeshFillFront> &fillFrontElements, size_t &nFrontElements, size_t nx, size_t ny, size_t nz, std::vector<char> &shapeFill)
{
	// Check if the point is outside the domain
	if (ix < 0 || ix > nx - 2) return;
	if (iy < 0 || iy > ny - 2) return;
	if (iz < 0 || iz > nz - 2) return;

	// Now check whether the point is still undefined
	size_t index = ix + iy * nx + iz * nx * ny;
	if (shapeFill[index] == 0)
	{
		shapeFill[index] = 3; // Mark the item as being a frontal item

		// Add the point to the front
		fillFrontElements[nFrontElements].setX(ix);
		fillFrontElements[nFrontElements].setY(iy);
		fillFrontElements[nFrontElements].setZ(iz);

		//std::cout << "front: " << ix << ", " << iy << ", " << iz << std::endl;

		nFrontElements++;
	}
}

void embreeHitFilter(const struct RTCFilterFunctionNArguments* args)
{
	args->valid[0] = 0; // Reject the hit such that further hits are detected

	RTCRay* ray = (RTCRay*) args->ray;
	RTCHit* hit = (RTCHit*) args->hit;

	CartesianMeshIntersectionData *intersectionData = (CartesianMeshIntersectionData *) args->geometryUserPtr;
	assert(intersectionData != nullptr);

	if (intersectionData->hitCount > 0)
	{
		// Check whether we have a new hit

		if (fabs(intersectionData->lastDistance - ray->tfar) > 1e-4)
		{
			assert(intersectionData->lastPrimID != hit->primID);

			// We have a new intersection point
			intersectionData->hitCount++;
			intersectionData->lastDistance = ray->tfar;
			intersectionData->lastPrimID = hit->primID;
		}
		else
		{
			assert(intersectionData->lastPrimID == hit->primID);
		}
	}
	else
	{
		intersectionData->hitCount = 1;
		intersectionData->lastDistance = ray->tfar;
		intersectionData->lastPrimID = hit->primID;
	}
}

void CartesianMeshCreation::setupEmbree(EntityFacetData *facetData, bool multipleIntersections)
{
	device = rtcNewDevice(NULL);
	scene  = rtcNewScene(device);
	geom   = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	size_t numberNodes = facetData->getNodeVector().size();
	size_t numberTriangles = facetData->getTriangleList().size();

	float* vb = (float*) rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), numberNodes);

	for (int iN = 0; iN < numberNodes; iN++)
	{
		vb[iN * 3    ] = (float)facetData->getNodeVector()[iN].getCoord(0);
		vb[iN * 3 + 1] = (float)facetData->getNodeVector()[iN].getCoord(1);
		vb[iN * 3 + 2] = (float)facetData->getNodeVector()[iN].getCoord(2);
	}

	unsigned* ib = (unsigned*) rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(unsigned), numberTriangles);

	size_t triangleIndex = 0;

	for (auto triangle : facetData->getTriangleList())
	{
		ib[triangleIndex++] = (unsigned int) triangle.getNode(0);
		ib[triangleIndex++] = (unsigned int) triangle.getNode(1);
		ib[triangleIndex++] = (unsigned int) triangle.getNode(2);
	}

	if (multipleIntersections)
	{
		rtcSetGeometryIntersectFilterFunction(geom, embreeHitFilter);
		rtcSetGeometryUserData(geom, &intersectionData);
	}

	rtcCommitGeometry(geom);
	rtcAttachGeometry(scene, geom);
	rtcReleaseGeometry(geom);
	rtcCommitScene(scene);
}

void CartesianMeshCreation::terminateEmbree(void)
{
	rtcReleaseScene(scene);
	rtcReleaseDevice(device);
}

bool CartesianMeshCreation::testPointInside(double xpos, double ypos, double zpos)
{
	numberPointInsideTests++;

	RTCRayHit rayhit; 
	rayhit.ray.org_x  = (float) xpos; rayhit.ray.org_y = (float) ypos; rayhit.ray.org_z = (float) zpos;
	rayhit.ray.dir_x  = 0.f; rayhit.ray.dir_y = 0.f; rayhit.ray.dir_z =  1.f;
	rayhit.ray.tnear  = 0.f;
	rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

	intersectionData.hitCount = 0;
	intersectionData.lastDistance = 0.0f;
	intersectionData.lastPrimID = 0;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	rtcIntersect1(scene, &context, &rayhit);

	if ((intersectionData.hitCount / 2) * 2 == intersectionData.hitCount)
	{
		// We have an even number of hits -> the point is outside
		return false;
	}

	// We have an odd number of hits, the point is inside
	return true;
}

void CartesianMeshCreation::extractAndStoreMesh(std::vector<EntityGeometry *> &meshFill, EntityMeshCartesianData *meshData, std::time_t &timer,
												std::list<EntityGeometry *> &geometryEntities)
{
	std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacets *> boundaryFacets;

	setProgress(60);

	createMatrices(meshFill, meshData);
	reportTime("Matrices created", timer);

	setProgress(80);

	extractInterfaces(meshFill, meshData, boundaryFacets);
	reportTime("Mesh interfaces extracted", timer);

	setProgress(85);

	// Now create an EntityMeshCartesianFace item for each boundary face
	storeBoundaryFaces(boundaryFacets);
	reportTime("Boundary face list created", timer);

	setProgress(90);

	// Create an EntityMeshCartesianFaceList item, add it to the EntityMeshCartesianData as child and add the EntityMeshCartesianFace ad children of the list
	storeBoundaryFacesList(boundaryFacets, meshData);
	reportTime("All boundary faces created", timer);

	// For each geometry entity, create a new EntityMeshCartesianItem and attach its faces. Add the cartesian mesh item as child to the 
	// EntityMeshCartesianData entities.

	std::map<EntityGeometry*, std::list<int>> boundaryFacesForShape;
	for (auto facet : boundaryFacets)
	{
		if (facet.first.first != nullptr)
		{
			boundaryFacesForShape[facet.first.first].push_back(facet.second->getFaceEntity()->getSurfaceId());
		}

		if (facet.first.second != nullptr)
		{
			boundaryFacesForShape[facet.first.second].push_back(-facet.second->getFaceEntity()->getSurfaceId());
		}
	}

	std::string geometryRootName = "Geometry";

	for (auto shape : geometryEntities)
	{
		EntityPropertiesColor *color = dynamic_cast<EntityPropertiesColor*>(shape->getProperties().getProperty("Color"));
		assert(color != nullptr);

		EntityMeshCartesianItem *meshEntity = new EntityMeshCartesianItem(0, nullptr, nullptr, nullptr, "Model");
		newTopologyEntities.push_back(meshEntity);

		std::string shapeName = shape->getName().substr(geometryRootName.size()+1);
		std::string name = meshData->getName() + "/" + shapeName;

		if (shape->getName() == backgroundShapeName)
		{
			backgroundMeshName = name;
		}

		meshEntity->setName(name);
		meshEntity->setMesh(meshData);
		meshEntity->setColor(color->getColorR(), color->getColorG(), color->getColorB());

		meshEntity->setNumberFaces(boundaryFacesForShape[shape].size());
		size_t faceIndex = 0;
		for (auto faceId : boundaryFacesForShape[shape])
		{
			meshEntity->setFace(faceIndex, faceId);
			faceIndex++;
		}

		meshEntity->setInitiallyHidden(true);
	}

	// Delete the boundary facets
	for (auto facet : boundaryFacets)
	{
		if (facet.second != nullptr)
		{
			delete facet.second;
		}
	}
	boundaryFacets.clear();
}

void CartesianMeshCreation::createNodes(unsigned int nx, unsigned int ny, unsigned int nz, 
										std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets, EntityMeshCartesianData *meshData, 
										CartesianMeshBoundaryPointData *&boundaryPointInfo)
{
	// First allocate an array to hold the indices of the mesh nodes being used in as surfaces
	size_t numMeshNodes = nx * ny * nz;

	std::vector<size_t> nodeIndices;
	nodeIndices.resize(numMeshNodes, std::numeric_limits<size_t>::max());
	
	// Now loop through all boundary facets and set increasing indices to the nodes which are used by the facets
	size_t nodeIndex = 0;

	for (auto facet : boundaryFacets)
	{
		std::list<CartesianMeshBoundaryFaceConformal> &facets = facet.second->getFacetList();

		for (CartesianMeshBoundaryFaceConformal &item : facets)
		{
			for (int ip = 0; ip < 4; ip++)
			{
				if (nodeIndices[item.getPoint(ip)] == std::numeric_limits<size_t>::max())
				{
					// This point was not yet set as being used -> assign a new index to it
					nodeIndices[item.getPoint(ip)] = nodeIndex;
					nodeIndex++;
				}
			}
		}
	}

	// Now we create a node vector which contains the points and their coordinates
	EntityMeshCartesianNodes *meshNodes = meshData->getMeshNodes();
	meshNodes->setNumberOfNodes(nodeIndex);

	boundaryPointInfo = new CartesianMeshBoundaryPointData[nodeIndex];

	for (size_t iz = 0; iz < nz; iz++)
	{
		for (size_t iy = 0; iy < ny; iy++)
		{
			for (size_t ix = 0; ix < nx; ix++)
			{
				size_t index = ix + iy * nx + iz * nx * ny;

				if (nodeIndices[index] != std::numeric_limits<size_t>::max())
				{
					// This node is in use -> store its coordinates
					meshNodes->setNodeCoordX(nodeIndices[index], meshData->getMeshLinesX()[ix]);
					meshNodes->setNodeCoordY(nodeIndices[index], meshData->getMeshLinesY()[iy]);
					meshNodes->setNodeCoordZ(nodeIndices[index], meshData->getMeshLinesZ()[iz]);

					boundaryPointInfo[nodeIndices[index]].setIndex((int) ix, (int) iy, (int) iz);
				}
			}
		}
	}

	// Finally, we switch the boundary facet indices to the point list indices

	for (auto facet : boundaryFacets)
	{
		std::list<CartesianMeshBoundaryFaceConformal> &facets = facet.second->getFacetList();

		for (CartesianMeshBoundaryFaceConformal &item : facets)
		{
			for (int ip = 0; ip < 4; ip++)
			{
				assert(nodeIndices[item.getPoint(ip)] != std::numeric_limits<size_t>::max());
				item.setPoint(ip, nodeIndices[item.getPoint(ip)]);
			}
		}
	}
}

void CartesianMeshCreation::extractInterfacesConformal(std::vector<EntityGeometry *> meshFill, EntityMeshCartesianData *meshData, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets)
{
	// Check for all facets with x-normal

	size_t nx = meshData->getNumberLinesX();
	size_t ny = meshData->getNumberLinesY();
	size_t nz = meshData->getNumberLinesZ();

	size_t mx = 1;
	size_t my = nx;
	size_t mz = nx * ny;

	extractInterfacesWConformal(meshFill, boundaryFacets, ny, nz, nx, my, mz, mx, 0);
	extractInterfacesWConformal(meshFill, boundaryFacets, nz, nx, ny, mz, mx, my, 1);
	extractInterfacesWConformal(meshFill, boundaryFacets, nx, ny, nz, mx, my, mz, 2);
}

void CartesianMeshCreation::extractInterfacesWConformal(std::vector<EntityGeometry *> meshFill, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets,
	size_t nu, size_t nv, size_t nw, size_t mu, size_t mv, size_t mw, int dirW)
{
	// We have a local coordinate transform and extract the faces with a dirW normal

	// First slice in meshfill in w direction
	for (size_t iu = 0; iu < nu - 1; iu++)
	{
		for (size_t iv = 0; iv < nv - 1; iv++)
		{
			size_t index = iu * mu + iv * mv;
			if (meshFill[index] != nullptr)
			{
				// We have a boundary at this face
				addBoundaryFacetConformal(dirW, iu, iv, 0, mu, mv, mw, nullptr, meshFill[index], -1, index, boundaryFacets);
			}
		}
	}

	// All intermediate slices in the meshfill
	for (size_t iw = 0; iw < nw - 2; iw++)
	{
		for (size_t iv = 0; iv < nv - 1; iv++)
		{
			for (size_t iu = 0; iu < nu - 1; iu++)
			{
				size_t indexFrom = iu * mu + iv * mv + iw * mw;
				size_t indexTo = iu * mu + iv * mv + (iw+1) * mw;

				if (meshFill[indexFrom] != meshFill[indexTo])
				{
					addBoundaryFacetConformal(dirW, iu, iv, iw+1, mu, mv, mw, meshFill[indexFrom], meshFill[indexTo], indexFrom, indexTo, boundaryFacets);
				}
			}
		}
	}

	// Last slice in meshfill in w direction
	for (size_t iu = 0; iu < nu - 1; iu++)
	{
		for (size_t iv = 0; iv < nv - 1; iv++)
		{
			size_t index = iu * mu + iv * mv + (nw-2) * mw;
			if (meshFill[index] != nullptr)
			{
				// We have a boundary at this face
				addBoundaryFacetConformal(dirW, iu, iv, nw-1, mu, mv, mw, meshFill[index], nullptr, index, -1, boundaryFacets);
			}
		}
	}
}

void CartesianMeshCreation::addBoundaryFacetConformal(int direction, size_t iu, size_t iv, size_t iw, size_t mu, size_t mv, size_t mw,
													  EntityGeometry *from, EntityGeometry *to, size_t indexFrom, size_t indexTo, std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets)
{
	auto itemFromTo = boundaryFacets.find(std::pair<EntityGeometry *, EntityGeometry *>(from, to));

	size_t p1 = iu     * mu + iv     * mv + iw * mw;
	size_t p2 = (iu+1) * mu + iv     * mv + iw * mw;
	size_t p3 = (iu+1) * mu + (iv+1) * mv + iw * mw;
	size_t p4 = iu     * mu + (iv+1) * mv + iw * mw;

	if (itemFromTo != boundaryFacets.end())
	{
		// Add forward face
		CartesianMeshBoundaryFaceConformal face;
		face.setPoints(p1, p2, p3, p4);
		face.setFrontBackCellIndex(indexFrom, indexTo);

		itemFromTo->second->addFacet(face);
	}
	else
	{
		auto itemToFrom = boundaryFacets.find(std::pair<EntityGeometry *, EntityGeometry *>(to, from));

		if (itemToFrom != boundaryFacets.end())
		{
			// Add reversed face
			CartesianMeshBoundaryFaceConformal face;
			face.setPoints(p4, p3, p2, p1);
			face.setFrontBackCellIndex(indexTo, indexFrom);

			itemToFrom->second->addFacet(face);
		}
		else
		{
			CartesianMeshBoundaryFacetsConformal *facets = new CartesianMeshBoundaryFacetsConformal;
			
			// Add forward face
			CartesianMeshBoundaryFaceConformal face;
			face.setPoints(p1, p2, p3, p4);
			face.setFrontBackCellIndex(indexFrom, indexTo);

			facets->addFacet(face);
			
			boundaryFacets[std::pair<EntityGeometry *, EntityGeometry *>(from, to)] = facets;
		}
	}
}

void CartesianMeshCreation::storeBoundaryFacesConformal(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets)
{
	int surfaceId = 1;

	for (auto item : boundaryFacets)
	{
		CartesianMeshBoundaryFacetsConformal *face = item.second;

		EntityMeshCartesianFace *entityFace = new EntityMeshCartesianFace(0, nullptr, nullptr, nullptr, "Model");
		newDataEntities.push_back(entityFace);

		face->setFaceEntity(entityFace);

		entityFace->setType(EntityMeshCartesianFace::POINT_BASED);
		entityFace->setSurfaceId(surfaceId);
		surfaceId++;

		std::list<CartesianMeshBoundaryFaceConformal> &facetList = face->getFacetList();

		entityFace->setNumberCellFaces(facetList.size());

		size_t faceIndex = 0;
		for (auto facet : facetList)
		{
			if (facet.getNumberOfPoints() == 4)
			{
				entityFace->setCellFace(faceIndex, facet.getPoint(0), facet.getPoint(1), facet.getPoint(2), facet.getPoint(3));
				faceIndex++;
			}

			if (facet.getNumberOfPoints() == 3)
			{
				entityFace->setCellFace(faceIndex, facet.getPoint(0), facet.getPoint(1), facet.getPoint(2), -1);
				faceIndex++;
			}
		}
	}
}

void CartesianMeshCreation::storeBoundaryFacesListConformal(std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> &boundaryFacets, EntityMeshCartesianData *meshData)
{
	EntityMeshCartesianFaceList *entityFaceList = meshData->getMeshFaces();
	newDataEntities.push_back(entityFaceList);

	for (auto item : boundaryFacets)
	{
		EntityMeshCartesianFace *entityFace = item.second->getFaceEntity();
		assert(entityFace != nullptr);

		entityFaceList->setFace(entityFace->getSurfaceId(), entityFace);
	}
}

void CartesianMeshCreation::extractAndStoreMeshConformal(std::vector<EntityGeometry *> &meshFill, EntityMeshCartesianData *meshData, std::time_t &timer,
														 std::list<EntityGeometry *> &geometryEntities, std::map<EntityGeometry*, double> shapePriorities,
														 std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> &triangleInCellInformation)
{
	std::map<std::pair<EntityGeometry*, EntityGeometry*>, CartesianMeshBoundaryFacetsConformal *> boundaryFacets;

	setProgress(75);

	// Extract the boundary facets
	extractInterfacesConformal(meshFill, meshData, boundaryFacets);
	reportTime("Mesh interfaces extracted", timer);

	// Now we need to create a point list containing all the nodes which are used by the boundary facets and attach this as mesh nodes
	CartesianMeshBoundaryPointData *boundaryPointInfo = nullptr;

	createNodes((unsigned int)meshData->getNumberLinesX(), (unsigned int)meshData->getNumberLinesY(), (unsigned int)meshData->getNumberLinesZ(), boundaryFacets, meshData, boundaryPointInfo);
	reportTime("Nodes created", timer);

	// Build the references from the points to the facets
	for (auto facet : boundaryFacets)
	{
		std::list<CartesianMeshBoundaryFaceConformal> &facets = facet.second->getFacetList();

		for (CartesianMeshBoundaryFaceConformal &item : facets)
		{
			for (int ip = 0; ip < 4; ip++)
			{
				boundaryPointInfo[item.getPoint(ip)].attachFacet(&item);
			}
		}
	}

	// We create a sorted list of entities
	std::list<MeshGeometryItem> sortedGeometryItems;

	for (auto geometryItem : geometryEntities)
	{
		MeshGeometryItem item;
		item.setGeometryItem(geometryItem);
		item.setPriority(shapePriorities[geometryItem]);

		sortedGeometryItems.push_back(item);
	}

	sortedGeometryItems.sort();
	sortedGeometryItems.reverse();

	// For each geometric entity, we now move the points to the surface

	// For each interface between two materials, we need to ensure that the face is processed
	// for the object which has a higher priority. The object with the higher priority will 
	// determine the boundary in case of overlaps. Furthermore, the volume of the higher
	// priority shape will always be over-estimated and the volume of the lower priority shape
	// may be under-estimated.
	std::map<EntityGeometry *, bool> alreadyProcessed;

	for (auto geometryItem : sortedGeometryItems)
	{
		for (auto facet : boundaryFacets)
		{
			if (facet.first.first == geometryItem.getGeometryItem() || facet.first.second == geometryItem.getGeometryItem())
			{
				// This facet belows to the current body. We process it here, since due to the sorting, we have a higher priority
				// than the following shapes.

				std::list<CartesianMeshBoundaryFaceConformal> &facets = facet.second->getFacetList();

				for (CartesianMeshBoundaryFaceConformal &item : facets)
				{
					if (item.isValid())
					{
						//size_t insideCellIndex = 0;

						//if (facet.first.first == geometryItem.getGeometryItem())
						//{
						//	insideCellIndex = item.getCellIndexFront();
						//}
						//else if (facet.first.second == geometryItem.getGeometryItem())
						//{
						//	insideCellIndex = item.getCellIndexBack();
						//}

						for (int ip = 0; ip < item.getNumberOfPoints(); ip++)
						{
							if (!boundaryPointInfo[item.getPoint(ip)].isFixed())
							{
								// This point can still be moved to the closest point on the geometry
							
								if (movePointToClosestPointOnGeometry(item.getPoint(ip), boundaryPointInfo, (int)meshData->getNumberLinesX(), (int)meshData->getNumberLinesY(), (int)meshData->getNumberLinesZ(),
																	  meshFill, geometryItem.getGeometryItem(), meshData->getMeshNodes(), geometryItem.getGeometryItem()->getFacets(), triangleInCellInformation))
								{
									// We have moved the point to the closest point on the geometry

									// We actually need to check whether the mesh is still valid
									//boundaryPointInfo[item.getPoint(ip)].setFixed();
								}
							}
						}
					}
				}
			}
		}
	}

	delete[] boundaryPointInfo;
	boundaryPointInfo = nullptr;

	// After the coordinates have reached their final destinations, the mesh nodes are created in the mesh Data
	newDataEntities.push_back(meshData->getMeshNodes());

	setProgress(85);

	// Now create an EntityMeshCartesianFace item for each boundary face
	storeBoundaryFacesConformal(boundaryFacets);
	reportTime("Boundary face list stored", timer);

	setProgress(90);

	// Create an EntityMeshCartesianFaceList item, add it to the EntityMeshCartesianData as child and add the EntityMeshCartesianFace ad children of the list
	storeBoundaryFacesListConformal(boundaryFacets, meshData);
	reportTime("All boundary faces stored", timer);

	// For each geometry entity, create a new EntityMeshCartesianItem and attach its faces. Add the cartesian mesh item as child to the 
	// EntityMeshCartesianData entities.

	std::map<EntityGeometry*, std::list<int>> boundaryFacesForShape;
	for (auto facet : boundaryFacets)
	{
		if (facet.first.first != nullptr)
		{
			boundaryFacesForShape[facet.first.first].push_back(facet.second->getFaceEntity()->getSurfaceId());
		}

		if (facet.first.second != nullptr)
		{
			boundaryFacesForShape[facet.first.second].push_back(-facet.second->getFaceEntity()->getSurfaceId());
		}
	}

	std::string geometryRootName = "Geometry";

	for (auto shape : geometryEntities)
	{
		EntityPropertiesColor *color = dynamic_cast<EntityPropertiesColor*>(shape->getProperties().getProperty("Color"));
		assert(color != nullptr);

		EntityMeshCartesianItem *meshEntity = new EntityMeshCartesianItem(0, nullptr, nullptr, nullptr, "Model");
		newTopologyEntities.push_back(meshEntity);

		std::string shapeName = shape->getName().substr(geometryRootName.size()+1);
		std::string name = meshData->getName() + "/" + shapeName;

		meshEntity->setName(name);
		meshEntity->setMesh(meshData);
		meshEntity->setColor(color->getColorR(), color->getColorG(), color->getColorB());

		meshEntity->setNumberFaces(boundaryFacesForShape[shape].size());
		size_t faceIndex = 0;
		for (auto faceId : boundaryFacesForShape[shape])
		{
			meshEntity->setFace(faceIndex, faceId);
			faceIndex++;
		}

		meshEntity->setInitiallyHidden(true);
	}

	// Delete the boundary facets
	for (auto facet : boundaryFacets)
	{
		if (facet.second != nullptr)
		{
			delete facet.second;
		}
	}
	boundaryFacets.clear();
}

bool CartesianMeshCreation::movePointToClosestPointOnGeometry(size_t pointIndex, CartesianMeshBoundaryPointData *boundaryPointInfo, int nx, int ny, int nz, std::vector<EntityGeometry *> &meshFill, EntityGeometry *geometryEntity, EntityMeshCartesianNodes *meshNodes, EntityFacetData *facets, std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> &triangleInCellInformation)
{
	vector3 closestPoint;
	float minDist = FLT_MAX;

	// We want to check the point at index pointIndex. Therefore, we may have up to 9 adjacent cells which we could check for nearby intersections

	int pointix = boundaryPointInfo[pointIndex].getIndexX();
	int pointiy = boundaryPointInfo[pointIndex].getIndexY();
	int pointiz = boundaryPointInfo[pointIndex].getIndexZ();

	for (int ix = std::max(0, pointix - 1); ix <= std::min(nx - 2, pointix); ix++)
	{
		for (int iy = std::max(0, pointiy - 1); iy <= std::min(ny - 2, pointiy); iy++)
		{
			for (int iz = std::max(0, pointiz - 1); iz <= std::min(nz - 2, pointiz); iz++)
			{
				size_t cellIndex = ix + iy * nx + iz * nx * ny;

				if (meshFill[cellIndex] == geometryEntity)
				{
					for (auto triangle : triangleInCellInformation[cellIndex][geometryEntity])
					{
						vector3 tNodes[3];

						tNodes[0].setX((float)facets->getNodeVector()[triangle->getNode(0)].getCoord(0));
						tNodes[0].setY((float)facets->getNodeVector()[triangle->getNode(0)].getCoord(1));
						tNodes[0].setZ((float)facets->getNodeVector()[triangle->getNode(0)].getCoord(2));

						tNodes[1].setX((float)facets->getNodeVector()[triangle->getNode(1)].getCoord(0));
						tNodes[1].setY((float)facets->getNodeVector()[triangle->getNode(1)].getCoord(1));
						tNodes[1].setZ((float)facets->getNodeVector()[triangle->getNode(1)].getCoord(2));

						tNodes[2].setX((float)facets->getNodeVector()[triangle->getNode(2)].getCoord(0));
						tNodes[2].setY((float)facets->getNodeVector()[triangle->getNode(2)].getCoord(1));
						tNodes[2].setZ((float)facets->getNodeVector()[triangle->getNode(2)].getCoord(2));

						vector3 point;
						point.setX((float)meshNodes->getNodeCoordX(pointIndex));
						point.setY((float)meshNodes->getNodeCoordY(pointIndex));
						point.setZ((float)meshNodes->getNodeCoordZ(pointIndex));

						vector3 newPoint = closestPointOnTriangle(tNodes, point);

						float distance2 = (newPoint.getX() - point.getX()) * (newPoint.getX() - point.getX())
							+ (newPoint.getY() - point.getY()) * (newPoint.getY() - point.getY())
							+ (newPoint.getZ() - point.getZ()) * (newPoint.getZ() - point.getZ());

						if (distance2 < minDist)
						{
							minDist = distance2;
							closestPoint = newPoint;
						}
					}
				}
			}
		}
	}

	//assert(minDist != FLT_MAX);

	if (minDist == FLT_MAX) return false;

	meshNodes->setNodeCoordX(pointIndex, closestPoint.getX());
	meshNodes->setNodeCoordY(pointIndex, closestPoint.getY());
	meshNodes->setNodeCoordZ(pointIndex, closestPoint.getZ());

	return true;
}

vector3 CartesianMeshCreation::closestPointOnTriangle( const vector3 *triangle, const vector3 &sourcePosition )
{
	vector3 edge0 = triangle[1] - triangle[0];
	vector3 edge1 = triangle[2] - triangle[0];
	vector3 v0 = triangle[0] - sourcePosition;

	float a = edge0.dot( edge0 );
	float b = edge0.dot( edge1 );
	float c = edge1.dot( edge1 );
	float d = edge0.dot( v0 );
	float e = edge1.dot( v0 );

	float det = a*c - b*b;
	float s = b*e - c*d;
	float t = b*d - a*e;

	if ( s + t < det )
	{
		if ( s < 0.f )
		{
			if ( t < 0.f )
			{
				if ( d < 0.f )
				{
					s = clamp( -d/a, 0.f, 1.f );
					t = 0.f;
				}
				else
				{
					s = 0.f;
					t = clamp( -e/c, 0.f, 1.f );
				}
			}
			else
			{
				s = 0.f;
				t = clamp( -e/c, 0.f, 1.f );
			}
		}
		else if ( t < 0.f )
		{
			s = clamp( -d/a, 0.f, 1.f );
			t = 0.f;
		}
		else
		{
			float invDet = 1.f / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else
	{
		if ( s < 0.f )
		{
			float tmp0 = b+d;
			float tmp1 = c+e;
			if ( tmp1 > tmp0 )
			{
				float numer = tmp1 - tmp0;
				float denom = a-2*b+c;
				s = clamp( numer/denom, 0.f, 1.f );
				t = 1-s;
			}
			else
			{
				t = clamp( -e/c, 0.f, 1.f );
				s = 0.f;
			}
		}
		else if ( t < 0.f )
		{
			if ( a+d > b+e )
			{
				float numer = c+e-b-d;
				float denom = a-2*b+c;
				s = clamp( numer/denom, 0.f, 1.f );
				t = 1-s;
			}
			else
			{
				s = clamp( -e/c, 0.f, 1.f );
				t = 0.f;
			}
		}
		else
		{
			float numer = c+e-b-d;
			float denom = a-2*b+c;
			s = clamp( numer/denom, 0.f, 1.f );
			t = 1.f - s;
		}
	}

	return triangle[0] + s * edge0 + t * edge1;
}

float CartesianMeshCreation::clamp(float value, float min, float max)
{
	if (value < min) value = min;
	if (value > max) value = max;

	return value;
}

void CartesianMeshCreation::setProgress(int percentage)
{
	assert(getApplication() != nullptr);

	if (getApplication()->getUiComponent())
	{
		getApplication()->getUiComponent()->setProgress(percentage);
	}
}

void CartesianMeshCreation::displayMessage(std::string message)
{
	assert(getApplication() != nullptr);

	if (getApplication()->getUiComponent())
	{
		getApplication()->getUiComponent()->displayMessage(message);
	}
}

void CartesianMeshCreation::setProgressInformation(std::string message, bool continuous)
{
	assert(getApplication() != nullptr);

	if (getApplication()->getUiComponent())
	{
		getApplication()->getUiComponent()->setProgressInformation(message, continuous);
	}
}

void CartesianMeshCreation::closeProgressInformation(void)
{
	assert(getApplication() != nullptr);

	if (getApplication()->getUiComponent())
	{
		getApplication()->getUiComponent()->closeProgressInformation();
	}
}

void CartesianMeshCreation::setUILock(bool flag, lockType type)
{
	assert(getApplication() != nullptr);

	if (getApplication()->getUiComponent())
	{
		static int count = 0;

		if (flag)
		{
			if (count == 0)
			{
				ot::LockTypes f;

				switch (type)
				{
				case ANY_OPERATION:
					f.setFlag(ot::LockType::All);
					break;
				case MODEL_CHANGE:
					f.setFlag(ot::LockType::ModelWrite);
					f.setFlag(ot::LockType::NavigationWrite);
					f.setFlag(ot::LockType::ViewWrite);
					f.setFlag(ot::LockType::Properties);
					break;
				default:
					assert(0); // Unknown lock type
					f.setFlag(ot::LockType::All);
				}

				getApplication()->getUiComponent()->lockUI(f);
			}

			count++;
		}
		else
		{
			assert(count > 0);
			if (count > 0)
			{
				count--;

				if (count == 0)
				{
					ot::LockTypes f;

					switch (type)
					{
					case ANY_OPERATION:
						f.setFlag(ot::LockType::All);
						break;
					case MODEL_CHANGE:
						f.setFlag(ot::LockType::ModelWrite);
						f.setFlag(ot::LockType::NavigationWrite);
						f.setFlag(ot::LockType::ViewWrite);
						f.setFlag(ot::LockType::Properties);
						break;
					default:
						assert(0); // Unknown lock type
						f.setFlag(ot::LockType::All);
					}

					getApplication()->getUiComponent()->unlockUI(f);
				}
			}
		}
	}
}

void CartesianMeshCreation::createMatrices(std::vector<EntityGeometry *> &meshFill, EntityMeshCartesianData *meshData)
{
	// Here we create the matrices and store them in the meshData object. Since all matrices are diagonal matrices, their diagonal is
	// stored as compressed vector

	// First, we create the dual ds and dual da matrices which are just containg one entries on the main diagonal

	matrixDualDs = new EntityCartesianVector(0, nullptr, nullptr, nullptr, "Model");
	matrixDualDs->setResultType(EntityResultBase::CARTESIAN_DUAL_EDGES_3D);
	newDataEntities.push_back(matrixDualDs);

	matrixDualDs->setConstantValue(1.0, 3 * meshData->getNumberCells());

	matrixDualDa = new EntityCartesianVector(0, nullptr, nullptr, nullptr, "Model");
	matrixDualDa->setResultType(EntityResultBase::CARTESIAN_DUAL_FACES_3D);
	newDataEntities.push_back(matrixDualDa);

	matrixDualDa->setConstantValue(1.0, 3 * meshData->getNumberCells());

	setProgress(61);

	// Now, we allocate the full storage for the ds matrix and initialize them by one as well. If a cell is made of PEC material, all adjacent
	// ds entries are set to zero. Afterward, the result is stored in the compressed matrix.
	
	size_t numberOfEdges = 3 * meshData->getNumberCells();

	double *matrix = new double[numberOfEdges];
	for (size_t index = 0; index < numberOfEdges; index++) matrix[index] = 1.0;

	setProgress(62);

	long long nx = meshData->getNumberLinesX();
	long long ny = meshData->getNumberLinesY();
	long long nz = meshData->getNumberLinesZ();

	long long np = nx * ny * nz;

	long long mx = 1;
	long long my = nx;
	long long mz = nx * ny;

	calcDsW(matrix + 2 * np, nx, ny, nz, mx, my, mz, meshFill.data());		// z normal direction
	setProgress(63);							   
	calcDsW(matrix +     np, nz, nx, ny, mz, mx, my, meshFill.data());		// y normal direction
	setProgress(64);							   
	calcDsW(matrix +      0, ny, nz, nx, my, mz, mx, meshFill.data());		// x normal direction
	setProgress(65);

	matrixDs = new EntityCartesianVector(0, nullptr, nullptr, nullptr, "Model");
	matrixDs->setResultType(EntityResultBase::CARTESIAN_EDGES_3D);

	newDataEntities.push_back(matrixDs);

	matrixDs->setValues(matrix, numberOfEdges);

	// No we determine the da matrix in the same way as the ds matrix before: all entries are one, except for those areas which are adjacent to a 
	// PEC material (which are set zo zero).

	// We can re-use the matrix storage, but have to initialize it by one again
	for (size_t index = 0; index < numberOfEdges; index++) matrix[index] = 1.0;

	calcDaW(matrix + 2 * np, nx, ny, nz, mx, my, mz, meshFill.data());		// z normal direction
	setProgress(66);
	calcDaW(matrix +     np, nz, nx, ny, mz, mx, my, meshFill.data());		// y normal direction
	setProgress(67);
	calcDaW(matrix +      0, ny, nz, nx, my, mz, mx, meshFill.data());		// x normal direction

	matrixDa = new EntityCartesianVector(0, nullptr, nullptr, nullptr, "Model");
	matrixDa->setResultType(EntityResultBase::CARTESIAN_FACES_3D);

	newDataEntities.push_back(matrixDa);

	matrixDa->setValues(matrix, numberOfEdges);
	setProgress(69);

	// Next, we calculate the material matrix for epsilon. This is done by area averaging the epsilon values of the materials adjacent to the 
	// respective edge. Inside the volume, each edge has four neighbours. At the surfaces of the mesh, it has two neighbours and at the edges 
	// of the mesh just one neighbour.

	// We can re-use the matrix storage, but have to initialize it again
	for (size_t index = 0; index < numberOfEdges; index++) matrix[index] = 0.0;

	calcDepsW(matrix + 2 * np, nx, ny, nz, mx, my, mz, meshData->getMeshLinesX().data(), meshData->getMeshLinesY().data(), meshFill.data());		// z normal direction
	setProgress(70);
	calcDepsW(matrix +     np, nz, nx, ny, mz, mx, my, meshData->getMeshLinesZ().data(), meshData->getMeshLinesX().data(), meshFill.data());		// y normal direction
	setProgress(71);
	calcDepsW(matrix +      0, ny, nz, nx, my, mz, mx, meshData->getMeshLinesY().data(), meshData->getMeshLinesZ().data(), meshFill.data());		// x normal direction

	matrixDeps = new EntityCartesianVector(0, nullptr, nullptr, nullptr, "Model");
	matrixDeps->setResultType(EntityResultBase::CARTESIAN_DUAL_FACES_3D);

	newDataEntities.push_back(matrixDeps);

	matrixDeps->setValues(matrix, numberOfEdges);
	setProgress(73);

	// Next, we calculate the material matrix for sigma. This is done by area averaging the sigma values of the materials adjacent to the 
	// respective edge. Inside the volume, each edge has four neighbours. At the surfaces of the mesh, it has two neighbours and at the edges 
	// of the mesh just one neighbour.

	// We can re-use the matrix storage, but have to initialize it again
	for (size_t index = 0; index < numberOfEdges; index++) matrix[index] = 0.0;

	calcDsigmaW(matrix + 2 * np, nx, ny, nz, mx, my, mz, meshData->getMeshLinesX().data(), meshData->getMeshLinesY().data(), meshFill.data());		// z normal direction
	setProgress(74);
	calcDsigmaW(matrix +     np, nz, nx, ny, mz, mx, my, meshData->getMeshLinesZ().data(), meshData->getMeshLinesX().data(), meshFill.data());		// y normal direction
	setProgress(75);
	calcDsigmaW(matrix +      0, ny, nz, nx, my, mz, mx, meshData->getMeshLinesY().data(), meshData->getMeshLinesZ().data(), meshFill.data());		// x normal direction

	matrixDsigma = new EntityCartesianVector(0, nullptr, nullptr, nullptr, "Model");
	matrixDsigma->setResultType(EntityResultBase::CARTESIAN_DUAL_FACES_3D);

	newDataEntities.push_back(matrixDsigma);

	matrixDsigma->setValues(matrix, numberOfEdges);
	setProgress(76);

	// Finally, we calculate the material matrix for mue. This is done by averaging the inverse mu values of the materials adjacent to the 
	// respective face. Inside the volume, each face has two neighbours. At the surfaces of the mesh, it has one neighbour.

	// We can re-use the matrix storage, but have to initialize it again
	for (size_t index = 0; index < numberOfEdges; index++) matrix[index] = 0.0;

	calcDmuW(matrix + 2 * np, nx, ny, nz, mx, my, mz, meshData->getMeshLinesZ().data(), meshFill.data());		// z normal direction
	setProgress(77);
	calcDmuW(matrix +     np, nz, nx, ny, mz, mx, my, meshData->getMeshLinesY().data(), meshFill.data());		// y normal direction
	setProgress(78);
	calcDmuW(matrix +      0, ny, nz, nx, my, mz, mx, meshData->getMeshLinesX().data(), meshFill.data());		// x normal direction

	matrixDmu = new EntityCartesianVector(0, nullptr, nullptr, nullptr, "Model");
	matrixDmu->setResultType(EntityResultBase::CARTESIAN_FACES_3D);

	newDataEntities.push_back(matrixDmu);

	matrixDmu->setValues(matrix, numberOfEdges);
	setProgress(80);

	delete[] matrix;
	matrix = nullptr;
}

void CartesianMeshCreation::calcDsW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, EntityGeometry **meshFill)
{
	#pragma omp parallel for
	for (long long iw = 0; iw < nw-1; iw++)
	{
		for (long long iv = 0; iv < nv-1; iv++)
		{
			for (long long iu = 0; iu < nu-1; iu++)
			{
				long long index = iu * mu + iv * mv + iw * mw;

				if (meshFill[index] == nullptr || ((CartesianMeshMaterial *)meshFill[index]->getData())->getIsPEC())
				{
					// PEC material -> set the four adjacent ds entries to zero

					matrix[index		   ] = 0.0;
					matrix[index + mu	   ] = 0.0;
					matrix[index + mv	   ] = 0.0;
					matrix[index + mu + mv ] = 0.0;
				}
			}
		}
	}
}

void CartesianMeshCreation::calcDaW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, EntityGeometry **meshFill)
{
	#pragma omp parallel for
	for (long long iw = 0; iw < nw-1; iw++)
	{
		for (long long iv = 0; iv < nv-1; iv++)
		{
			for (long long iu = 0; iu < nu-1; iu++)
			{
				long long index = iu * mu + iv * mv + iw * mw;

				if (meshFill[index] == nullptr || ((CartesianMeshMaterial *)meshFill[index]->getData())->getIsPEC())
				{
					// PEC material -> set the two adjacent faces with normal w to zero

					matrix[index     ] = 0.0;
					matrix[index + mw] = 0.0;
				}
			}
		}
	}
}

void CartesianMeshCreation::calcDmuW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, double *meshLinesW, EntityGeometry **meshFill)
{
	// Bottom slice

	long long iw = 0;

	for (long long iv = 0; iv < nv-1; iv++)
	{
		for (long long iu = 0; iu < nu-1; iu++)
		{
			long long index = iu * mu + iv * mv + iw * mw;

			if (meshFill[index] == nullptr)
			{
				matrix[index] = 0.0;
			}
			else
			{
				matrix[index] = ((CartesianMeshMaterial *) meshFill[index]->getData())->getMu();
			}

		}
	}

	// Middle slices

	#pragma omp parallel for
	for (long long iw = 1; iw < nw-1; iw++)
	{
		double ds1 = meshLinesW[iw    ] - meshLinesW[iw - 1];
		double ds2 = meshLinesW[iw + 1] - meshLinesW[iw    ];

		for (long long iv = 0; iv < nv-1; iv++)
		{
			for (long long iu = 0; iu < nu-1; iu++)
			{
				long long index = iu * mu + iv * mv + iw * mw;

				if (meshFill[index - mw] == nullptr || meshFill[index] == nullptr)
				{
					matrix[index] = 0.0;
				}
				else
				{
					double mu1 = ((CartesianMeshMaterial *) meshFill[index - mw]->getData())->getMu();
					double mu2 = ((CartesianMeshMaterial *) meshFill[index     ]->getData())->getMu();

					assert(mu1 != 0.0 && mu2 != 0.0);

					matrix[index] = (ds1 + ds2) / (ds1 / mu1 + ds2 / mu2);
				}
			}
		}
	}

	// Top slice

	iw = nw-1;

	for (long long iv = 0; iv < nv-1; iv++)
	{
		for (long long iu = 0; iu < nu-1; iu++)
		{
			long long index = iu * mu + iv * mv + iw * mw;

			if (meshFill[index] == nullptr)
			{
				matrix[index] = 0.0;
			}
			else
			{
				matrix[index] = ((CartesianMeshMaterial *) meshFill[index - mw]->getData())->getMu();
			}
		}
	}
}

void CartesianMeshCreation::calcDepsW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, double *meshLinesU, double *meshLinesV, EntityGeometry **meshFill)
{
	#pragma omp parallel for
	for (long long iw = 0; iw < nw-1; iw++)
	{
		double a[4] = { 0 };

		for (long long iv = 0; iv < nv; iv++)
		{
			double dv1 = iv > 0    ? meshLinesV[iv    ] - meshLinesV[iv - 1] : 0.0;
			double dv2 = iv < nv-1 ? meshLinesV[iv + 1] - meshLinesV[iv    ] : 0.0;

			for (long long iu = 0; iu < nu; iu++)
			{
				long long index = iu * mu + iv * mv + iw * mw;

				double eps[4] = { 0 };

				double du1 = iu > 0    ? meshLinesU[iu    ] - meshLinesU[iu - 1] : 0.0;
				double du2 = iu < nu-1 ? meshLinesU[iu + 1] - meshLinesU[iu    ] : 0.0;

				if (iu > 0    && iv > 0    && meshFill[index - mu - mv] != nullptr) eps[0] = ((CartesianMeshMaterial *) meshFill[index - mu - mv]->getData())->getEpsilon();
				if (iu < nu-1 && iv > 0    && meshFill[index - mv     ] != nullptr) eps[1] = ((CartesianMeshMaterial *) meshFill[index - mv     ]->getData())->getEpsilon();
				if (iu > 0    && iv < nv-1 && meshFill[index - mu     ] != nullptr) eps[2] = ((CartesianMeshMaterial *) meshFill[index - mu     ]->getData())->getEpsilon();
				if (iu < nu-1 && iv < nv-1 && meshFill[index          ] != nullptr) eps[3] = ((CartesianMeshMaterial *) meshFill[index          ]->getData())->getEpsilon();

				a[0] = du1 * dv1;
				a[1] = du2 * dv1;
				a[2] = du1 * dv2;
				a[3] = du2 * dv2;

				matrix[index] = (eps[0] * a[0] + eps[1] * a[1] + eps[2] * a[2] + eps[3] * a[3]) / (a[0] + a[1] + a[2] + a[3]);
			}
		}
	}
}

void CartesianMeshCreation::calcDsigmaW(double *matrix, long long nu, long long nv, long long nw, long long mu, long long mv, long long mw, double *meshLinesU, double *meshLinesV, EntityGeometry **meshFill)
{
	#pragma omp parallel for
	for (long long iw = 0; iw < nw-1; iw++)
	{
		double a[4] = { 0 };

		for (long long iv = 0; iv < nv; iv++)
		{
			double dv1 = iv > 0    ? meshLinesV[iv    ] - meshLinesV[iv - 1] : 0.0;
			double dv2 = iv < nv-1 ? meshLinesV[iv + 1] - meshLinesV[iv    ] : 0.0;

			for (long long iu = 0; iu < nu; iu++)
			{
				long long index = iu * mu + iv * mv + iw * mw;

				double eps[4] = { 0 };

				double du1 = iu > 0    ? meshLinesU[iu    ] - meshLinesU[iu - 1] : 0.0;
				double du2 = iu < nu-1 ? meshLinesU[iu + 1] - meshLinesU[iu    ] : 0.0;

				if (iu > 0    && iv > 0    && meshFill[index - mu - mv] != nullptr) eps[0] = ((CartesianMeshMaterial *) meshFill[index - mu - mv]->getData())->getSigma();
				if (iu < nu-1 && iv > 0    && meshFill[index - mv     ] != nullptr) eps[1] = ((CartesianMeshMaterial *) meshFill[index - mv     ]->getData())->getSigma();
				if (iu > 0    && iv < nv-1 && meshFill[index - mu     ] != nullptr) eps[2] = ((CartesianMeshMaterial *) meshFill[index - mu     ]->getData())->getSigma();
				if (iu < nu-1 && iv < nv-1 && meshFill[index          ] != nullptr) eps[3] = ((CartesianMeshMaterial *) meshFill[index          ]->getData())->getSigma();

				a[0] = du1 * dv1;
				a[1] = du2 * dv1;
				a[2] = du1 * dv2;
				a[3] = du2 * dv2;

				matrix[index] = (eps[0] * a[0] + eps[1] * a[1] + eps[2] * a[2] + eps[3] * a[3]) / (a[0] + a[1] + a[2] + a[3]);
			}
		}
	}
}

#include "MeshSlicer.h"

void CartesianMeshCreation::determine2DFill(std::list<EntityGeometry *> &geometryEntities, EntityMeshCartesianData *meshData)
{
	for (auto geometry : geometryEntities)
	{
		EntityFacetData *facetData = geometry->getFacets();
		MeshSlicer slicer;
		slicer.doit(facetData);

	}

}

