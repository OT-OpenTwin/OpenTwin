// @otlicense
// File: ModelBuilder.cpp
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

#include "ModelBuilder.h"

#include "Properties.h"
#include "StepWidthManager.h"
#include "MaterialManager.h"
#include "Application.h"

#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/GeometryOperations.h"
#include "OTModelEntities/Database.h"
#include "OTModelEntities/EntityMaterial.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/EntityAnnotation.h"
#include "OTModelEntities/EntityAnnotationData.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <TopLoc_Location.hxx>
#include <TopAbs_Orientation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <TopAbs_State.hxx>
#include <BRepLProp_SLProps.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <gp_Vec.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

ModelBuilder::~ModelBuilder()
{
	// Delete all the geometry entities
	for (auto entity : modelEntities)
	{
		delete entity;
	}

	modelEntities.clear();
	meshPriorities.clear();
	parentShapeName.clear();
	allShapeOverlaps.clear();
}

void ModelBuilder::buildModel(const std::string &meshName, std::list<EntityGeometry *> &geometryEntities, Properties &properties, StepWidthManager &stepWidthManager, MaterialManager &materialManager)
{
	// Check for materials if necessary
	if (properties.getCheckMaterial())
	{
		std::string error = checkMaterialAssignmentForShapes(geometryEntities);
		if (!error.empty()) throw error;

		error = checkMaterialAssignmentForBoundingSphere(properties);
		if (!error.empty()) throw error;
	}

	// Add bounding sphere if requested
	if (properties.getBoundingSphereRadius() > 0.0)
	{
		std::string error = addBoundingSphere(geometryEntities, properties, stepWidthManager);
		if (!error.empty()) throw error;
	}

	// Build the non-manifold model for meshing
	buildNonManifoldModel(meshName, geometryEntities, properties, materialManager, stepWidthManager);
}

std::string ModelBuilder::checkMaterialAssignmentForShapes(std::list<EntityGeometry *> &geometryEntities)
{
	std::string message;

	for (auto entity : geometryEntities)
	{
		EntityPropertiesEntityList *material = dynamic_cast<EntityPropertiesEntityList*>(entity->getProperties().getProperty("Material"));

		bool materialError = (material == nullptr);

		if (!materialError)
		{
			if (application->getModelComponent()->getCurrentMaterialName(material).empty())
			{
				materialError = true;
			}
		}

		if (materialError)
		{
			message += "\n     " + entity->getName();
		}
	}

	if (!message.empty())
	{
		message = "\nERROR: No material has been defined for the following shapes:" + message + "\n";
	}

	return message;
}

std::string ModelBuilder::checkMaterialAssignmentForBoundingSphere(Properties &properties)
{
	std::string message;

	if (properties.getBoundingSphereRadius() > 0.0)
	{
		std::string material;

		if (properties.getBoundingSphereMaterial() != nullptr)
		{
			material = application->getModelComponent()->getCurrentMaterialName(properties.getBoundingSphereMaterial());
		}

		if (material.empty())
		{
			message = "\nERROR: No material has been assigned to the bounding sphere\n";
		}
	}

	return message;
}

std::string ModelBuilder::addBoundingSphere(std::list<EntityGeometry *> &geometryEntities, Properties &properties, StepWidthManager &stepWidthManager)
{
	std::string error;

	// Determine bounding box of geometry
	BoundingBox boundingBox = ot::GeometryOperations::getBoundingBox(geometryEntities);

	// Determine the sphere radius (consider whether the radius is specified in absolute or relative dimension)
	double sphereRadius = properties.getBoundingSphereRadius();

	if (!properties.getBoundingSphereAbsolute())
	{
		sphereRadius *= 0.5 * boundingBox.getDiagonal();
	}

	double minRadius = 1.1 * 0.5 * boundingBox.getDiagonal();  // The minimum radius is 10% larger than the bounding box radius

	if (sphereRadius < minRadius)
	{
		if (properties.getBoundingSphereAbsolute())
		{
			error = "ERROR: The radius of the bounding sphere is too small (minimum absolute radius is " + std::to_string(minRadius) + ")\n";
		}
		else
		{
			error = "ERROR: The radius of the bounding sphere is too small (minimum relative radius is " + std::to_string(2.0 * minRadius / boundingBox.getDiagonal()) + ")\n";
		}

		return error;
	}

	// Determine center of the sphere
	double xc{ 0.0 }, yc{ 0.0 }, zc{ 0.0 };
	boundingBox.getCenterPoint(xc, yc, zc);

	// Now we create the sphere 
	BRepPrimAPI_MakeSphere sphere(gp_Pnt(xc, yc, zc), sphereRadius);

	// Get the default color for the background sphere
	int valueR = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("Model", "Background sphere color", 0, 204);
	int valueG = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("Model", "Background sphere color", 1, 204);
	int valueB = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor("Model", "Background sphere color", 2, 204);

	// Build the geometry entity
	EntityGeometry *backgroundSphere = new EntityGeometry(0, nullptr, nullptr, nullptr);

	backgroundSphere->setName("Geometry/Background");
	backgroundSphere->setBrep(sphere.Shape());

	EntityPropertiesDouble::createProperty( "Mesh",		"Mesh priority",											  -1e30, "", backgroundSphere->getProperties());
	EntityPropertiesDouble::createProperty( "Mesh",		"Maximum edge length", stepWidthManager.getBoundingSphereMeshStep(), "", backgroundSphere->getProperties());
	EntityPropertiesBoolean::createProperty("Mesh",		"Curvature refinement",										  false, "", backgroundSphere->getProperties());
	EntityPropertiesColor::createProperty(  "Appearance", "Color",								 { valueR, valueG, valueB }, "", backgroundSphere->getProperties());

	if (properties.getBoundingSphereMaterial() != nullptr)
	{
		EntityPropertiesEntityList *material = new EntityPropertiesEntityList(*properties.getBoundingSphereMaterial());
		material->setName("Material");

		backgroundSphere->getProperties().createProperty(material, "Solver");
	}

	EntityPropertiesSelection::createProperty("Mesh", "Mesh refinement", { "Global setting", "Local setting" }, "Local setting", "", backgroundSphere->getProperties());

	geometryEntities.push_back(backgroundSphere);

	return error;
}

