// @otlicense

#pragma once

#include "EdgesOperationBase.h"

#include "OldTreeIcon.h"
#include "ShapesBase.h"

#include "TopoDS_TShape.hxx"
#include "TopTools_ListOfShape.hxx"

class Model;
class EntityGeometry;
class EntityBrep;
class UpdateManager;

class TopoDS_Shape;
class BRepTools_History;

class BlendEdges : public EdgesOperationBase
{
public:
	BlendEdges(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache) 
		: EdgesOperationBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache) {};
	virtual ~BlendEdges() {}

protected:
	void addSpecificProperties(EntityGeometry* geometryEntity);
	ot::components::UiComponent::entitySelectionAction getSelectionAction();
	std::string getOperationDescription();
	std::string getVisibleTreeItemName();
	std::string getHiddenTreeItemName();
	std::string getShapeType();
	bool operationActive(EntityGeometry* geometryEntity);
	bool performActualOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation, TopoDS_Shape& shape, TopTools_ListOfShape &listOfProcessedEdges, BRepTools_History*& history);
};
