// @otlicense
// File: GmshMeshCreation.cpp
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


#define _USE_MATH_DEFINES

#include "GmshMeshCreation.h"
#include "Properties.h"
#include "StepWidthManager.h"
#include "ModelBuilder.h"
#include "MaterialManager.h"
#include "FaceAnnotationsManager.h"
#include "ObjectManager.h"
#include "ProgressLogger.h"
#include "ProximityMeshing.h"
#include "MeshWriter.h"
#include "PhysicalGroupsManager.h"

#include "OTModelAPI/ModelServiceAPI.h"

#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTModelEntities/EntityMeshTet.h"
#include "OTModelEntities/EntityMaterial.h"
#include "OTModelEntities/EntityAnnotation.h"
#include "OTModelEntities/EntityMeshTetFace.h"
#include "OTModelEntities/EntityMeshTetItem.h"
#include "OTModelEntities/EntityMeshTetInfo.h"
#include "OTModelEntities/EntityMeshTetData.h"
#include "OTModelEntities/EntityMeshTetFaceData.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/InvalidUID.h"

#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/EntityFaceAnnotation.h"
#include "OTCADEntities/GeometryOperations.h"
#include "SelfIntersectionCheck.h"
#include "Application.h"

#include <set>
#include <cmath>
#include <cassert>

#include <gmsh.h_cwrap>

#include "BRepBuilderAPI_Copy.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS_Shape.hxx"
#include "TopoDS_TShape.hxx"
#include "TopoDS.hxx"
#include "BRepAlgoAPI_Check.hxx"
#include "BRepPrimAPI_MakeBox.hxx"
#include "BRepAlgoAPI_Cut.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepAdaptor_Surface.hxx"
#include "BRepLProp_SLProps.hxx"
#include "Standard_Handle.hxx"
#include "BRepAlgoAPI_Fuse.hxx"
#include "BRepPrimAPI_MakeSphere.hxx"

#include <chrono>
#include <thread>	

#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"


GmshMeshCreation::GmshMeshCreation(Application *app) : 
	application(app),
	entityMesh(nullptr),
	loggerRunning(false)
{

}

GmshMeshCreation::~GmshMeshCreation()
{

}