void ModelBuilder::buildNonManifoldModel(const std::string &meshName, std::list<EntityGeometry *> &geometryEntities, Properties &properties, MaterialManager &materialManager, StepWidthManager& stepWidthManager)
{
	// First of all, perform a non-reg unite operation on all shapes
	TopTools_ListOfShape inputs;
	for (auto entity : geometryEntities)
	{
		inputs.Append(entity->getBrep());
	}

	BRepAlgoAPI_BuilderAlgo booleanOperation;

	booleanOperation.SetRunParallel(Standard_False);
	if (properties.getTolerance() > 0.0) booleanOperation.SetFuzzyValue(properties.getTolerance());

	booleanOperation.SetArguments(inputs);
	booleanOperation.Build();

	// Next, create geometry entities for all new (merged) shapes

	for (auto ent : geometryEntities)
	{
		if (!booleanOperation.IsDeleted(ent->getBrep()))
		{
			// This shape still exists after the unite operation

			// Determine the mesh priority of the shape
			double meshPriority = getMeshPriority(ent, materialManager);

			createAllShapes(booleanOperation, meshName, ent, meshPriority);
		}
	}

	// Analyze shape overlaps
	analyzeOverlaps();

	// Resolve overlaps according to priorities
	if (properties.getUsePriorities())
	{
		resolveOverlaps();

		// Now, the entity list contains all shapes which are still present in the model where potential 
		// overlaps have been resolved according to the priorities.
		if (properties.getMergeShapes())
		{
			mergeShapesOfSameParent();
		}

		// Finally, we check whether a shape has one child only. In this case, the name of the shape is reset to the original parent name
		removeUnnecessaryHierarchies();
	}

	// Check all shapes for their ability to be meshed (e.g. check for near conincident faces)
	checkShapesForMeshing(meshName, stepWidthManager.getMaximumEdgeLength() * 0.1);

	// Now store all the newly created mesh model entities
	storeEntities();

	// Check whether any unresolved overlaps are remaining
	if (allShapeOverlaps.size() > 0)
	{
		std::string message = "ERROR: Unresolved shape overlaps detected: \n";

		// Warn for shape overlaps
		for (auto overlap : allShapeOverlaps)
		{
			std::string line;
			for (auto shape : overlap)
			{
				if (!line.empty()) line += ", ";
				line += shape;
			}
			message += "  " + line + "\n";
		}

		throw message;
	}
}

void ModelBuilder::checkShapesForMeshing(const std::string& meshName, double gapTolerance)
{
	for (auto geometryEntity : modelEntities)
	{
		double minEdgeLength = 1.0e-9;
		double minFaceArea = 1.0e-18;
		double minVolume = 1.0e-27;

		auto report = checkShapeForVolumeMeshing(geometryEntity->getBrep(), minEdgeLength, minFaceArea, minVolume, gapTolerance);

		std::cout << report.toString() << std::endl;

		if (!report.meshable)
		{
			EntityAnnotation* annotation = new EntityAnnotation(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);

			for (const auto& p : report.diagnosticPoints)
			{
				const gp_Pnt& q = p.point;

				annotation->addPoint(q.X(), q.Y(), q.Z(), 1.0, 0.0, 0.0);
			}

			for (const auto& pair : report.coincidentFaces)
			{
				extractFaceTriangles(pair.first, annotation, 1.0, 0.0, 0.0);
				extractFaceTriangles(pair.second, annotation, 1.0, 0.5, 0.0);
			}

			visualizeThinGapFaces(report, annotation);

			annotation->setName(meshName + "/Mesh Warnings/" + geometryEntity->getName());
			annotation->setInitiallyHidden(true);

			annotation->storeToDataBase();
			application->getModelComponent()->addNewTopologyEntity(annotation->getEntityID(), annotation->getEntityStorageVersion(), true);
			application->getModelComponent()->addNewDataEntity(annotation->getAnnotationData()->getEntityID(), annotation->getAnnotationData()->getEntityStorageVersion(), annotation->getEntityID());

			ot::StyledTextBuilder message;
			message << "\n[" << ot::StyledText::Warning << ot::StyledText::Bold << "WARNING" << ot::StyledText::ClearStyle << "] "
				<< geometryEntity->getName() << ":\n";
			
			for (auto msg : report.messages)
			{
				message << msg << "\n";
			}

			Application::instance()->getUiComponent()->displayStyledMessage(message);
		}
	}
}

double ModelBuilder::getMeshPriority(EntityBase *entity, MaterialManager &materialManager)
{
	double meshPriority = 0.0;

	// First consider the shape property
	EntityPropertiesDouble *meshPriorityProperty = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Mesh priority"));
	if (meshPriorityProperty != nullptr) meshPriority = meshPriorityProperty->getValue();

	// Now we add the material priority (if any)
	EntityPropertiesEntityList *materialProperty = dynamic_cast<EntityPropertiesEntityList*>(entity->getProperties().getProperty("Material"));
	if (materialProperty != nullptr)
	{
		EntityMaterial *material = materialManager.getMaterial(application->getModelComponent()->getCurrentMaterialName(materialProperty));
		if (material != nullptr)
		{
			EntityPropertiesDouble *meshMaterialPriorityProperty = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Mesh priority"));
			if (meshMaterialPriorityProperty != nullptr)
			{
				meshPriority += meshMaterialPriorityProperty->getValue();
			}
		}
	}

	return meshPriority;
}

void ModelBuilder::createAllShapes(BRepAlgoAPI_BuilderAlgo &booleanOperation, const std::string &meshName, EntityGeometry *entity, double meshPriority)
{

	// The shape still exists -> Build a list of all shapes which belong to the current shape
	TopTools_ListOfShape allShapes;

	TopTools_ListOfShape generatedShapes = booleanOperation.Generated(entity->getBrep());
	TopTools_ListOfShape modifiedShapes  = booleanOperation.Modified(entity->getBrep());

	ot::PropertyGridCfg cfg;

	entity->getProperties().addToConfiguration(nullptr, false, cfg);

	allShapes.Append(generatedShapes);

	if (modifiedShapes.Size() > 0)
	{
		allShapes.Append(modifiedShapes);
	}
	else
	{
		allShapes.Append(entity->getBrep());
	}

	if (allShapes.Size() > 1)
	{
		int count = 1;

		for (auto body : allShapes)
		{
			std::string shapeName = meshName + "/" + entity->getName() + "/" + std::to_string(count);
			count++;

			modelEntities.push_back(createGeometryEntity(shapeName, body, cfg));

			meshPriorities[shapeName] = meshPriority;
			parentShapeName[shapeName] = meshName + "/" + entity->getName();
		}
	}
	else
	{
		std::string shapeName = meshName + "/" + entity->getName();

		modelEntities.push_back(createGeometryEntity(shapeName, allShapes.First(), cfg));

		meshPriorities[shapeName] = meshPriority;
		parentShapeName[shapeName] = shapeName;
	}
}

EntityGeometry *ModelBuilder::createGeometryEntity(const std::string &name, TopoDS_Shape &shape, const ot::PropertyGridCfg& shapeProperties)
{
	EntityGeometry *entityGeom = new EntityGeometry(0, nullptr, nullptr, nullptr);

	entityGeom->setName(name);
	entityGeom->setTreeItemEditable(true);
	entityGeom->setBrep(shape);
	entityGeom->setInitiallyHidden(true);
	entityGeom->registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify,
		application->getServiceName()
	);

	entityGeom->getProperties().buildFromConfiguration(shapeProperties, nullptr, EntityProperties::All);
	entityGeom->getProperties().forceResetUpdateForAllProperties();

	entityGeom->getProperties().setAllPropertiesReadOnly();

	return entityGeom;
}

void ModelBuilder::storeEntities(void)
{
	DataBase::instance().setWritingQueueEnabled(true);

	for (auto geometryEntity : modelEntities)
	{
		application->getModelComponent()->facetAndStoreGeometryEntity(geometryEntity, false);
	}

	DataBase::instance().setWritingQueueEnabled(false);
}

