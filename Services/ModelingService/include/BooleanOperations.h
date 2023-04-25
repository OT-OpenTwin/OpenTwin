#pragma once

namespace ot
{
	class EntityInformation;
}

#include <list>
#include <string>

#include <OpenTwinFoundation/ModelComponent.h>
#include <OpenTwinFoundation/UiComponent.h>

class EntityBrep;
class EntityGeometry;
class EntityCache;
class TopoDS_Shape;

class BooleanOperations
{
public:
	BooleanOperations(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, const std::string &_serviceName, EntityCache *_entityCache, ot::serviceID_t _serviceID);
	~BooleanOperations() {};

	void enterAddMode(const std::list<ot::EntityInformation> &selectedGeometryEntities);
	void enterSubtractMode(const std::list<ot::EntityInformation> &selectedGeometryEntities);
	void enterIntersectMode(const std::list<ot::EntityInformation> &selectedGeometryEntities);
	void perfromOperationForSelectedEntities(const std::string &selectionAction, const std::string &selectionInfo, std::map<std::string, std::string> &options);
	bool performOperation(const std::string &selectionAction, EntityBrep *baseBrepEntity, std::list<EntityBrep *> &brepEntities, TopoDS_Shape &shape, std::string &treeIconVisible, std::string &treeIconHidden);

	BooleanOperations() = delete;

private:
	bool add(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape);
	bool subtract(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape);
	bool intersect(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape);
	void deletePropertyCategory(EntityGeometry *geometryEntity, const std::string category);

	ot::components::UiComponent *uiComponent;
	ot::components::ModelComponent *modelComponent;
	std::string serviceName;
	EntityCache *entityCache;
	ot::serviceID_t serviceID;
};