void GmshMeshCreation::updateMesh(EntityMeshTet *mesh)
{
	setUILock(true, MODEL_CHANGE);

	assert(mesh != nullptr);
	setEntityMesh(mesh);

	application->getModelComponent()->clearNewEntityList();

	// Get information about the materials folder
	std::string materialsFolder = "Materials";

	std::list<std::string> entityList{ materialsFolder };
	std::list<ot::EntityInformation> entityInfo;

	ot::ModelServiceAPI::getEntityInformation(entityList, entityInfo);

	assert(entityInfo.size() == 1);
	assert(entityInfo.front().getEntityName() == materialsFolder);

	ot::UID materialsFolderID = entityInfo.front().getEntityID();

	std::time_t totalTimer = time(nullptr);
	std::time_t timer = time(nullptr);
	
	reportTime("Time: Meshing started", timer, false);

	// Initialize the gmsh component
	gmsh::initialize();

	DataBase::instance().setWritingQueueEnabled(true);
	ot::ModelServiceAPI::enableMessageQueueing(true);

	Properties properties;
	ModelBuilder meshModelBuilder(application);		
	StepWidthManager stepWidthManager(application);
	FaceAnnotationsManager annotationsManager(application);
	ObjectManager objectManager(application);
	MeshWriter meshWriter(application, getEntityMesh());
	BoundingBox boundingBox;
	ProximityMeshing proximityMeshing(application);
	PhysicalGroupsManager groupsManager(application);

	std::list<std::pair<ot::UID, ot::UID>> inputDependencyList;
	std::list<std::pair<ot::UID, ot::UID>> outputDependencyList;

	try
	{
		// -----------------------------------------------------------------------------------------
		// In a first step, we load all necessary data for the meshing process 
		// (this will also create the input dependency list)
		// -----------------------------------------------------------------------------------------

		inputDependencyList.push_back(std::pair<ot::UID, ot::UID>(mesh->getEntityID(), mesh->getEntityStorageVersion()));  // Add ourselves to the dependency list (because of the properties)

		// Now create a list of all geometry entities in the model
		std::list<ot::UID> geometryEntitiesID = getAllGeometryEntitiesForMeshing();

		std::list<EntityGeometry *> geometryEntities = loadGeometryEntitiesAndBreps(geometryEntitiesID, inputDependencyList);

		properties.readSettings(getEntityMesh());

		MaterialManager materialManager(application);
		materialManager.loadNecessaryMaterials(geometryEntities, properties, inputDependencyList);

		annotationsManager.loadAllFaceAnnotations(inputDependencyList);

		// -----------------------------------------------------------------------------------------
		// Check whether the mesh is up-to-date and does not need updating
		// -----------------------------------------------------------------------------------------
		
		inputDependencyList.sort();
		inputDependencyList.unique();

		if (!meshNeedsUpdating(mesh, inputDependencyList))
		{
			// Nothing to do here. Stop the meshing process
			displayMessage("____________________________________________________________\n\nThe mesh is already up-to-date.\n");

			gmsh::finalize();

			closeProgressInformation();

			DataBase::instance().setWritingQueueEnabled(false);
			ot::ModelServiceAPI::enableMessageQueueing(false);

			setUILock(false, MODEL_CHANGE);
			return;
		}

		// The mesh needs to be updated, so we delete the old mesh first.
		deleteMesh();

		// Create a new emtpy mesh data entity
		getEntityMesh()->setModified();
		getEntityMesh()->getMeshData()->setName(getEntityMesh()->getName() + "/Mesh");
		getEntityMesh()->getMeshData()->setEntityID(application->getModelComponent()->createEntityUID());

		// -----------------------------------------------------------------------------------------
		// Prepare the model for the meshing process
		// -----------------------------------------------------------------------------------------

		if (geometryEntities.empty())
		{
			throw std::string("ERROR: Mesh cannot be created, because no shapes are selected for meshing.\n\n");
		}

		boundingBox = ot::GeometryOperations::getBoundingBox(geometryEntities);

		stepWidthManager.determineBaseStepWidths(boundingBox, properties);

		gmsh::model::add("model");

		gmsh::option::setNumber("Mesh.Optimize",					properties.getMeshOptimization() ? 1 : 0);
		gmsh::option::setNumber("Mesh.Algorithm",					properties.getMeshAlg2D());
		gmsh::option::setNumber("Mesh.Algorithm3D",					properties.getMeshAlg3D());
		gmsh::option::setNumber("Mesh.MeshSizeExtendFromBoundary",	0);
		gmsh::option::setNumber("Mesh.MinimumCircleNodes",			3);

		displayMessage("____________________________________________________________\n\n"
					   "Tetrahedral mesh generation started.\n\n"
					   "1/3: Building mesh geometry model\n");

		reportTime("\tTime: Meshing preparation completed", timer, properties.getVerbose());

		setProgressInformation("Step 1/3 : Building mesh model", true);

		meshModelBuilder.buildModel(getEntityMesh()->getName(), geometryEntities, properties, stepWidthManager, materialManager);

		reportTime("\tTime: Meshing geometry model build completed", timer, properties.getVerbose());

		annotationsManager.buildEntityNameToAnnotationsMap();

		for (auto entity : meshModelBuilder.getModelEntities())
		{
			const TopoDS_Shape *shape = &((entity->getBrep()));
			std::string entityName    = entity->getName().substr(getEntityMesh()->getName().size() + 1);

			annotationsManager.buildFaceToAnnotationMap(entityName, entity->getBrepEntity());
			annotationsManager.buildIndexedFaceToAnnotationVector(shape);

			stepWidthManager.buildIndexedFaceToStepSizeVector(entity, shape, properties);

			gmsh::vectorpair output;
			gmsh::model::occ::importShapesNativePointer((const void *)shape, output, false);

			annotationsManager.buildTagToAnnotationMap(output);
			stepWidthManager.buildTagToAnnotationMap(output);

			objectManager.storeMeshElementsForEntity(entity, output);
		}

		if (!objectManager.hasAnyVolumes())
		{
			throw std::string("\nERROR: Topology error - no volumes coud be built for the model. Meshing failed.\n");
		}
		else
		{
			if (properties.getVerbose()) displayMessage("\tInfo: Mesh geometry model successfully built.\n");
		}

		// Build the internal mesh model represenation in gmsh
		gmsh::model::occ::synchronize();

		groupsManager.applyGroups(getEntityMesh(), & meshModelBuilder, &objectManager, &materialManager, &annotationsManager);

		reportTime("\tTime: Gmsh model build completed", timer, properties.getVerbose());

		// -----------------------------------------------------------------------------------------
		// Apply the step with settings for the meshing
		// -----------------------------------------------------------------------------------------

		// Apply the basic volumetric mesh step settings if needed (either local for the shape or global)
		for (auto entity : meshModelBuilder.getModelEntities())
		{		
			stepWidthManager.applyVolumeMeshStepWidthToEntity(entity, objectManager, properties);
		}

		// Now we apply the curvature refinement to all faces
		stepWidthManager.applyCurvatureMeshStepWidthToEntities();

		// Now apply the user defined refinements for individual faces and refinement points
		stepWidthManager.applyUserDefinedRefinmentsToFacesAndPoints(properties.getRefinementStep(), properties.getRefinementList());

		if (stepWidthManager.hasAnyRefinementPoints())
		{
			// If we have refinement points, then we need to switch to the MeshAdapt algorithm
			gmsh::option::setNumber("Mesh.Algorithm", 1);
		}

		// Finally, we need to set all refinements for volumes, faces, edges and points in the mesh generator
		stepWidthManager.apply2DRefinementsToMesher(properties.getRefinementStep(), properties.getRefinementRadius());
		reportTime("\tTime: Gmsh mesh refinement settings completed", timer, properties.getVerbose());

		displayMessage("2/3: Generating surface mesh\n");
		setProgressInformation("Step 2/3: Generating surface mesh", false);
		setProgress(0);

		// -----------------------------------------------------------------------------------------
		// Perform the actual mesh generation
		// -----------------------------------------------------------------------------------------

		// Perform the 2D mesh generation 
		ProgressLogger logger(application);
		logger.startLogger2D(properties.getVerbose());

		try
		{
			gmsh::model::mesh::generate(2);
		}
		catch (int ierr)
		{
			throw std::string("ERROR in surface mesh generation. Error code: " + std::to_string(ierr) + "\n");
		}
		catch (std::string error)
		{
			throw std::string("ERROR in surface mesh generation. Error code: " + error + "\n");
		}
		catch (...)
		{
			throw std::string("ERROR in surface mesh generation. Error code: unknown\n");
		}

		logger.stopLogger2D();
		reportTime("\tTime: Gmsh surface meshing completed", timer, properties.getVerbose());

		// No we perform proximity meshing if active
		bool surfaceMeshIsValid = true;
		if (properties.getProximityMeshing() != 0)
		{
			surfaceMeshIsValid = proximityMeshing.fixMeshIntersections(getEntityMesh()->getName(), properties.getProximityMeshing(), &logger, &stepWidthManager, properties.getVerbose());
			reportTime("\tProximits meshing completed", timer,  properties.getVerbose());
		}

		if (surfaceMeshIsValid)
		{
			// Now we apply the volumetric refinements
			stepWidthManager.apply3DRefinementsToMesher(properties.getUseDistanceForVolumeMeshRefinement(), properties.getRefinementStep(), properties.getRefinementRadius());

			// Now we generate the volume mesh
			displayMessage("3/3: Generating volume mesh\n");

			if (properties.getMeshAlg3D() == 10)
			{
				// Use the HXT mesher (no progress information available)
				setProgressInformation("Step 3/3: Generating volume mesh", true);
			}
			else
			{
				// Use a conventional mesher (progress information available)
				setProgressInformation("Step 3/3: Generating volume mesh", false);
				setProgress(0);
			}

			logger.startLogger3D(properties.getVerbose());

			try
			{
				gmsh::model::mesh::generate(3);

				// Now we apply the element curvature
				if (properties.getElementOrder() > 1)
				{
					gmsh::model::mesh::setOrder(properties.getElementOrder());
				}
			}
			catch (int ierr)
			{
				displayMessage("ERROR in volume mesh generation. Error code: " + std::to_string(ierr) + "\n");
				logger.showLogMessages(false);
			}
			catch (std::string error)
			{
				if (properties.getMeshAlg3D() == 10) error += " - please consider refining the mesh or switching to the Delauney type volume mesher.";

				displayMessage("ERROR in volume mesh generation: " + error + "\n");
				logger.showLogMessages(false);
			}
			catch (...)
			{
				displayMessage("ERROR in volume mesh generation. Error code: unknown\n");
				logger.showLogMessages(false);
			}

			logger.stopLogger3D();

			reportTime("\tTime: Gmsh volume meshing completed", timer, properties.getVerbose());
		}

		// -----------------------------------------------------------------------------------------
		// Convert and store the results
		// -----------------------------------------------------------------------------------------

		setProgressInformation("Checking and storing mesh", true);

		// We now convert the mesh data from gmsh into internal data structures
		// First we convert the nodes and store them in the mesh entity
		meshWriter.convertAndStoreNodes();

		// In a next step, we convert all mesh faces and store them as triangle lists in the mesh entity
		meshWriter.convertFaces();

		reportTime("\tTime: Mesh nodes and faces converted", timer, properties.getVerbose());

		// In a next step, process each mesh entity and create a corresponding mesh item to store the surface mesh and volume mesh
		objectManager.writeMeshEntities(&meshWriter, &annotationsManager, &logger, entityMesh, materialsFolder, materialsFolderID);

		// Now store all mesh face entities
		meshWriter.storeFaces();
		reportTime("\tTime: Entity mesh information converted (face lists and volume meshes)", timer, properties.getVerbose());

		// Next we write the gmsh mesh file into the database
		meshWriter.storeMeshFile();

		// Now store potential error annotations from proximity meshing
		proximityMeshing.storeErrorAnnotations();

		// Now we analyze the error points and determine which faces they belong to
		meshWriter.analyzeErrorPoints(&boundingBox);

		displayMessage("\nTetrahedral mesh generation completed.\n\n"
					   "Mesh statistics:\n"
					   "  Number of nodes       :\t" + std::to_string(meshWriter.getNumberOfNodes()) + "\n"
					   "  Number of triangles   :\t" + std::to_string(meshWriter.getNumberOfTriangles()) + "\n"
					   "  Number of tetrahedrons:\t" + std::to_string(meshWriter.getNumberOfTets()) + "\n\n");

		meshWriter.displayShapesWithoutTets();

		entityMesh->setMeshValid(true);

		hideAllOtherEntities(getEntityMesh());
	}
	catch (std::string &error)
	{
		displayMessage(error);
	}
	catch (...)
	{
		displayMessage("ERROR: Unknown error occurred\n");
	}

	// Terminate the gmsh component
	gmsh::finalize();

	// Write the properties of the mesh data entity
	EntityPropertiesBoolean::createProperty("Mesh Visualization", "Show volume mesh", false, "", getEntityMesh()->getMeshData()->getProperties());

	EntityPropertiesString::createProperty("Mesh Dimensions", "Number of nodes", std::to_string(meshWriter.getNumberOfNodes()), "", getEntityMesh()->getMeshData()->getProperties());
	EntityPropertiesString::createProperty("Mesh Dimensions", "Number of triangles", std::to_string(meshWriter.getNumberOfTriangles()), "", getEntityMesh()->getMeshData()->getProperties());
	EntityPropertiesString::createProperty("Mesh Dimensions", "Number of tetrahedrons", std::to_string(meshWriter.getNumberOfTets()), "", getEntityMesh()->getMeshData()->getProperties());

	getEntityMesh()->getMeshData()->getProperties().getProperty("Number of nodes")->setReadOnly(true);
	getEntityMesh()->getMeshData()->getProperties().getProperty("Number of triangles")->setReadOnly(true);
	getEntityMesh()->getMeshData()->getProperties().getProperty("Number of tetrahedrons")->setReadOnly(true);

	// Store the mesh data entity
	getEntityMesh()->storeMeshData();
	application->getModelComponent()->addNewTopologyEntity(getEntityMesh()->getMeshData()->getEntityID(), 
														getEntityMesh()->getMeshData()->getEntityStorageVersion(), 
														false);

	// Get the information of all newly created entities as output dependency
	application->getModelComponent()->getListOfNewEntities(outputDependencyList);

	getEntityMesh()->storeToDataBase();
	application->getModelComponent()->addNewTopologyEntity(getEntityMesh()->getEntityID(), 
														getEntityMesh()->getEntityStorageVersion(), 
														false);

	// We release the mesh data from memory (since it is already stored in the data base)
	getEntityMesh()->releaseMeshData(); 

	// Turn off the write queue (this will store all entities in the data base)
	DataBase::instance().setWritingQueueEnabled(false);
	ot::ModelServiceAPI::enableMessageQueueing(false);

	reportTime("\tTime: Mesh items stored to data base", timer, properties.getVerbose());

	// Finally store newly created entities in the model. Due to building the hierarchy in the model service, this 
	// might update the entity versions. Therefore, we do not save the model here.
	application->getModelComponent()->storeNewEntities("intermediate store", false, false);
	ot::ModelServiceAPI::storeAllEntitiesToDataBase();  // Make sure that the modeler writes all entities to update their save versions before we get them here

	// Now update the versions of the input and output depencencies according to the current state
	application->getModelComponent()->updateEntityVersions(inputDependencyList);
	application->getModelComponent()->updateEntityVersions(outputDependencyList);

	// Now the mesh entity was stored and potentially modified by the model service when saving. Therefore we need to load the
	// mesh entity again here and just add the dependency information
	ot::EntityInformation updatedMeshEntityInfo;
	ot::ModelServiceAPI::getEntityInformation(getEntityMesh()->getName(), updatedMeshEntityInfo);

	EntityMesh* updatedMeshEntity = dynamic_cast<EntityMesh*>(ot::EntityAPI::readEntityFromEntityIDandVersion(updatedMeshEntityInfo.getEntityID(), updatedMeshEntityInfo.getEntityVersion()));
	assert(updatedMeshEntity != nullptr);

	if (updatedMeshEntity != nullptr)
	{
		updatedMeshEntity->setInputDependency(inputDependencyList);		// The mesh entity is part of the input dependency, but will be updated to the latest version while saving automatically.
		updatedMeshEntity->setOutputDependency(outputDependencyList);
		updatedMeshEntity->storeToDataBase();

		// Finally store the new model state (and save the updated mesh entity)
		ot::UIDList topologyEntityIDs = { updatedMeshEntity->getEntityID() };
		ot::UIDList topologyEntityVersions = { updatedMeshEntity->getEntityStorageVersion() };
		ot::ModelServiceAPI::updateTopologyEntities(topologyEntityIDs, topologyEntityVersions, "update tetrahedral mesh: " + mesh->getName(), false, true);
	}
	else
	{
		// Just save the new state
		ot::ModelServiceAPI::modelChangeOperationCompleted("update tetrahedral mesh: " + mesh->getName());
	}

	closeProgressInformation();
	setUILock(false, MODEL_CHANGE);
}