void ModelBuilder::analyzeOverlaps(void)
{
	// Analyze overlaps
	std::map<std::string, std::vector<std::string>> allShapeFaces;

	for (auto entity : modelEntities)
	{
		TopoDS_Shape &shape = entity->getBrep();

		std::vector<TopoDS_TShape*> tFaceList;

		for (TopExp_Explorer aExpFace(shape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
		{
			const TopoDS_Face &aFace = TopoDS::Face(aExpFace.Current());
			tFaceList.push_back(aFace.TShape().get());
		}

		// Now sort the vector
		std::sort(tFaceList.begin(), tFaceList.end());

		// And write everything into a string
		std::stringstream faceString;
		for (auto ptr : tFaceList) faceString << ptr << " ";

		// Build a string
		std::string key = faceString.str();

		// Store the result in the map
		std::vector<std::string> names;
		if (allShapeFaces.count(key) > 0) names = allShapeFaces[key];
		names.push_back(entity->getName());
		allShapeFaces[key] = names;
	}

	for (std::map<std::string, std::vector<std::string>>::iterator it = allShapeFaces.begin(); it != allShapeFaces.end(); it++)
	{
		if (it->second.size() > 1)
		{
			allShapeOverlaps.push_back(it->second);
		}
	}
}

void ModelBuilder::resolveOverlaps(void)
{
	std::list<std::vector<std::string>> remainingOverlaps;
	std::list<std::string> shapesToDelete;

	for (auto overlap : allShapeOverlaps)
	{
		// Search for the maximum priority
		double maxPrio = meshPriorities[overlap.front()];
		for (auto shape : overlap) if (meshPriorities[shape] > maxPrio) maxPrio = meshPriorities[shape];

		// Mark all shapes with lower priority for delete
		std::vector<std::string> overlapShapes;

		for (auto shape : overlap)
		{
			if (meshPriorities[shape] < maxPrio)
			{
				// Mark shape for delete
				shapesToDelete.push_back(shape);
			}
			else
			{
				// Keep shape
				overlapShapes.push_back(shape);
			}
		}

		// If we still have an overlap, store it
		if (overlapShapes.size() > 1)
		{
			remainingOverlaps.push_back(overlapShapes);
		}
	}

	// Store the remaining overlaps
	allShapeOverlaps = remainingOverlaps;

	// Delete the shapes
	for (auto delShape : shapesToDelete)
	{
		for (auto entity : modelEntities)
		{
			if (entity->getName() == delShape)
			{
				// we need to delete this entity
				modelEntities.remove(entity);
				delete entity;

				break;
			}
		}
	}
}

void ModelBuilder::removeUnnecessaryHierarchies(void)
{
	// Get a map with all shape names in the entitiy list
	std::map<std::string, EntityGeometry *> shapesInEntityList;
	for (auto entity : modelEntities) shapesInEntityList[entity->getName()] = entity;

	// Now we build a map with all parent shapes containg a list of their childs. Here we consider only children which are not 
	// involved in an overlap. The shapes in this list could then be potentially be merged together again.
	std::map<std::string, std::vector<std::string>> childrenList;

	for (std::map<std::string, std::string>::iterator it = parentShapeName.begin(); it != parentShapeName.end(); it++)
	{
		if (shapesInEntityList.count(it->first) > 0)
		{
			// The current shape is not involved in an overlap and still exists in the entityList
			childrenList[it->second].push_back(it->first);
		}
	}

	// Now loop through all entites and check whether there is an entity with only one child
	for (std::map<std::string, std::vector<std::string>>::iterator it = childrenList.begin(); it != childrenList.end(); it++)
	{
		if (it->second.size() == 1)
		{
			// In this case, we can rename the entity back to its parent name
			shapesInEntityList[it->second.front()]->setName(it->first);
		}
	}
}

void ModelBuilder::mergeShapesOfSameParent(void)
{
	// Get a map with all shape names in the entitiy list
	std::map<std::string, EntityGeometry *> shapesInEntityList;
	for (auto entity : modelEntities) shapesInEntityList[entity->getName()] = entity;

	// First, we get a map of all shapes which are involved in an overlap
	std::map<std::string, bool> shapeInvolvedInOverlap;
	for (auto &shapeList : allShapeOverlaps)
	{
		for (auto &shape : shapeList)
		{
			shapeInvolvedInOverlap[shape] = true;
		}
	}

	// Now we build a map with all parent shapes containg a list of their childs. Here we consider only children which are not 
	// involved in an overlap. The shapes in this list could then be potentially be merged together again.
	std::map<std::string, std::vector<std::string>> childrenList;

	for (std::map<std::string, std::string>::iterator it = parentShapeName.begin(); it != parentShapeName.end(); it++)
	{
		if (shapeInvolvedInOverlap.count(it->first) == 0 && shapesInEntityList.count(it->first) > 0)
		{
			// The current shape is not involved in an overlap and still exists in the entityList
			childrenList[it->second].push_back(it->first);
		}
	}

	// We now check whether children of a parent shape have a common TFACE. If so, these shapes are merged by using a general fuse operation.

	std::list<EntityGeometry *> remainingGeometryEntities;

	for (std::map<std::string, std::vector<std::string>>::iterator it = childrenList.begin(); it != childrenList.end(); it++)
	{
		std::vector<std::string> childList = it->second;

		// Merge children pairwise as long as possible
		while (mergeChildren(childList, shapesInEntityList));

		// Now the child list is reduced and contains only the remaining children -> store them in the list of remaining entities
		it->second = childList;

		for (auto child : childList) remainingGeometryEntities.push_back(shapesInEntityList[child]);
	}

	// The shapes which are involved in an overlap have not been considered for the merging, but they are still in the model
	// Therefore, these overlap shapes need to be added to the list again

	for (std::map<std::string, bool>::iterator it = shapeInvolvedInOverlap.begin(); it != shapeInvolvedInOverlap.end(); it++)
	{
		if (it->second)
		{
			remainingGeometryEntities.push_back(shapesInEntityList[it->first]);
		}
	}

	// Now update the list
	modelEntities = remainingGeometryEntities;
}

bool ModelBuilder::mergeChildren(std::vector<std::string> &childList, std::map<std::string, EntityGeometry *> &shapesInEntityList)
{
	if (childList.size() <= 1) return false;

	// We have more than one child. We first build a map of TFACE pointing to the list of entities which use them

	std::map< TopoDS_TShape*, std::list<std::string>> tFaceMap;

	for (auto child : childList)
	{
		EntityGeometry *entity = shapesInEntityList[child];

		TopoDS_Shape &shape = entity->getBrep();

		for (TopExp_Explorer aExpFace(shape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
		{
			const TopoDS_Face &aFace = TopoDS::Face(aExpFace.Current());
			tFaceMap[aFace.TShape().get()].push_back(child);
		}
	}

	// Now check whether there is a TFACE shared between to shapes
	std::string name1, name2;

	for (std::map< TopoDS_TShape*, std::list<std::string>>::iterator it = tFaceMap.begin(); it != tFaceMap.end(); it++)
	{
		if (it->second.size() > 1)
		{
			// We have a shared TFACE
			assert(it->second.size() == 2);

			name1 = it->second.front();
			name2 = it->second.back();

			break;
		}
	}

	// Here we may hame names on name1, name2. If so, these shapes need to be merged
	bool mergePerformed = false;

	if (!name1.empty() && !name2.empty())
	{
		EntityGeometry *entity1 = shapesInEntityList[name1];
		EntityGeometry *entity2 = shapesInEntityList[name2];

		BRepAlgoAPI_Fuse fuseOperation(entity1->getBrep(), entity2->getBrep());
		fuseOperation.Build();

		entity1->setBrep(fuseOperation.Shape());

		mergePerformed = true;
	}

	// Now we need to delete the shape with name2 from the childList, the shapesInEntityList and delete the item itself.
	if (!name2.empty())
	{
		EntityGeometry *entity = shapesInEntityList[name2];

		for (std::vector<std::string>::iterator it = childList.begin(); it != childList.end(); it++)
		{
			if (*it == name2)
			{
				childList.erase(it);
				break;
			}
		}

		delete entity;
		shapesInEntityList.erase(name2);
	}

	return mergePerformed;
}

std::string ModelBuilder::ShapeCheckReport::toString() const
{
	std::ostringstream os;

	os << "Shape check report\n";
	os << "------------------\n";
	os << "OCC valid:        " << occValid << "\n";
	os << "Has solid:        " << hasSolid << "\n";
	os << "Positive volume:  " << hasPositiveVolume << "\n";
	os << "Closed:           " << closed << "\n";
	os << "Manifold:         " << manifold << "\n";
	os << "Meshable:         " << meshable << "\n";
	os << "Total volume:     " << totalVolume << "\n\n";

	os << "Invalid subshapes:       " << invalidSubShapes.size() << "\n";
	os << "Invalid faces:           " << invalidFaces.size() << "\n";
	os << "Invalid solids:          " << invalidSolids.size() << "\n";
	os << "Free edges:              " << freeEdges.size() << "\n";
	os << "Isolated edges:          " << isolatedEdges.size() << "\n";
	os << "Non-manifold edges:      " << nonManifoldEdges.size() << "\n";
	os << "Degenerated edges:       " << degeneratedEdges.size() << "\n";
	os << "Too short edges:         " << tooShortEdges.size() << "\n";
	os << "Too small faces:         " << tooSmallFaces.size() << "\n";
	os << "Zero/negative solids:    " << negativeOrZeroVolumeSolids.size() << "\n";

	for (const auto& msg : messages)
		os << "- " << msg << "\n";

	return os.str();
}

ModelBuilder::ShapeCheckReport ModelBuilder::checkShapeForVolumeMeshing(
	const TopoDS_Shape& shape,
	double minEdgeLength,
	double minFaceArea,
	double minVolume,
	double gapTolerance,
	bool checkGeometry) const
{
	ShapeCheckReport report;

	collectOccInvalidSubShapes(shape, report, checkGeometry);
	checkEdgeIncidence(shape, report);
	checkDegeneratedAndSmallEdges(shape, report, minEdgeLength);
	checkSmallFaces(shape, report, minFaceArea);
	checkSolidsAndVolumes(shape, report, minVolume);
	checkCoincidentFaces(
		shape,
		report,
		1e-8,     // distance tolerance
		1e-16,    // area tolerance
		5,        // samples per direction
		4);       // required interior hits
	checkThinGaps(
		shape,
		report,
		gapTolerance,   // gap tolerance
		1.0e-12,  // area tolerance
		9,        // samples per direction
		0.30,     // minimum hit ratio
		5.0);     // maximum distance variation

	report.meshable =
		report.occValid &&
		report.hasSolid &&
		report.hasPositiveVolume &&
		report.closed &&
		report.manifold &&
		report.freeEdges.empty() &&
		report.isolatedEdges.empty() &&
		report.nonManifoldEdges.empty() &&
		report.invalidFaces.empty() &&
		report.invalidSolids.empty() &&
		report.negativeOrZeroVolumeSolids.empty() &&
		report.coincidentFaces.empty() &&
		report.thinGapFaces.empty();

	if (!report.occValid)
		report.messages.push_back("Shape is not valid according to BRepCheck_Analyzer.");

	if (!report.closed)
		report.messages.push_back("Shape is not closed. Free or isolated edges were found.");

	if (!report.manifold)
		report.messages.push_back("Shape is non-manifold. At least one edge has more than two adjacent faces.");

	if (!report.hasSolid)
		report.messages.push_back("Shape does not contain any solid.");

	if (!report.hasPositiveVolume)
		report.messages.push_back("Shape has no positive volume.");

	collectDiagnosticPoints(report);

	return report;
}

void ModelBuilder::collectOccInvalidSubShapes(
	const TopoDS_Shape& shape,
	ShapeCheckReport& report,
	bool checkGeometry) const
{
	BRepCheck_Analyzer analyzer(shape, checkGeometry);
	report.occValid = analyzer.IsValid();

	const TopAbs_ShapeEnum types[] = {
		TopAbs_VERTEX,
		TopAbs_EDGE,
		TopAbs_WIRE,
		TopAbs_FACE,
		TopAbs_SHELL,
		TopAbs_SOLID,
		TopAbs_COMPSOLID,
		TopAbs_COMPOUND
	};

	for (TopAbs_ShapeEnum type : types)
	{
		for (TopExp_Explorer ex(shape, type); ex.More(); ex.Next())
		{
			const TopoDS_Shape& subShape = ex.Current();

			if (analyzer.IsValid(subShape))
				continue;

			report.invalidSubShapes.push_back(subShape);

			if (type == TopAbs_FACE)
				report.invalidFaces.push_back(TopoDS::Face(subShape));

			if (type == TopAbs_SOLID)
				report.invalidSolids.push_back(TopoDS::Solid(subShape));

			Handle(BRepCheck_Result) result = analyzer.Result(subShape);

			if (!result.IsNull())
			{
				const BRepCheck_ListOfStatus& statuses = result->Status();

				for (BRepCheck_ListIteratorOfListOfStatus it(statuses); it.More(); it.Next())
				{
					std::string statusName = brepCheckStatusToString(static_cast<int>(it.Value()));

					if (statusName != "NoError")
						report.messages.push_back("OCC check: " + statusName);
				}
			}
		}
	}
}

void ModelBuilder::checkEdgeIncidence(
	const TopoDS_Shape& shape,
	ShapeCheckReport& report) const
{
	TopTools_IndexedDataMapOfShapeListOfShape edgeToFaces;
	TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, edgeToFaces);

	for (int i = 1; i <= edgeToFaces.Extent(); ++i)
	{
		const TopoDS_Edge& edge = TopoDS::Edge(edgeToFaces.FindKey(i));
		const TopTools_ListOfShape& faces = edgeToFaces.FindFromIndex(i);

		const int numberOfAdjacentFaces = faces.Extent();
		const bool isDegenerated = BRep_Tool::Degenerated(edge);

		if (numberOfAdjacentFaces == 0)
		{
			if (isDegenerated)
			{
				// Degenerated pole edges can appear on spherical, conical, or periodic surfaces.
				// They do not necessarily indicate an open boundary.
				continue;
			}

			report.isolatedEdges.push_back(edge);
			report.closed = false;
			report.manifold = false;
		}
		else if (numberOfAdjacentFaces == 1)
		{
			if (isDegenerated)
			{
				// A degenerated edge with one adjacent face is usually a collapsed parametric boundary.
				// Do not classify it as a free edge.
				continue;
			}

			report.freeEdges.push_back(edge);
			report.closed = false;
		}
		else if (numberOfAdjacentFaces > 2)
		{
			if (isDegenerated)
			{
				// Degenerated edges may have unusual topological incidence.
				// Treat them as tolerated unless you want a strict B-Rep check mode.
				continue;
			}

			if (isToleratedTouchingEdge(edge, faces))
			{
				// Two solids touch along this edge.
				// This is non-manifold in a strict sense, but accepted for this workflow.
				continue;
			}

			report.nonManifoldEdges.push_back(edge);
			report.manifold = false;
		}
	}
}

void ModelBuilder::checkDegeneratedAndSmallEdges(
	const TopoDS_Shape& shape,
	ShapeCheckReport& report,
	double minEdgeLength) const
{
	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());

		if (BRep_Tool::Degenerated(edge))
		{
			// Degenerated edges are common and valid on spherical or conical poles.
			// Keep them out of the error list.
			//
			// Optional:
			// report.toleratedDegeneratedEdges.push_back(edge);
			continue;
		}

		const double length = edgeLength(edge);

		if (length > 0.0 && length < minEdgeLength)
			report.tooShortEdges.push_back(edge);
	}
}

void ModelBuilder::checkSmallFaces(
	const TopoDS_Shape& shape,
	ShapeCheckReport& report,
	double minFaceArea) const
{
	for (TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next())
	{
		const TopoDS_Face& face = TopoDS::Face(ex.Current());
		const double area = faceArea(face);

		if (area > 0.0 && area < minFaceArea)
			report.tooSmallFaces.push_back(face);
	}
}

void ModelBuilder::checkSolidsAndVolumes(
	const TopoDS_Shape& shape,
	ShapeCheckReport& report,
	double minVolume) const
{
	TopTools_IndexedMapOfShape solids;
	TopExp::MapShapes(shape, TopAbs_SOLID, solids);

	report.hasSolid = solids.Extent() > 0;

	double totalVolume = 0.0;

	for (int i = 1; i <= solids.Extent(); ++i)
	{
		const TopoDS_Solid& solid = TopoDS::Solid(solids(i));
		const double volume = solidVolume(solid);

		totalVolume += volume;

		if (volume <= minVolume)
			report.negativeOrZeroVolumeSolids.push_back(solid);
	}

	report.totalVolume = totalVolume;
	report.hasPositiveVolume = totalVolume > minVolume;
}

bool boxesOverlapWithTolerance(
	const Bnd_Box& a,
	const Bnd_Box& b,
	double tolerance)
{
	Bnd_Box aa = a;
	Bnd_Box bb = b;

	aa.Enlarge(tolerance);
	bb.Enlarge(tolerance);

	return !aa.IsOut(bb);
}

void ModelBuilder::checkCoincidentFaces(
	const TopoDS_Shape& shape,
	ShapeCheckReport& report,
	double distanceTolerance,
	double areaTolerance,
	int samplesPerDirection,
	int minInteriorHits) const
{
	bool found = false;

	for (TopExp_Explorer solidEx(shape, TopAbs_SOLID); solidEx.More(); solidEx.Next())
	{
		const TopoDS_Solid solid = TopoDS::Solid(solidEx.Current());

		std::vector<TopoDS_Face> faces;

		for (TopExp_Explorer shellEx(solid, TopAbs_SHELL); shellEx.More(); shellEx.Next())
		{
			const TopoDS_Shell shell = TopoDS::Shell(shellEx.Current());

			for (TopExp_Explorer faceEx(shell, TopAbs_FACE); faceEx.More(); faceEx.Next())
			{
				TopoDS_Face face = TopoDS::Face(faceEx.Current());

				if (face.Orientation() == TopAbs_FORWARD ||
					face.Orientation() == TopAbs_REVERSED)
				{
					faces.push_back(face);
				}
			}
		}

		const std::size_t oldCount = report.coincidentFaces.size();

		checkCoincidentFacesInList(
			faces,
			report,
			distanceTolerance,
			areaTolerance,
			samplesPerDirection,
			minInteriorHits);

		if (report.coincidentFaces.size() > oldCount)
			found = true;
	}

	if (found)
	{
		report.messages.push_back(
			"Coincident or nearly coincident boundary faces were found. "
			"The topology may be valid, but the model can contain zero-thickness regions.");
	}
}

void ModelBuilder::checkCoincidentFacesInList(
	const std::vector<TopoDS_Face>& faces,
	ShapeCheckReport& report,
	double distanceTolerance,
	double areaTolerance,
	int samplesPerDirection,
	int minInteriorHits) const
{
	std::vector<Bnd_Box> boxes;
	std::vector<double> areas;

	boxes.reserve(faces.size());
	areas.reserve(faces.size());

	for (const auto& face : faces)
	{
		Bnd_Box box;
		BRepBndLib::Add(face, box);

		boxes.push_back(box);
		areas.push_back(faceArea(face));
	}

	for (std::size_t i = 0; i < faces.size(); ++i)
	{
		for (std::size_t j = i + 1; j < faces.size(); ++j)
		{
			if (!boxesOverlapWithTolerance(boxes[i], boxes[j], distanceTolerance))
				continue;

			const double areaA = areas[i];
			const double areaB = areas[j];

			if (areaA <= areaTolerance || areaB <= areaTolerance)
				continue;

			const double maxArea = std::max(areaA, areaB);

			if (std::abs(areaA - areaB) > 0.05 * maxArea)
				continue;

			BRepExtrema_DistShapeShape dist(faces[i], faces[j]);
			dist.Perform();

			if (!dist.IsDone())
				continue;

			if (dist.Value() > distanceTolerance)
				continue;

			// Important: use &&, not ||.
			// Both trimmed faces must contain several interior points of the other face.
			const bool coincident =
				areFacesCoincidentBySampling(
					faces[i],
					faces[j],
					distanceTolerance,
					samplesPerDirection,
					minInteriorHits) &&
				areFacesCoincidentBySampling(
					faces[j],
					faces[i],
					distanceTolerance,
					samplesPerDirection,
					minInteriorHits);

			if (!coincident)
				continue;

			CoincidentFacePair pair;
			pair.first = faces[i];
			pair.second = faces[j];
			pair.distance = dist.Value();

			report.coincidentFaces.push_back(pair);
		}
	}
}

bool ModelBuilder::areFacesCoincidentBySampling(
	const TopoDS_Face& a,
	const TopoDS_Face& b,
	double distanceTolerance,
	int samplesPerDirection,
	int minInteriorHits) const
{
	Standard_Real uMin = 0.0;
	Standard_Real uMax = 0.0;
	Standard_Real vMin = 0.0;
	Standard_Real vMax = 0.0;

	BRepTools::UVBounds(a, uMin, uMax, vMin, vMax);

	if (uMax <= uMin || vMax <= vMin)
		return false;

	BRepAdaptor_Surface adaptorA(a);

	int interiorHits = 0;

	for (int iu = 1; iu <= samplesPerDirection; ++iu)
	{
		const double u =
			uMin + (uMax - uMin) * static_cast<double>(iu) /
			static_cast<double>(samplesPerDirection + 1);

		for (int iv = 1; iv <= samplesPerDirection; ++iv)
		{
			const double v =
				vMin + (vMax - vMin) * static_cast<double>(iv) /
				static_cast<double>(samplesPerDirection + 1);

			gp_Pnt p = adaptorA.Value(u, v);

			// The sample must be strictly inside the trimmed source face.
			if (!isPointStrictlyInsideFaceUV(a, p, distanceTolerance))
				continue;

			// The same 3D point must also be strictly inside the trimmed target face.
			// This excludes edge-only contacts and complementary cylinder patches.
			if (!isPointStrictlyInsideFaceUV(b, p, distanceTolerance))
				continue;

			++interiorHits;

			if (interiorHits >= minInteriorHits)
				return true;
		}
	}

	return false;
}

bool ModelBuilder::samplePointOnFace(
	const TopoDS_Face& face,
	double u,
	double v,
	gp_Pnt& point) const
{
	BRepAdaptor_Surface adaptor(face);
	Handle(Geom_Surface) surface = adaptor.Surface().Surface();

	if (surface.IsNull())
		return false;

	point = surface->Value(u, v);
	return true;
}

bool ModelBuilder::isPointStrictlyInsideFaceUV(
	const TopoDS_Face& face,
	const gp_Pnt& point,
	double tolerance) const
{
	BRepAdaptor_Surface adaptor(face);
	Handle(Geom_Surface) surface = adaptor.Surface().Surface();

	if (surface.IsNull())
		return false;

	GeomAPI_ProjectPointOnSurf projector(point, surface);

	if (projector.NbPoints() < 1)
		return false;

	Standard_Real u = 0.0;
	Standard_Real v = 0.0;
	projector.LowerDistanceParameters(u, v);

	gp_Pnt projected = surface->Value(u, v);

	if (point.Distance(projected) > tolerance)
		return false;

	BRepClass_FaceClassifier classifier;
	classifier.Perform(face, gp_Pnt2d(u, v), tolerance);

	return classifier.State() == TopAbs_IN;
}

double ModelBuilder::edgeLength(const TopoDS_Edge& edge) const
{
	GProp_GProps props;
	BRepGProp::LinearProperties(edge, props);
	return props.Mass();
}

double ModelBuilder::faceArea(const TopoDS_Face& face) const
{
	GProp_GProps props;
	BRepGProp::SurfaceProperties(face, props);
	return props.Mass();
}

double ModelBuilder::solidVolume(const TopoDS_Solid& solid) const
{
	GProp_GProps props;
	BRepGProp::VolumeProperties(solid, props);
	return props.Mass();
}

std::string ModelBuilder::brepCheckStatusToString(int status) const
{
	switch (static_cast<BRepCheck_Status>(status))
	{
	case BRepCheck_NoError: return "NoError";
	case BRepCheck_InvalidPointOnCurve: return "InvalidPointOnCurve";
	case BRepCheck_InvalidPointOnCurveOnSurface: return "InvalidPointOnCurveOnSurface";
	case BRepCheck_InvalidPointOnSurface: return "InvalidPointOnSurface";
	case BRepCheck_No3DCurve: return "No3DCurve";
	case BRepCheck_Multiple3DCurve: return "Multiple3DCurve";
	case BRepCheck_Invalid3DCurve: return "Invalid3DCurve";
	case BRepCheck_NoCurveOnSurface: return "NoCurveOnSurface";
	case BRepCheck_InvalidCurveOnSurface: return "InvalidCurveOnSurface";
	case BRepCheck_InvalidCurveOnClosedSurface: return "InvalidCurveOnClosedSurface";
	case BRepCheck_InvalidSameRangeFlag: return "InvalidSameRangeFlag";
	case BRepCheck_InvalidSameParameterFlag: return "InvalidSameParameterFlag";
	case BRepCheck_InvalidDegeneratedFlag: return "InvalidDegeneratedFlag";
	case BRepCheck_FreeEdge: return "FreeEdge";
	case BRepCheck_InvalidMultiConnexity: return "InvalidMultiConnexity";
	case BRepCheck_InvalidRange: return "InvalidRange";
	case BRepCheck_EmptyWire: return "EmptyWire";
	case BRepCheck_RedundantEdge: return "RedundantEdge";
	case BRepCheck_SelfIntersectingWire: return "SelfIntersectingWire";
	case BRepCheck_NoSurface: return "NoSurface";
	case BRepCheck_InvalidWire: return "InvalidWire";
	case BRepCheck_RedundantWire: return "RedundantWire";
	case BRepCheck_IntersectingWires: return "IntersectingWires";
	case BRepCheck_InvalidImbricationOfWires: return "InvalidImbricationOfWires";
	case BRepCheck_EmptyShell: return "EmptyShell";
	case BRepCheck_RedundantFace: return "RedundantFace";
	case BRepCheck_UnorientableShape: return "UnorientableShape";
	case BRepCheck_NotClosed: return "NotClosed";
	case BRepCheck_NotConnected: return "NotConnected";
	case BRepCheck_SubshapeNotInShape: return "SubshapeNotInShape";
	case BRepCheck_BadOrientation: return "BadOrientation";
	case BRepCheck_BadOrientationOfSubshape: return "BadOrientationOfSubshape";
	case BRepCheck_InvalidToleranceValue: return "InvalidToleranceValue";
	case BRepCheck_CheckFail: return "CheckFail";
	default: return "Unknown";
	}
}

gp_Pnt ModelBuilder::computeEdgeMidPoint(const TopoDS_Edge& edge) const
{
	Standard_Real first = 0.0;
	Standard_Real last = 0.0;

	Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);

	if (!curve.IsNull())
	{
		const Standard_Real mid = 0.5 * (first + last);
		return curve->Value(mid);
	}

	GProp_GProps props;
	BRepGProp::LinearProperties(edge, props);
	return props.CentreOfMass();
}

