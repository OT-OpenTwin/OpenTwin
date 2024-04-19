#include "ModelBuilder.h"

#include "Properties.h"
#include "StepWidthManager.h"
#include "MaterialManager.h"
#include "Application.h"

#include "EntityGeometry.h"
#include "EntityMaterial.h"
#include "GeometryOperations.h"
#include "TemplateDefaultManager.h"
#include "Database.h"

#include "OTServiceFoundation/ModelComponent.h"

#include "BRepPrimAPI_MakeSphere.hxx"
#include "BRepAlgoAPI_BuilderAlgo.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepAlgoAPI_Fuse.hxx"
#include "TopoDS.hxx"

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
	buildNonManifoldModel(meshName, geometryEntities, properties, materialManager);
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
			if (application->modelComponent()->getCurrentMaterialName(material).empty())
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
			material = application->modelComponent()->getCurrentMaterialName(properties.getBoundingSphereMaterial());
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
	BoundingBox boundingBox = GeometryOperations::getBoundingBox(geometryEntities);

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
	EntityGeometry *backgroundSphere = new EntityGeometry(0, nullptr, nullptr, nullptr, nullptr, "Model");

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

void ModelBuilder::buildNonManifoldModel(const std::string &meshName, std::list<EntityGeometry *> &geometryEntities, Properties &properties, MaterialManager &materialManager)
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
		EntityMaterial *material = materialManager.getMaterial(application->modelComponent()->getCurrentMaterialName(materialProperty));
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
	EntityGeometry *entityGeom = new EntityGeometry(0, nullptr, nullptr, nullptr, nullptr, application->serviceName());

	entityGeom->setName(name);
	entityGeom->setEditable(true);
	entityGeom->setBrep(shape);
	entityGeom->setInitiallyHidden(true);

	entityGeom->getProperties().buildFromConfiguration(shapeProperties);
	entityGeom->getProperties().forceResetUpdateForAllProperties();

	entityGeom->getProperties().setAllPropertiesReadOnly();

	return entityGeom;
}

void ModelBuilder::storeEntities(void)
{
	DataBase::GetDataBase()->queueWriting(true);

	for (auto geometryEntity : modelEntities)
	{
		application->modelComponent()->facetAndStoreGeometryEntity(geometryEntity, false);
	}

	DataBase::GetDataBase()->queueWriting(false);
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