void GmshMeshCreation::deleteMesh(void)
{
	std::list<std::string> entityNameList;
	entityNameList.push_back(getEntityMesh()->getName() + "/Geometry");
	entityNameList.push_back(getEntityMesh()->getName() + "/Mesh");
	entityNameList.push_back(getEntityMesh()->getName() + "/Mesh Errors");

	ot::ModelServiceAPI::deleteEntitiesFromModel(entityNameList, false);

	getEntityMesh()->deleteMeshData();
}

std::list<ot::UID> GmshMeshCreation::getAllGeometryEntitiesForMeshing(void)
{
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	application->getModelComponent()->sendMessage(false, requestDoc, response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	
	std::list<ot::UID> entityIDs;
	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);

	return entityIDs;
}

void GmshMeshCreation::reportTime(const std::string &message, std::time_t &timer, bool verbose)
{
	double seconds = difftime(time(nullptr), timer);

	std::cout << message << ": " << seconds << " sec." << std::endl;

	if (verbose)
	{
		displayMessage(message + ": " + std::to_string(seconds) + " sec.\n");
	}

	timer = time(nullptr);
}

std::list<EntityGeometry *> GmshMeshCreation::loadGeometryEntitiesAndBreps(std::list<ot::UID> &geometryEntitiesID, std::list<std::pair<ot::UID, ot::UID>> &inputDependencyList)
{
	if (geometryEntitiesID.empty())
	{
		throw std::string("ERROR: Mesh cannot be created, because no shapes are selected for meshing.\n\n");
	}

	// Now load all geometry entities in the model
	application->prefetchDocumentsFromStorage(geometryEntitiesID);
	
	std::list<EntityGeometry *> geometryEntities;

	for (auto entityID : geometryEntitiesID)
	{
		ot::UID entityVersion = application->getPrefetchedEntityVersion(entityID);
		EntityGeometry *geom = dynamic_cast<EntityGeometry *>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion));

		inputDependencyList.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));

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

	// In the following we need the Brep description for each object.
	// Now loop through every geometry entity and check which data base objects will need to be loaded for pre-fetching
	std::list<ot::UID> prefetchIds;
	std::list<EntityBase *> allEntities;

	for (EntityGeometry *geomEntity : geometryEntities)
	{
		geomEntity->addPrefetchingRequirementsForBrep(prefetchIds);
		allEntities.push_back(geomEntity);
	}

	application->prefetchDocumentsFromStorage(prefetchIds);

	// Now load the breps and assign them to the geometry entities
	for (EntityGeometry *geomEntity : geometryEntities)
	{
		ot::UID brepID = geomEntity->getBrepStorageObjectID();
		ot::UID brepVersion = application->getPrefetchedEntityVersion(brepID);

		EntityBrep *brep = dynamic_cast<EntityBrep *>(ot::EntityAPI::readEntityFromEntityIDandVersion(brepID, brepVersion));

		inputDependencyList.push_back(std::pair<ot::UID, ot::UID>(brepID, brepVersion));

		geomEntity->setBrepEntity(brep);
	}

	return geometryEntities;
}