gp_Pnt ModelBuilder::computeFaceCenterPoint(const TopoDS_Face& face) const
{
	Standard_Real uMin = 0.0;
	Standard_Real uMax = 0.0;
	Standard_Real vMin = 0.0;
	Standard_Real vMax = 0.0;

	BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);

	BRepAdaptor_Surface adaptor(face);

	const double tolerance = 1.0e-7;

	// Try the parametric center first.
	{
		const Standard_Real u = 0.5 * (uMin + uMax);
		const Standard_Real v = 0.5 * (vMin + vMax);

		gp_Pnt p = adaptor.Value(u, v);

		if (isPointStrictlyInsideFace(face, p, tolerance))
			return p;
	}

	// Search a valid inner sample point in UV space.
	const int samples = 15;

	for (int iu = 1; iu <= samples; ++iu)
	{
		const Standard_Real u =
			uMin + (uMax - uMin) * static_cast<double>(iu) /
			static_cast<double>(samples + 1);

		for (int iv = 1; iv <= samples; ++iv)
		{
			const Standard_Real v =
				vMin + (vMax - vMin) * static_cast<double>(iv) /
				static_cast<double>(samples + 1);

			gp_Pnt p = adaptor.Value(u, v);

			if (isPointStrictlyInsideFace(face, p, tolerance))
				return p;
		}
	}

	// Fallback: use the surface center of mass only as last resort.
	// This point is not guaranteed to lie on curved faces such as cylinders.
	GProp_GProps props;
	BRepGProp::SurfaceProperties(face, props);
	gp_Pnt center = props.CentreOfMass();

	return center;
}

