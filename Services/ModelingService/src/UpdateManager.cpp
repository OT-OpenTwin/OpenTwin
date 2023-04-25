#include "UpdateManager.h"

#include "DataBase.h"
#include "ClassFactoryCAD.h"
#include "EntityGeometry.h"
#include "Transformations.h"
#include "BooleanOperations.h"
#include "entityCache.h"
#include "PrimitiveManager.h"

#include <map>

#include "TopoDS.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "TopExp_Explorer.hxx"

#include "Application.h"

UpdateManager::UpdateManager(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, EntityCache *_entityCache, PrimitiveManager *_primitiveManager, BooleanOperations *_booleanOperations) :
	uiComponent(_uiComponent),
	modelComponent(_modelComponent),
	entityCache(_entityCache),
	primitiveManager(_primitiveManager),
	booleanOperations(_booleanOperations)
{

}

void UpdateManager::checkParentUpdates(std::list<ot::UID> modifiedEntities)
{
	while (!modifiedEntities.empty())
	{
		OT_rJSON_createDOC(parentCheckDoc);
		ot::rJSON::add(parentCheckDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_CheckParentUpdates);
		ot::rJSON::add(parentCheckDoc, OT_ACTION_PARAM_MODEL_EntityIDList, modifiedEntities);

		std::string response = modelComponent->sendMessage(false, parentCheckDoc);

		modifiedEntities.clear();

		if (!response.empty())
		{
			// Here we received some information that we need to update the parents. The return doc contains a list of parents to update
			rapidjson::Document doc;
			doc.Parse(response.c_str());

			if (doc.IsObject())
			{
				std::string action = ot::rJSON::getString(doc, OT_ACTION_MEMBER);
				assert(action == OT_ACTION_CMD_MODEL_ParentNeedsUpdate);

				// Now we process the response and update the parents
				std::list<ot::UID> entityIDs = ot::rJSON::getULongLongList(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
				std::list<ot::UID> entityVersions = ot::rJSON::getULongLongList(doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
				std::list<ot::UID> entityInfoIdList = ot::rJSON::getULongLongList(doc, OT_ACTION_PARAM_MODEL_EntityInfoID);
				std::list<ot::UID> entityInfoVersionList = ot::rJSON::getULongLongList(doc, OT_ACTION_PARAM_MODEL_EntityInfoVersion);

				// This update of the parent entities may again result in parents which need to be updated
				modifiedEntities = updateParents(entityIDs, entityVersions, entityInfoIdList, entityInfoVersionList);
			}
		}
	}
}

std::list<ot::UID> UpdateManager::updateParents(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityVersions, std::list<ot::UID> &entityInfoIdList, 
											std::list<ot::UID> &entityInfoVersionList)
{
	std::list<std::pair<unsigned long long, unsigned long long>> entitiesToUpdate;

	while (!entityIDs.empty())
	{
		ot::UID entityID      = entityIDs.front();
		ot::UID entityVersion = entityVersions.front();

		entityIDs.pop_front();
		entityVersions.pop_front();

		entitiesToUpdate.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
	}

	std::map<unsigned long long, unsigned long long> entityVersionMap;

	while (!entityInfoIdList.empty())
	{
		ot::UID entityID      = entityInfoIdList.front();
		ot::UID entityVersion = entityInfoVersionList.front();

		entityInfoIdList.pop_front();
		entityInfoVersionList.pop_front();

		entityVersionMap[entityID] = entityVersion;
	}

	std::list<ot::UID> modifiedEntities;
	
	// Prefetch all necessary entities (we do not use the cache for these entities, so we need to prefetch all of the top level entities)
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(entitiesToUpdate);

	for (auto entity : entitiesToUpdate)
	{
		updateSingleParent(entity.first, entity.second, entityVersionMap, modifiedEntities);
	}

	return modifiedEntities;
}

void UpdateManager::updateSingleParent(ot::UID entityID, ot::UID entityVersion, std::map<ot::UID, ot::UID> &entityVersionMap, std::list<ot::UID> &modifiedEntities)
{
	ClassFactoryCAD classFactory;

	// This entity will be modified later on in the process, so this one is only temporary and should not be cached.
	// Therefore, we don't use the cache to load this entity
	EntityGeometry *geomEntity = dynamic_cast<EntityGeometry*>(modelComponent->readEntityFromEntityIDandVersion(entityID, entityVersion, classFactory));

	if (geomEntity == nullptr)
	{
		return;
	}

	EntityPropertiesString *typeProperty = dynamic_cast<EntityPropertiesString*>(geomEntity->getProperties().getProperty("shapeType"));
	if (typeProperty == nullptr)
	{
		delete geomEntity;
		geomEntity = nullptr;

		// We cannot update this entity 
		return;
	}

	// We create new entities for Brep and Facets
	geomEntity->resetBrep();
	geomEntity->resetFacets();

	ot::UID brepID = modelComponent->createEntityUID();
	ot::UID facetsID = modelComponent->createEntityUID();

	geomEntity->getBrepEntity()->setEntityID(brepID);
	geomEntity->getFacets()->setEntityID(facetsID);

	// Update the actual shape geometry
	TopoDS_Shape shape;
	bool success = updateParent(typeProperty->getValue(), geomEntity, shape, entityVersionMap);

	if (success)
	{
		gp_Trsf newTransform = Transformations::setTransform(geomEntity, shape, gp_Trsf());

		geomEntity->setBrep(shape);
		geomEntity->getBrepEntity()->setTransform(newTransform);

		// Now we facet the entity 
		geomEntity->facetEntity(false);

		// Finally store the brep and facets entities in the database
		geomEntity->storeBrep();

		ot::UID brepStorageVersion = geomEntity->getBrepEntity()->getEntityStorageVersion();

		entityCache->cacheEntity(geomEntity->getBrepEntity());
		geomEntity->detachBrep();

		geomEntity->storeFacets();

		ot::UID facetsStorageVersion = geomEntity->getFacets()->getEntityStorageVersion();

		geomEntity->releaseFacets();

		OT_rJSON_createDOC(requestDoc);
		ot::rJSON::add(requestDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdateGeometryEntity);
		ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID, geomEntity->getEntityID());
		ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID_Brep, brepID);
		ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID_Facets, facetsID);
		ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion);
		ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion);
		ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_OverrideGeometry, false);

		modelComponent->sendMessage(false, requestDoc);

		// Here the new geom entity will be stored in the model service, so we cannot cache it here.

		// This entity was modified
		modifiedEntities.push_back(geomEntity->getEntityID());
	}

	// We have not used the cache, so its safe to delete the entity
	delete geomEntity;
	geomEntity = nullptr;
}