void GmshMeshCreation::setProgress(int percentage)
{
	assert(application != nullptr);

	if (application->getUiComponent())
	{
		application->getUiComponent()->setProgress(percentage);
	}
}

void GmshMeshCreation::displayMessage(std::string message)
{
	assert(application != nullptr);

	if (application->getUiComponent())
	{
		application->getUiComponent()->displayMessage(message);
	}
}

void GmshMeshCreation::setProgressInformation(std::string message, bool continuous)
{
	assert(application != nullptr);

	if (application->getUiComponent())
	{
		application->getUiComponent()->setProgressInformation(message, continuous);
	}
}

void GmshMeshCreation::closeProgressInformation(void)
{
	assert(application != nullptr);

	if (application->getUiComponent())
	{
		application->getUiComponent()->closeProgressInformation();
	}
}

void GmshMeshCreation::setUILock(bool flag, lockType type)
{
	assert(application != nullptr);

	if (application->getUiComponent())
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
					f.set(ot::LockType::All);
					break;
				case MODEL_CHANGE:
					f.set(ot::LockType::ModelWrite);
					f.set(ot::LockType::NavigationWrite);
					f.set(ot::LockType::ViewWrite);
					f.set(ot::LockType::Properties);
					break;
				default:
					assert(0); // Unknown lock type
					f.set(ot::LockType::All);
				}

				application->getUiComponent()->lockUI(f);
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
						f.set(ot::LockType::All);
						break;
					case MODEL_CHANGE:
						f.set(ot::LockType::ModelWrite);
						f.set(ot::LockType::NavigationWrite);
						f.set(ot::LockType::ViewWrite);
						f.set(ot::LockType::Properties);
						break;
					default:
						assert(0); // Unknown lock type
						f.set(ot::LockType::All);
					}

					application->getUiComponent()->unlockUI(f);
				}
			}
		}
	}
}