bool ModelBuilder::isPointStrictlyInsideFace(
	const TopoDS_Face& face,
	const gp_Pnt& point,
	double tolerance) const
{
	BRepClass_FaceClassifier classifier(face, point, tolerance);

	// Strictly require IN.
	// ON would also be true for edge contacts, which should not be considered.
	return classifier.State() == TopAbs_IN;
}

void ModelBuilder::collectDiagnosticPoints(ShapeCheckReport& report) const
{
	auto addEdgePoint = [&](const TopoDS_Edge& edge, const std::string& reason)
		{
			ShapeDiagnosticPoint p;
			p.point = computeEdgeMidPoint(edge);
			p.sourceShape = edge;
			p.reason = reason;
			report.diagnosticPoints.push_back(p);
		};

	auto addFacePoint = [&](const TopoDS_Face& face, const std::string& reason)
		{
			ShapeDiagnosticPoint p;
			p.point = computeFaceCenterPoint(face);
			p.sourceShape = face;
			p.reason = reason;
			report.diagnosticPoints.push_back(p);
		};

	for (const auto& edge : report.freeEdges)
		addEdgePoint(edge, "Free edge");

	for (const auto& edge : report.nonManifoldEdges)
		addEdgePoint(edge, "Non-manifold edge");

	for (const auto& edge : report.isolatedEdges)
		addEdgePoint(edge, "Isolated edge");

	for (const auto& edge : report.degeneratedEdges)
		addEdgePoint(edge, "Degenerated edge");

	for (const auto& edge : report.tooShortEdges)
		addEdgePoint(edge, "Too short edge");

	for (const auto& face : report.invalidFaces)
		addFacePoint(face, "Invalid face");

	for (const auto& face : report.tooSmallFaces)
		addFacePoint(face, "Too small face");

	for (const auto& pair : report.coincidentFaces)
	{
		addFacePoint(pair.first, "Coincident face pair");
	}
}