bool UpdateManager::updateParent(const std::string &type, EntityGeometry *geomEntity, TopoDS_Shape &shape, std::map<ot::UID, ot::UID> &entityVersionMap)
{
	// First, get information about the base and tool properties
	EntityPropertiesString *baseShapeProperty = dynamic_cast<EntityPropertiesString*>(geomEntity->getProperties().getProperty("baseShape"));
	assert(baseShapeProperty != nullptr);

	ot::UID baseShapeID = std::stoull(baseShapeProperty->getValue());

	if (entityVersionMap.count(baseShapeID) == 0)
	{
		uiComponent->displayMessage("ERROR: Unable to update the operation to build shape: " + geomEntity->getName() + " (base shape is missing)\n\n");
		return false;
	}

	ot::UID baseShapeVersion = entityVersionMap[baseShapeID];

	EntityPropertiesString *toolShapeProperty = dynamic_cast<EntityPropertiesString*>(geomEntity->getProperties().getProperty("toolShapes"));
	assert(toolShapeProperty != nullptr);

	std::list<ot::UID> toolShapeID = splitString(toolShapeProperty->getValue());

	// Prefetch all necessary breps
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchData;
	prefetchData.push_back(std::pair<unsigned long long, unsigned long long>(baseShapeID, baseShapeVersion));

	int toolShapeCount = 0;

	for (auto entity : toolShapeID)
	{
		if (entityVersionMap.count(entity) != 0)
		{
			toolShapeCount++;

			prefetchData.push_back(std::pair<unsigned long long, unsigned long long>(entity, entityVersionMap[entity]));
		}
	}

	if (toolShapeCount == 0)
	{
		uiComponent->displayMessage("ERROR: Unable to update the operation to build shape: " + geomEntity->getName() + " (no tool shape is present)\n\n");
		return false;
	}

	entityCache->removeCachedEntitiesFromList(prefetchData);
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchData);

	// Load the base and tool breps
	ClassFactoryCAD classFactory;

	EntityGeometry *baseShape = dynamic_cast<EntityGeometry*>(entityCache->getEntity(baseShapeID, baseShapeVersion));
	assert(baseShape != nullptr);

	ot::UID baseBrepID = baseShape->getBrepStorageObjectID();

	assert(entityVersionMap.count(baseBrepID) != 0);
	ot::UID baseBrepVersion = entityVersionMap[baseBrepID];

	// We do not delete the base shape here, since it is managed by the entity Cache

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchBrepData;
	prefetchBrepData.push_back(std::pair<unsigned long long, unsigned long long>(baseBrepID, baseBrepVersion));

	std::list<std::pair<unsigned long long, unsigned long long>> toolBrepsIds;
	for (auto entityID : toolShapeID)
	{
		assert(entityVersionMap.count(entityID) != 0);

		EntityGeometry *toolShape = dynamic_cast<EntityGeometry*>(entityCache->getEntity(entityID, entityVersionMap[entityID]));
		assert(toolShape != nullptr);

		assert(entityVersionMap.count(toolShape->getBrepStorageObjectID()) != 0);

		toolBrepsIds.push_back(std::pair<unsigned long long, unsigned long long>(toolShape->getBrepStorageObjectID(), entityVersionMap[toolShape->getBrepStorageObjectID()]));
		prefetchBrepData.push_back(std::pair<unsigned long long, unsigned long long>(toolShape->getBrepStorageObjectID(), entityVersionMap[toolShape->getBrepStorageObjectID()]));

		// We do not delete the tool shape here, since it is managed by the entity Cache
	}

	// Now we prefetch the breps
	EntityBrep *baseBrep = dynamic_cast<EntityBrep*>(entityCache->getEntity(baseBrepID, baseBrepVersion));
	assert(baseBrep != nullptr);

	std::list<EntityBrep *> toolBreps;
	for (auto brep : toolBrepsIds)
	{
		EntityBrep *toolBrep = dynamic_cast<EntityBrep*>(entityCache->getEntity(brep.first, brep.second));
		assert(toolBrep != nullptr);

		toolBreps.push_back(toolBrep);
	}

	// Perform the operation to build the new shape
	std::string treeIconVisible, treeIconHidden;
	bool success = getBooleanOperations()->performOperation(type, baseBrep, toolBreps, shape, treeIconVisible, treeIconHidden);

	if (success)
	{
		// Ensure that we have a valid solid topology
		BRepBuilderAPI_MakeSolid ms;
		TopExp_Explorer exp;
		for (exp.Init(shape, TopAbs_SHELL); exp.More(); exp.Next())
		{
			ms.Add(TopoDS::Shell(exp.Current()));
		}

		shape = ms.Shape();
	}

	// We do not need to delete the breps here, since they are handled by the entityCache
	toolBreps.clear();

	return success;
}

