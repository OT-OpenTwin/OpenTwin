#include "SimplifyRemoveFaces.h"
#include "EntityGeometry.h"
#include "EntityFaceAnnotation.h"
#include "DataBase.h"
#include "EntityCache.h"
#include "UpdateManager.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include "BRepExtrema_DistShapeShape.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Shell.hxx"
#include "BRepAlgoAPI_Defeaturing.hxx"

#include <map>
#include <set>

void SimplifyRemoveFaces::enterRemoveFacesMode(void)
{
	std::map<std::string, std::string> options;

	uiComponent->enterEntitySelectionMode(modelComponent->getCurrentVisualizationModelID(), ot::components::UiComponent::entitySlectionType::FACE, 
									      true, "", ot::components::UiComponent::entitySelectionAction::REMOVE_FACE, "remove", options, serviceID);
}

void SimplifyRemoveFaces::performOperation(const std::string &selectionInfo)
{
	ot::LockTypeFlags lockFlags;
	lockFlags.setFlag(ot::LockModelWrite);
	lockFlags.setFlag(ot::LockNavigationWrite);
	lockFlags.setFlag(ot::LockViewWrite);
	lockFlags.setFlag(ot::LockProperties);

	uiComponent->lockUI(lockFlags);

	ot::JsonDocument doc;
	doc.fromJson(selectionInfo);

	ot::ConstJsonArray modelID = ot::json::getArray(doc, "modelID");
	ot::ConstJsonArray posX = ot::json::getArray(doc, "posX");
	ot::ConstJsonArray posY = ot::json::getArray(doc, "posY");
	ot::ConstJsonArray posZ = ot::json::getArray(doc, "posZ");

	// Get a list of all shapes which are involved in this operation for prefetching
	std::set<ot::UID> affectedEntitiesSet;
	std::list<SimplifyRemoveFaceData> faceList;

	for (unsigned int i = 0; i < modelID.Size(); i++)
	{
		ot::UID entityID = modelID[i].GetUint64();

		affectedEntitiesSet.insert(entityID);

		SimplifyRemoveFaceData faceData;
		faceData.setEntityID(entityID);
		faceData.setPosition(posX[i].GetDouble(), posY[i].GetDouble(), posZ[i].GetDouble());
		faceList.push_back(faceData);
	}

	// Now get the current versions for these entities
	std::list<ot::UID> affectedEntities(affectedEntitiesSet.begin(), affectedEntitiesSet.end());
	std::list<ot::EntityInformation> entityInfo;
	modelComponent->getEntityInformation(affectedEntities, entityInfo);

	// Prefetch the geometry entities
	entityCache->prefetchEntities(entityInfo);

	// Now we prefetch all breps for these entities
	std::list<std::pair<ot::UID, ot::UID>> prefetchList;

	std::map<ot::UID, ot::UID> entityVersionMap;
	std::list<ot::UID> entityBreps;

	for (auto entity : entityInfo)
	{
		entityVersionMap[entity.getID()] = entity.getVersion();

		EntityGeometry *geometryEntity = dynamic_cast<EntityGeometry *>(entityCache->getEntity(entity.getID(), entity.getVersion()));

		if (geometryEntity->getEditable())
		{
			entityBreps.push_back(geometryEntity->getBrepStorageObjectID());
		}
	}

	std::list<ot::EntityInformation> entityBrepInfo;
	modelComponent->getEntityInformation(entityBreps, entityBrepInfo);

	// Prefetch the brep entities
	entityCache->prefetchEntities(entityBrepInfo);

	// Add the brep entities to the version map
	for (auto entity : entityBrepInfo)
	{
		entityVersionMap[entity.getID()] = entity.getVersion();
	}

	// Here, we sort the picked faces by entity 
	std::map<ot::UID, std::list<SimplifyRemoveFaceData>> facesMap;

	for (auto face : faceList)
	{
		facesMap[face.getEntityID()].push_back(face);
	}

	modelComponent->enableMessageQueueing(true);

	bool anySuccess = false;
	std::string error;

	// In a next step, we process entity by entity and perform the face removal operation for this entity
	std::list<ot::UID> modifiedEntities;

	for (auto shape : facesMap)
	{
		// Here we need to load the geometry entity, since we are going to modify it later. If we took it from the cache, it would be modified there.

		EntityGeometry *geometryEntity = dynamic_cast<EntityGeometry*>(modelComponent->readEntityFromEntityIDandVersion(shape.first, entityVersionMap[shape.first], *classFactory));

		if (geometryEntity != nullptr)
		{
			if (geometryEntity->getEditable())
			{
				if (removeFacesFromEntity(geometryEntity, geometryEntity->getBrepStorageObjectID(), entityVersionMap[geometryEntity->getBrepStorageObjectID()], shape.second, modifiedEntities))
				{
					anySuccess = true;
				}
				else
				{
					if (error.empty()) error = "Unable to remove the faces from the following shapes:\n";
					error.append(geometryEntity->getName() + "\n");
				}
			}

			delete geometryEntity;
			geometryEntity = nullptr;
		}
	}

	// Now we ask the model service to check for the potential updates of the parent entities
	getUpdateManager()->checkParentUpdates(modifiedEntities);

	modelComponent->enableMessageQueueing(false);
	uiComponent->refreshSelection(modelComponent->getCurrentVisualizationModelID());

	if (anySuccess)
	{
		// Now we note the end of the model change operation
		modelComponent->modelChangeOperationCompleted("remove faces");
	}

	// Request a view refresh and release the user interface
	uiComponent->refreshSelection(modelComponent->getCurrentVisualizationModelID());
	uiComponent->unlockUI(lockFlags);

	// Notify the error, if any
	if (!error.empty())
	{
		uiComponent->displayErrorPrompt(error);
	}
}

