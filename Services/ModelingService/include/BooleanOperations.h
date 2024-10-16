#pragma once

namespace ot
{
	class EntityInformation;
}

#include <list>
#include <string>

#include "ShapesBase.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include "TopoDS_TShape.hxx"
#include "TopTools_ListOfShape.hxx"

class EntityBrep;
class EntityGeometry;
class EntityCache;
class TopoDS_Shape;
class BRepAlgoAPI_BuilderAlgo;
class ClassFactory;

class BooleanOperations : public ShapesBase
{
public:
	BooleanOperations(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, const std::string &_serviceName, EntityCache *_entityCache, ot::serviceID_t _serviceID, ClassFactory* _classFactory);
	~BooleanOperations() {};

	void enterAddMode(const std::list<ot::EntityInformation> &selectedGeometryEntities);
	void enterSubtractMode(const std::list<ot::EntityInformation> &selectedGeometryEntities);
	void enterIntersectMode(const std::list<ot::EntityInformation> &selectedGeometryEntities);
	void perfromOperationForSelectedEntities(const std::string &selectionAction, const std::string &selectionInfo, std::map<std::string, std::string> &options);
	bool performOperation(const std::string &selectionAction, EntityBrep *baseBrepEntity, std::list<EntityBrep *> &brepEntities, TopoDS_Shape &shape, std::string &treeIconVisible, std::string &treeIconHidden, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);

	BooleanOperations() = delete;

private:
	bool add(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);
	bool subtract(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);
	bool intersect(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);
	void preOperation(TopoDS_Shape& shapeA, const std::map< const opencascade::handle<TopoDS_TShape>, std::string>& faceNamesA, TopoDS_Shape& shapeB, const std::map< const opencascade::handle<TopoDS_TShape>, std::string>& faceNamesB);
	std::map< const opencascade::handle<TopoDS_TShape>, std::string> postOperation(BRepAlgoAPI_BuilderAlgo& theAlgo, TopoDS_Shape& resultShape, TopoDS_Shape& shapeA, TopoDS_Shape& shapeB);

	TopTools_ListOfShape anArguments;
	std::map< const opencascade::handle<TopoDS_TShape>, std::string> allFaceNames;
};