void ModelBuilder::extractFaceTriangles(
	const TopoDS_Face& face,
	EntityAnnotation* annotation,
	double r,
	double g,
	double b,
	double linearDeflection,
	double angularDeflection) const
{
	if (annotation == nullptr)
		return;

	BRepMesh_IncrementalMesh mesher(
		face,
		linearDeflection,
		Standard_False,
		angularDeflection,
		Standard_True);

	TopLoc_Location location;
	Handle(Poly_Triangulation) triangulation =
		BRep_Tool::Triangulation(face, location);

	if (triangulation.IsNull())
		return;

	const gp_Trsf transform = location.Transformation();

	for (int i = 1; i <= triangulation->NbTriangles(); ++i)
	{
		int n1, n2, n3;
		triangulation->Triangle(i).Get(n1, n2, n3);

		gp_Pnt p1 = triangulation->Node(n1);
		gp_Pnt p2 = triangulation->Node(n2);
		gp_Pnt p3 = triangulation->Node(n3);

		p1.Transform(transform);
		p2.Transform(transform);
		p3.Transform(transform);

		if (face.Orientation() == TopAbs_REVERSED)
		{
			annotation->addTriangle(
				p1.X(), p1.Y(), p1.Z(),
				p3.X(), p3.Y(), p3.Z(),
				p2.X(), p2.Y(), p2.Z(),
				r, g, b);
		}
		else
		{
			annotation->addTriangle(
				p1.X(), p1.Y(), p1.Z(),
				p2.X(), p2.Y(), p2.Z(),
				p3.X(), p3.Y(), p3.Z(),
				r, g, b);
		}
	}
}