std::list<ot::UID> UpdateManager::splitString(std::string value)
{
	std::list<ot::UID> ids;

	while (!value.empty())
	{
		size_t index = value.find(',');
		assert(index != value.npos);

		std::string id = value.substr(0, index);
		value = value.substr(index + 1);

		ids.push_back(std::stoull(id));
	}

	return ids;
}

std::list<ot::UID> UpdateManager::updateEntities(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityVersions, std::list<ot::UID> &brepVersions, bool itemsVisible)
{
	std::list<std::pair<unsigned long long, unsigned long long>> entitiesToUpdate;

	while (!entityIDs.empty())
	{
		ot::UID entityID      = entityIDs.front();
		ot::UID entityVersion = entityVersions.front();

		entityIDs.pop_front();
		entityVersions.pop_front();

		entitiesToUpdate.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
	}

	// Prefetch all necessary entities (we do not use the cache here, so we need to prefetch all the entities)
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(entitiesToUpdate);

	std::list<ot::UID> modifiedEntities;

	for (auto entity : entitiesToUpdate)
	{
		ot::UID brepVersion = brepVersions.front();
		brepVersions.pop_front();

		updateSingleEntity(entity.first, entity.second, brepVersion, itemsVisible, modifiedEntities);
	}

	return modifiedEntities;
}


