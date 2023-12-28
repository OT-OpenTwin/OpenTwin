#include "EntityCache.h"
#include "EntityBase.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"
#include "DataBase.h"
#include "Application.h"

#include "OTServiceFoundation/ModelComponent.h"

EntityCache::EntityCache() : 
	maxNumberOfCacheItems(40),
	application(nullptr)
{

}

EntityCache::~EntityCache()
{
	for (auto entity : entityList)
	{
		entity->setParent(nullptr);  // We have detached this entity from the hierarchy
		delete entity;
	}

	entityList.clear();
	entityMap.clear();
}

void EntityCache::shrinkCache(void)
{
	// Here we delete entities until we have reached the maximum number of desired cache entities.
	while (entityList.size() > maxNumberOfCacheItems)
	{
		EntityBase *lastEntity = entityList.back();
		entityList.pop_back();

		entityMap.erase(std::pair<ot::UID, ot::UID>(lastEntity->getEntityID(), lastEntity->getEntityStorageVersion()));

		lastEntity->setParent(nullptr);  // We have detached this entity from the hierarchy

		delete lastEntity;
		lastEntity = nullptr;
	}
}

void EntityCache::cacheEntity(EntityBase *entity)
{
	// Now we check whether the entity is already in the map
	if (entityMap.count(std::pair<ot::UID, ot::UID>(entity->getEntityID(), entity->getEntityStorageVersion())) != 0)
	{
		// We already have this entity. Make sure that it gets on top of the list
		entityList.remove(entity);
		entityList.push_front(entity);
	}
	else
	{
		// Now we add the new entity to the top of the list
		entityList.push_front(entity);
		entityMap[std::pair<ot::UID, ot::UID>(entity->getEntityID(), entity->getEntityStorageVersion())] = entity;
	}
}

EntityBase *EntityCache::getEntity(ot::UID entityID, ot::UID entityVersion)
{
	EntityBase *entity = nullptr;

	if (entityMap.count(std::pair<ot::UID, ot::UID>(entityID, entityVersion)) == 0)
	{
		assert(modelComponent != nullptr);

		// We have not yet cached this item, so let's read it from the data base

		entity = modelComponent->readEntityFromEntityIDandVersion(entityID, entityVersion, application->getClassFactory());
	}
	else
	{
		entity = entityMap[std::pair<ot::UID, ot::UID>(entityID, entityVersion)];

		// Make sure that it gets on top of the list
		entityList.remove(entity);
		entityList.push_front(entity);
	}

	if (entity != nullptr)
	{
		cacheEntity(entity);
	}

	return entity;
}

void EntityCache::removeEntity(EntityBase *entity)
{
	entityMap.erase(std::pair<ot::UID, ot::UID>(entity->getEntityID(), entity->getEntityStorageVersion()));
	entityList.remove(entity);
}

void EntityCache::removeCachedEntitiesFromList(std::list<std::pair<unsigned long long, unsigned long long>> &prefetchEntities)
{
	std::list<std::pair<unsigned long long, unsigned long long>> necessaryEntities;

	for (auto entity : prefetchEntities)
	{
		if (entityMap.count(std::pair<ot::UID, ot::UID>(entity.first, entity.second)) == 0)
		{
			necessaryEntities.push_back(entity);
		}
	}

	prefetchEntities = necessaryEntities;
}

void EntityCache::prefetchEntities(std::list<ot::EntityInformation> &entityList)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchList;

	for (auto entity : entityList)
	{
		prefetchList.push_back(std::pair<unsigned long long, unsigned long long>(entity.getID(), entity.getVersion()));
	}

	prefetchEntities(prefetchList);
}

void EntityCache::prefetchEntities(std::list<std::pair<unsigned long long, unsigned long long>> &prefetchEntities)
{
	removeCachedEntitiesFromList(prefetchEntities);
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchEntities);
}