void GmshMeshCreation::hideAllOtherEntities(EntityMeshTet *thisMesh)
{
	ot::JsonDocument docHideGeometry;
	docHideGeometry.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_HideBranch, docHideGeometry.GetAllocator()), docHideGeometry.GetAllocator());
	docHideGeometry.AddMember(OT_ACTION_PARAM_MODEL_ID, ot::ModelServiceAPI::getCurrentVisualizationModelID(), docHideGeometry.GetAllocator());
	docHideGeometry.AddMember(OT_ACTION_PARAM_MODEL_ITM_BRANCH, ot::JsonString("Geometry", docHideGeometry.GetAllocator()), docHideGeometry.GetAllocator());

	std::string tmp;
	application->getUiComponent()->sendMessage(true, docHideGeometry, tmp);

	ot::JsonDocument docHideMeshes;
	docHideMeshes.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_HideBranch, docHideGeometry.GetAllocator()), docHideGeometry.GetAllocator());
	docHideMeshes.AddMember(OT_ACTION_PARAM_MODEL_ID, ot::ModelServiceAPI::getCurrentVisualizationModelID(), docHideGeometry.GetAllocator());
	docHideMeshes.AddMember(OT_ACTION_PARAM_MODEL_ITM_BRANCH, ot::JsonString("Meshes", docHideGeometry.GetAllocator()), docHideGeometry.GetAllocator());

	application->getUiComponent()->sendMessage(true, docHideMeshes, tmp);

	ot::JsonDocument docShowMesh;
	docShowMesh.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_ShowBranch, docHideGeometry.GetAllocator()), docHideGeometry.GetAllocator());
	docShowMesh.AddMember(OT_ACTION_PARAM_MODEL_ID, ot::ModelServiceAPI::getCurrentVisualizationModelID(), docHideGeometry.GetAllocator());
	docShowMesh.AddMember(OT_ACTION_PARAM_MODEL_ITM_BRANCH, ot::JsonString(thisMesh->getName(), docHideGeometry.GetAllocator()), docHideGeometry.GetAllocator());

	application->getUiComponent()->sendMessage(true, docShowMesh, tmp);
}