void UpdateManager::updateSingleEntity(ot::UID entityID, ot::UID entityVersion, ot::UID brepVersion, bool itemsVisible, std::list<ot::UID> &modifiedEntities)
{
	ClassFactoryCAD classFactory;
	// This entity will be modified later on in the process, so this one is only temporary and should not be cached.
	// Therefore, we don't use the cache to load this entity
	EntityGeometry *geomEntity = dynamic_cast<EntityGeometry*>(modelComponent->readEntityFromEntityIDandVersion(entityID, entityVersion, classFactory));
	if (geomEntity == nullptr)
	{
		// We cannot update this entity
		return;
	}

	EntityPropertiesBoolean *geometryNeedsUpdateProperty = dynamic_cast<EntityPropertiesBoolean*>(geomEntity->getProperties().getProperty("geometryNeedsUpdate"));
	EntityPropertiesBoolean *transformNeedsUpdateProperty = dynamic_cast<EntityPropertiesBoolean*>(geomEntity->getProperties().getProperty("transformNeedsUpdate"));

	if (geometryNeedsUpdateProperty != nullptr && transformNeedsUpdateProperty != nullptr)
	{
		if (!geometryNeedsUpdateProperty->getValue()) 
		{
			if (transformNeedsUpdateProperty->getValue())
			{
				// We need to update the transformation only
				EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(geomEntity->getBrepStorageObjectID(), brepVersion));
				if (brepEntity == nullptr) return;

				TopoDS_Shape shape = brepEntity->getBrep();

				gp_Trsf newTransform = Transformations::setTransform(geomEntity, shape, brepEntity->getTransform());

				geomEntity->resetBrep();
				geomEntity->resetFacets();

				ot::UID brepID = modelComponent->createEntityUID();
				ot::UID facetsID = modelComponent->createEntityUID();

				geomEntity->getBrepEntity()->setEntityID(brepID);
				geomEntity->getFacets()->setEntityID(facetsID);

				geomEntity->setBrep(shape);
				geomEntity->getBrepEntity()->setTransform(newTransform);

				// Now we facet the entity 
				geomEntity->facetEntity(false);

				// Finally store the brep and facets entities in the database
				geomEntity->storeBrep();

				ot::UID brepStorageVersion = geomEntity->getBrepEntity()->getEntityStorageVersion();

				entityCache->cacheEntity(geomEntity->getBrepEntity());
				geomEntity->detachBrep();

				geomEntity->storeFacets();
				ot::UID facetsStorageVersion = geomEntity->getFacets()->getEntityStorageVersion();

				geomEntity->releaseFacets();

				OT_rJSON_createDOC(requestDoc);
				ot::rJSON::add(requestDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdateGeometryEntity);
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID, geomEntity->getEntityID());
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID_Brep, brepID);
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID_Facets, facetsID);
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion);
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion);
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_OverrideGeometry, true);	// Here we can replace the geometry entity, since it was 
																							// already written during this operation as a result to the 
																							// parameter change. This means that we must not cache this geometry entity.
				modelComponent->sendMessage(false, requestDoc);

				// This entity was modified
				modifiedEntities.push_back(geomEntity->getEntityID());

				// Delete the geometry entity (will be updated later in the model). We have created a copy of the cache item, so its safe to delete. 
				delete geomEntity;
				geomEntity = nullptr;
			}
			else
			{
				delete geomEntity;
				geomEntity = nullptr;

				// The shape geometry or transformation do not need to be updated
				return;
			}
		}
	}

	EntityPropertiesString *typeProperty = dynamic_cast<EntityPropertiesString*>(geomEntity->getProperties().getProperty("shapeType"));
	if (typeProperty == nullptr)
	{
		delete geomEntity;
		geomEntity = nullptr;

		// We cannot update this entity
		return;
	}

	// Now check whether we have a primitive shape which can be updated below
	if (!getPrimitiveManager()->isPrimitiveShape(typeProperty->getValue()))
	{
		delete geomEntity;
		geomEntity = nullptr;

		// We do not need to update this entity
		return;
	}

	// In the following, we update the shape primitive by rebuilding it from its paramters
	// We create new entities for Brep and Facets
	geomEntity->resetBrep();
	geomEntity->resetFacets();

	ot::UID brepID = modelComponent->createEntityUID();
	ot::UID facetsID = modelComponent->createEntityUID();

	geomEntity->getBrepEntity()->setEntityID(brepID);
	geomEntity->getFacets()->setEntityID(facetsID);

	// Update the actual shape geometry
	TopoDS_Shape shape;
	getPrimitiveManager()->updateShape(typeProperty->getValue(), geomEntity, shape);

	gp_Trsf newTransform = Transformations::setTransform(geomEntity, shape, gp_Trsf());

	geomEntity->setBrep(shape);
	geomEntity->getBrepEntity()->setTransform(newTransform);

	// Now we facet the entity 
	geomEntity->facetEntity(false);

	// Finally store the brep and facets entities in the database
	geomEntity->storeBrep();

	ot::UID brepStorageVersion = geomEntity->getBrepEntity()->getEntityStorageVersion();

	entityCache->cacheEntity(geomEntity->getBrepEntity());
	geomEntity->detachBrep();

	geomEntity->storeFacets();
	ot::UID facetsStorageVersion = geomEntity->getFacets()->getEntityStorageVersion();

	geomEntity->releaseFacets();

	OT_rJSON_createDOC(requestDoc);
	ot::rJSON::add(requestDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdateGeometryEntity);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID, geomEntity->getEntityID());
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID_Brep, brepID);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityID_Facets, facetsID);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion);
	ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_OverrideGeometry, true);   // Here we can replace the geometry entity, since it was 
																				// already written during this operation as a result to the 
																				// parameter change
	modelComponent->sendMessage(false, requestDoc);

	// This entity was modified
	modifiedEntities.push_back(geomEntity->getEntityID());

	// Delete the geometry entity (will be updated later in the model)
	delete geomEntity;
	geomEntity = nullptr;
}

