#pragma once

#include "OTCore/CoreTypes.h"
#include "OldTreeIcon.h"

#include "TopoDS_TShape.hxx"

#include <list>
#include <map>

class EntityGeometry;
class TopoDS_Shape;
class EntityCache;
class PrimitiveManager;
class BooleanOperations;
class ChamferEdges;
class BlendEdges;
class ClassFactory;

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
	UpdateManager(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, EntityCache *_entityCache, PrimitiveManager *_primitiveManager, BooleanOperations *_booleanOperations, ChamferEdges* _chamferEdgesManager, BlendEdges* _blendEdgesManager, ClassFactory *_classFactory);
	~UpdateManager() {};

	void checkParentUpdates(std::list<ot::UID> modifiedEntities);
	std::list<ot::UID> updateEntities(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityVersions, std::list<ot::UID> &brepVersions, bool itemsVisible);

	UpdateManager() = delete;

private:
	std::list<ot::UID> updateParents(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityInfoIdList, std::list<ot::UID> &entityInfoVersionList, std::list<ot::UID> &brepVersions);
	void updateSingleParent(ot::UID entityID, ot::UID entityVersion, std::map<ot::UID, ot::UID> &entityVersionMap, std::list<ot::UID> &modifiedEntities);
	bool updateParent(const std::string& type, EntityGeometry* geomEntity, TopoDS_Shape& shape, std::map<ot::UID, ot::UID>& entityVersionMap, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);
	bool updateBooleanParent(const std::string& type, EntityGeometry* geomEntity, TopoDS_Shape& shape, std::map<ot::UID, ot::UID>& entityVersionMap, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);
	bool updateChamferEdgesParent(const std::string& type, EntityGeometry* geomEntity, TopoDS_Shape& shape, std::map<ot::UID, ot::UID>& entityVersionMap, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);
	bool updateBlendEdgesParent(const std::string& type, EntityGeometry* geomEntity, TopoDS_Shape& shape, std::map<ot::UID, ot::UID>& entityVersionMap, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);

	std::list<ot::UID> splitString(std::string value);

	void updateSingleEntity(ot::UID entityID, ot::UID entityVersion, ot::UID brepVersion, bool itemsVisible, std::list<ot::UID> &modifiedEntities);

	PrimitiveManager *getPrimitiveManager(void) { assert(primitiveManager != nullptr); return primitiveManager; }
	BooleanOperations* getBooleanOperations(void) { assert(booleanOperations != nullptr); return booleanOperations; }
	ChamferEdges* getChamferEdgesManager(void) { assert(chamferEdgesManager != nullptr); return chamferEdgesManager; }
	BlendEdges* getBlendEdgesManager(void) { assert(blendEdgesManager != nullptr); return blendEdgesManager; }

	ot::components::UiComponent *uiComponent;
	ot::components::ModelComponent *modelComponent;
	EntityCache *entityCache;
	PrimitiveManager *primitiveManager;
	BooleanOperations* booleanOperations;
	ChamferEdges* chamferEdgesManager;
	BlendEdges* blendEdgesManager;
	ClassFactory* classFactory;
};
