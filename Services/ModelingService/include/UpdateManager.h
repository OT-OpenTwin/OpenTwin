#pragma once

#include "OTCore/CoreTypes.h"
#include "Types.h"

#include <list>
#include <map>

class EntityGeometry;
class TopoDS_Shape;
class EntityCache;
class PrimitiveManager;
class BooleanOperations;

namespace ot
{
	namespace components
	{
		class UiComponent;
		class ModelComponent;
	}
}

class UpdateManager
{
public:
	UpdateManager(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, EntityCache *_entityCache, PrimitiveManager *_primitiveManager, BooleanOperations *_booleanOperations);
	~UpdateManager() {};

	void checkParentUpdates(std::list<ot::UID> modifiedEntities);
	std::list<ot::UID> updateEntities(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityVersions, std::list<ot::UID> &brepVersions, bool itemsVisible);

	UpdateManager() = delete;

private:
	std::list<ot::UID> updateParents(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityInfoIdList, std::list<ot::UID> &entityInfoVersionList, std::list<ot::UID> &brepVersions);
	void updateSingleParent(ot::UID entityID, ot::UID entityVersion, std::map<ot::UID, ot::UID> &entityVersionMap, std::list<ot::UID> &modifiedEntities);
	bool updateParent(const std::string &type, EntityGeometry *geomEntity, TopoDS_Shape &shape, std::map<ot::UID, ot::UID> &entityVersionMap);

	std::list<ot::UID> splitString(std::string value);

	void updateSingleEntity(ot::UID entityID, ot::UID entityVersion, ot::UID brepVersion, bool itemsVisible, std::list<ot::UID> &modifiedEntities);

	PrimitiveManager *getPrimitiveManager(void) { assert(primitiveManager != nullptr); return primitiveManager; }
	BooleanOperations *getBooleanOperations(void) { assert(booleanOperations != nullptr); return booleanOperations; }

	ot::components::UiComponent *uiComponent;
	ot::components::ModelComponent *modelComponent;
	EntityCache *entityCache;
	PrimitiveManager *primitiveManager;
	BooleanOperations *booleanOperations;
};