bool ModelBuilder::computeFaceNormalAtUV(
	const TopoDS_Face& face,
	double u,
	double v,
	gp_Vec& normal) const
{
	BRepAdaptor_Surface adaptor(face);
	BRepLProp_SLProps props(adaptor, u, v, 1, 1.0e-9);

	if (!props.IsNormalDefined())
		return false;

	gp_Dir n = props.Normal();

	if (face.Orientation() == TopAbs_REVERSED)
		n.Reverse();

	normal = gp_Vec(n);
	return true;
}

bool ModelBuilder::analyzeThinGapBySampling(
	const TopoDS_Face& a,
	const TopoDS_Face& b,
	ThinGapFacePair& result,
	double gapTolerance,
	int samplesPerDirection,
	double minHitRatio,
	double maxDistanceVariation) const
{
	Standard_Real uMin, uMax, vMin, vMax;
	BRepTools::UVBounds(a, uMin, uMax, vMin, vMax);

	if (uMax <= uMin || vMax <= vMin)
		return false;

	BRepAdaptor_Surface adaptorA(a);
	BRepAdaptor_Surface adaptorB(b);
	Handle(Geom_Surface) surfaceB = adaptorB.Surface().Surface();

	if (surfaceB.IsNull())
		return false;

	int validSamples = 0;
	int hits = 0;

	double minDist = std::numeric_limits<double>::max();
	double maxDist = 0.0;
	double sumDist = 0.0;

	std::vector<gp_Pnt> hitPoints;

	// Use only the inner UV region to avoid wedge-like edge contacts.
	const double innerMin = 0.20;
	const double innerMax = 0.80;

	for (int iu = 0; iu < samplesPerDirection; ++iu)
	{
		const double tu =
			innerMin + (innerMax - innerMin) *
			static_cast<double>(iu) /
			static_cast<double>(std::max(1, samplesPerDirection - 1));

		const double u = uMin + (uMax - uMin) * tu;

		for (int iv = 0; iv < samplesPerDirection; ++iv)
		{
			const double tv =
				innerMin + (innerMax - innerMin) *
				static_cast<double>(iv) /
				static_cast<double>(std::max(1, samplesPerDirection - 1));

			const double v = vMin + (vMax - vMin) * tv;

			gp_Pnt p = adaptorA.Value(u, v);

			if (!isPointStrictlyInsideFaceUV(a, p, gapTolerance))
				continue;

			++validSamples;

			GeomAPI_ProjectPointOnSurf projector(p, surfaceB);

			if (projector.NbPoints() < 1)
				continue;

			Standard_Real ub = 0.0;
			Standard_Real vb = 0.0;
			projector.LowerDistanceParameters(ub, vb);

			gp_Pnt q = surfaceB->Value(ub, vb);
			const double d = p.Distance(q);

			if (d <= 0.0 || d > gapTolerance)
				continue;

			if (!isPointStrictlyInsideFaceUV(b, q, gapTolerance))
				continue;

			gp_Vec normalA;
			gp_Vec normalB;

			if (!computeFaceNormalAtUV(a, u, v, normalA))
				continue;

			if (!computeFaceNormalAtUV(b, ub, vb, normalB))
				continue;

			normalA.Normalize();
			normalB.Normalize();

			// Thin gaps usually have opposite-facing normals.
			const double dot = normalA.Dot(normalB);

			if (dot > -0.7)
				continue;

			++hits;

			minDist = std::min(minDist, d);
			maxDist = std::max(maxDist, d);
			sumDist += d;

			hitPoints.push_back(p);
		}
	}

	if (validSamples == 0 || hits == 0)
		return false;

	const double hitRatio = static_cast<double>(hits) / static_cast<double>(validSamples);

	if (hitRatio < minHitRatio)
		return false;

	const double meanDist = sumDist / static_cast<double>(hits);

	// Reject wedge-like situations where the distance changes too much.
	if (minDist > 0.0 && maxDist / minDist > maxDistanceVariation)
		return false;

	result.first = a;
	result.second = b;
	result.minDistance = minDist;
	result.meanDistance = meanDist;
	result.maxDistance = maxDist;
	result.hitRatio = hitRatio;
	result.diagnosticPoints = std::move(hitPoints);

	return true;
}