bool SimplifyRemoveFaces::removeFacesFromEntity(EntityGeometry *geometryEntity, ot::UID brepID, ot::UID brepVersion, std::list<SimplifyRemoveFaceData> &faces, std::list<ot::UID> &modifiedEntities)
{
	assert(geometryEntity != nullptr);

	EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(brepID, brepVersion));
	if (brepEntity == nullptr) return false;

	TopoDS_Shape shape = brepEntity->getBrep();

	// Search for all the OCC faces in the entity and store them in the algorithm
	BRepAlgoAPI_Defeaturing algorithm;
	algorithm.SetShape(shape);
	algorithm.SetRunParallel(false);
	algorithm.SetToFillHistory(false);

	for (auto face : faces)
	{
		TopoDS_Shape result;
		if (findFaceFromPosition(shape, face.getX(), face.getY(), face.getZ(), result))
		{
			algorithm.AddFaceToRemove(result);
		}
	}

	if (!algorithm.FacesToRemove().IsEmpty())
	{
		// We have something do do (at least one face was selected
		algorithm.Build();

		if (!algorithm.IsDone())
		{
			// We have some errors, the operation did not complete
			std::stringstream errors;
			algorithm.DumpErrors(errors);

			std::cout << errors.str() << std::endl;
			return false;
		}

		if (algorithm.HasWarnings())
		{
			// We have some warnings
			std::stringstream warnings;
			algorithm.DumpWarnings(warnings);

			std::cout << warnings.str() << std::endl;
			return false;
		}

		// Replace the brep with the new one and facet the entity (this will require new ot::UIDs for brep and facet entities)
		geometryEntity->resetBrep();
		geometryEntity->resetFacets();

		geometryEntity->getBrepEntity()->setEntityID(modelComponent->createEntityUID());
		geometryEntity->getFacets()->setEntityID(modelComponent->createEntityUID());

		// Add the new brep entity to the geometry entity
		geometryEntity->setBrep(algorithm.Shape());

		// Facet the geometry entity and store it
		geometryEntity->facetEntity(false);

		geometryEntity->storeBrep();

		ot::UID brepStorageVersion = geometryEntity->getBrepEntity()->getEntityStorageVersion();

		entityCache->cacheEntity(geometryEntity->getBrepEntity());
		geometryEntity->detachBrep();

		geometryEntity->storeFacets();

		ot::UID facetsStorageVersion = geometryEntity->getFacets()->getEntityStorageVersion();

		geometryEntity->releaseFacets();

		modifiedEntities.push_back(geometryEntity->getEntityID());

		// Store the information about the entities such that they can be added to the model

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_UpdateGeometryEntity, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, geometryEntity->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Brep, (unsigned long long) geometryEntity->getBrepStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Facets, (unsigned long long) geometryEntity->getFacetsStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_OverrideGeometry, false, doc.GetAllocator());	// The modified entity was not yet written 

		ot::PropertyGridCfg cfg;
		geometryEntity->getProperties().addToConfiguration(nullptr, false, cfg);

		ot::JsonObject cfgObj;
		cfg.addToJsonObject(cfgObj, doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_MODEL_NewProperties, cfgObj, doc.GetAllocator());

		modelComponent->sendMessage(false, doc);
	}
	else
	{
		return false;
	}

	return true;
}

bool SimplifyRemoveFaces::findFaceFromPosition(TopoDS_Shape &shape, double x, double y, double z, TopoDS_Shape &face)
{
	BRepBuilderAPI_MakeVertex vertex(gp_Pnt(x, y, z));

	TopoDS_Shape result;
	bool success = false;

	double minDist = 1e30;

	TopExp_Explorer exp;
	for (exp.Init(shape, TopAbs_SHELL); exp.More(); exp.Next())
	{
		TopoDS_Shell aShell = TopoDS::Shell(exp.Current());
		BRepExtrema_DistShapeShape minimumDist(aShell, vertex.Vertex(), Extrema_ExtFlag_MIN);

		int hits = minimumDist.NbSolution();

		assert(!minimumDist.InnerSolution());  // If the point is inside the solid, we get this behavior. In this case, it we don't know what the closest face is.
												// We need a special treatment for this situation (e.g. invert the solid or make a sheet model out of it).

		for (int hit = 1; hit <= hits; hit++)
		{
			if (minimumDist.SupportTypeShape1(hit) == BRepExtrema_SupportType::BRepExtrema_IsInFace)
			{
				// We found the face which is closest to the given point

				if (minimumDist.Value() < minDist)
				{
					success = true;
					minDist = minimumDist.Value();
					face = minimumDist.SupportOnShape1(hit);
				}
			}
		}
	}

	return success;
}