bool GmshMeshCreation::meshNeedsUpdating(EntityMeshTet* mesh, const std::list<std::pair<ot::UID, ot::UID>> &currentInputDependencyList)
{
	// First, we need to check whether the new check has delivered the same dependencies as the ones which are already stored.
	std::list<std::pair<ot::UID, ot::UID>> previousInputDependencyList;
	mesh->getInputDependency(previousInputDependencyList);

	if (currentInputDependencyList.size() != previousInputDependencyList.size()) return true; // The number of dependencies is different -> mesh needs to be updated

	// Now we have the same count of dependencies in both lists. We just need to check if all entires from one list are also contained in the other list.
	std::set<std::pair<ot::UID, ot::UID>> currentInputDependencySet;
	for (auto entity : currentInputDependencyList)
	{
		currentInputDependencySet.emplace(entity);
	}

	for (auto entity : previousInputDependencyList)
	{
		if (currentInputDependencySet.find(entity) == currentInputDependencySet.end())
		{
			// This item is not present in the list. Therefore the lists are not identical and need to be updated
			return true;
		}
	}

	// So far, all input dependencies still seem to be the same. Check whether the previous output dependencies are still present.
	std::list<std::pair<ot::UID, ot::UID>> previousOutputDependencyList;
	mesh->getOutputDependency(previousOutputDependencyList);

	std::list<ot::UID> outputIDList;
	std::map<ot::UID, ot::UID> outputEntityMap;

	for (auto entity : previousOutputDependencyList)
	{
		outputIDList.push_back(entity.first);
		outputEntityMap[entity.first] = entity.second;
	}

	std::list<ot::EntityInformation> entityInfo;
	ot::ModelServiceAPI::getEntityInformation(outputIDList, entityInfo);

	for (auto entity : entityInfo)
	{
		if (entity.getEntityID() == ot::getInvalidUID())
		{
			// The entity is not longer available
			return false;
		}

		if (outputEntityMap[entity.getEntityID()] != entity.getEntityVersion())
		{
			// The version of the output entity has changed or the output entity is no longer present
			return true;
		}
	}

	// The input are unchanged and all output dependencies are still there, no update necessary
	return false;
}