void ModelBuilder::checkThinGapsInList(
	const std::vector<TopoDS_Face>& faces,
	ShapeCheckReport& report,
	double gapTolerance,
	double areaTolerance,
	int samplesPerDirection,
	double minHitRatio,
	double maxDistanceVariation) const
{
	std::vector<Bnd_Box> boxes;
	std::vector<double> areas;

	boxes.reserve(faces.size());
	areas.reserve(faces.size());

	for (const auto& face : faces)
	{
		Bnd_Box box;
		BRepBndLib::Add(face, box);
		box.Enlarge(gapTolerance);

		boxes.push_back(box);
		areas.push_back(faceArea(face));
	}

	for (std::size_t i = 0; i < faces.size(); ++i)
	{
		for (std::size_t j = i + 1; j < faces.size(); ++j)
		{
			if (boxes[i].IsOut(boxes[j]))
				continue;

			if (areas[i] <= areaTolerance || areas[j] <= areaTolerance)
				continue;

			BRepExtrema_DistShapeShape dist(faces[i], faces[j]);
			dist.Perform();

			if (!dist.IsDone())
				continue;

			const double minDistance = dist.Value();

			if (minDistance <= 0.0 || minDistance > gapTolerance)
				continue;

			ThinGapFacePair gap;

			const bool found =
				analyzeThinGapBySampling(
					faces[i],
					faces[j],
					gap,
					gapTolerance,
					samplesPerDirection,
					minHitRatio,
					maxDistanceVariation) ||
				analyzeThinGapBySampling(
					faces[j],
					faces[i],
					gap,
					gapTolerance,
					samplesPerDirection,
					minHitRatio,
					maxDistanceVariation);

			if (!found)
				continue;

			report.thinGapFaces.push_back(gap);
		}
	}
}

void ModelBuilder::checkThinGaps(
	const TopoDS_Shape& shape,
	ShapeCheckReport& report,
	double gapTolerance,
	double areaTolerance,
	int samplesPerDirection,
	double minHitRatio,
	double maxDistanceVariation) const
{
	bool found = false;

	for (TopExp_Explorer solidEx(shape, TopAbs_SOLID); solidEx.More(); solidEx.Next())
	{
		TopoDS_Solid solid = TopoDS::Solid(solidEx.Current());

		std::vector<TopoDS_Face> faces;

		for (TopExp_Explorer shellEx(solid, TopAbs_SHELL); shellEx.More(); shellEx.Next())
		{
			TopoDS_Shell shell = TopoDS::Shell(shellEx.Current());

			for (TopExp_Explorer faceEx(shell, TopAbs_FACE); faceEx.More(); faceEx.Next())
			{
				TopoDS_Face face = TopoDS::Face(faceEx.Current());

				if ((face.Orientation() == TopAbs_FORWARD || face.Orientation() == TopAbs_REVERSED) && !isPlanarFace(face))
				{
					faces.push_back(face);
				}
			}
		}

		const std::size_t oldCount = report.thinGapFaces.size();

		checkThinGapsInList(
			faces,
			report,
			gapTolerance,
			areaTolerance,
			samplesPerDirection,
			minHitRatio,
			maxDistanceVariation);

		if (report.thinGapFaces.size() > oldCount)
			found = true;
	}

	if (found)
	{
		report.messages.push_back(
			"Thin gaps between nearby opposite boundary faces were found. "
			"The shape may be topologically valid but problematic for surface or volume meshing.");
	}
}

void ModelBuilder::visualizeThinGapFaces(
	const ShapeCheckReport& report,
	EntityAnnotation* annotation,
	double linearDeflection,
	double angularDeflection) const
{
	if (annotation == nullptr)
		return;

	for (const auto& gap : report.thinGapFaces)
	{
		// First face (red)
		extractFaceTriangles(
			gap.first,
			annotation,
			1.0, 0.0, 0.0,
			linearDeflection,
			angularDeflection);

		// Second face (orange)
		extractFaceTriangles(
			gap.second,
			annotation,
			1.0, 0.5, 0.0,
			linearDeflection,
			angularDeflection);
	}
}

bool ModelBuilder::isPlanarFace(const TopoDS_Face& face) const
{
	BRepAdaptor_Surface adaptor(face);
	return adaptor.GetType() == GeomAbs_Plane;
}

bool ModelBuilder::isToleratedTouchingEdge(
	const TopoDS_Edge& edge,
	const TopTools_ListOfShape& adjacentFaces,
	double angleTolerance) const
{
	std::vector<TopoDS_Face> faces;

	for (TopTools_ListIteratorOfListOfShape it(adjacentFaces); it.More(); it.Next())
		faces.push_back(TopoDS::Face(it.Value()));

	if (faces.size() != 4)
		return false;

	std::vector<gp_Vec> normals;

	for (const auto& face : faces)
	{
		gp_Vec n;

		if (!computeFaceNormalNearEdge(face, edge, n))
			return false;

		if (n.Magnitude() <= 0.0)
			return false;

		n.Normalize();
		normals.push_back(n);
	}

	std::vector<bool> used(4, false);
	int oppositePairs = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (used[i])
			continue;

		int bestJ = -1;
		double bestDot = 1.0;

		for (int j = i + 1; j < 4; ++j)
		{
			if (used[j])
				continue;

			const double dot = normals[i].Dot(normals[j]);

			if (dot < bestDot)
			{
				bestDot = dot;
				bestJ = j;
			}
		}

		if (bestJ < 0)
			return false;

		// Opposite normals indicate two faces belonging to two different touching sheets.
		if (bestDot > -1.0 + angleTolerance)
			return false;

		used[i] = true;
		used[bestJ] = true;
		++oppositePairs;
	}

	return oppositePairs == 2;
}

bool ModelBuilder::computeFaceNormalNearEdge(
	const TopoDS_Face& face,
	const TopoDS_Edge& edge,
	gp_Vec& normal) const
{
	Standard_Real first = 0.0;
	Standard_Real last = 0.0;

	Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);

	if (curve.IsNull())
		return false;

	const gp_Pnt edgePoint = curve->Value(0.5 * (first + last));

	BRepAdaptor_Surface adaptor(face);
	Handle(Geom_Surface) surface = adaptor.Surface().Surface();

	if (surface.IsNull())
		return false;

	GeomAPI_ProjectPointOnSurf projector(edgePoint, surface);

	if (projector.NbPoints() < 1)
		return false;

	Standard_Real u = 0.0;
	Standard_Real v = 0.0;
	projector.LowerDistanceParameters(u, v);

	BRepLProp_SLProps props(adaptor, u, v, 1, 1.0e-9);

	if (!props.IsNormalDefined())
		return false;

	gp_Dir dir = props.Normal();

	if (face.Orientation() == TopAbs_REVERSED)
		dir.Reverse();

	normal = gp_Vec(dir);
	return true;
}

