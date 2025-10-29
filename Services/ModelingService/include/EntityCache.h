// @otlicense

#pragma once

class EntityBase;
namespace ot
{
	namespace components
	{
		class ModelComponent;
	}
}

#include "OTCore/CoreTypes.h"
#include "OldTreeIcon.h"

#include <map>
#include <list>

namespace ot
{
	class EntityInformation;
}

class Application;

class EntityCache
{
public:
	EntityCache();
	~EntityCache();

	void setApplication(Application* app) { application = app; }

	void cacheEntity(EntityBase *entity);
	EntityBase *getEntity(ot::UID entityID, ot::UID entityVersion);

	void setModelComponent(ot::components::ModelComponent *mdl) { modelComponent = mdl; }

	void removeCachedEntitiesFromList(std::list<std::pair<unsigned long long, unsigned long long>> &prefetchEntities);

	void shrinkCache(void);

	void removeEntity(EntityBase *entity);

	void prefetchEntities(std::list<ot::EntityInformation> &entityList);
	void prefetchEntities(std::list<std::pair<unsigned long long, unsigned long long>> &prefetchEntities);

private:
	int maxNumberOfCacheItems;
	std::map<std::pair<ot::UID, ot::UID>, EntityBase*> entityMap;
	std::list<EntityBase*> entityList;

	ot::components::ModelComponent *modelComponent;
	Application* application;
};

