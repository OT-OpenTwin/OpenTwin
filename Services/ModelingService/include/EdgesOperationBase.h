#pragma once

#include "OldTreeIcon.h"
#include "ShapesBase.h"

#include "TopoDS_TShape.hxx"
#include "TopTools_ListOfShape.hxx"

class Model;
class EntityGeometry;
class EntityBrep;
class UpdateManager;

class TopoDS_Shape;
class TopoDS_Vertex;
class BRepTools_History;

class EdgesData
{
public:
	EdgesData() : entityID(0) {}
	virtual ~EdgesData() {}

	void setEntityID(ot::UID id) { entityID = id; }

	ot::UID getEntityID(void) { return entityID; }

	void setEdgeName(const std::string& name) { edgeName = name; }
	std::string getEdgeName(void) { return edgeName; }

private:
	ot::UID entityID;
	std::string edgeName;
};


class EdgesOperationBase : public ShapesBase
{
public:
	EdgesOperationBase(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache) 
		: ShapesBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache), updateManager(nullptr) {};
	virtual ~EdgesOperationBase() {}

	void setUpdateManager(UpdateManager *_updateManager) { updateManager = _updateManager; };

	void enterSelectEdgesMode(void);
	void performOperation(const std::string &selectionInfo);

	void updateShape(EntityGeometry* geometryEntity, TopoDS_Shape& shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);

private:
	UpdateManager *getUpdateManager(void) { assert(updateManager != nullptr); return updateManager; }
	void storeEdgeListInProperties(std::list<EdgesData>& edgeList, EntityProperties& properties);
	std::list<EdgesData> readEdgeListFromProperties(EntityProperties& properties);
	void performOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, TopoDS_Shape& shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);
	void storeInputShapeFaceNames(EntityBrep* baseBrep, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allFaceNames);
	void getAllEdgesFromInputShape(EntityBrep* baseBrep, std::map< std::string, const opencascade::handle<TopoDS_TShape>>& allEdges, std::map<const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesFace1, std::map<const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesFace2);
	void getAllEdgesForOperation(std::list<EdgesData>& edgeList, std::map< std::string, const opencascade::handle<TopoDS_TShape>>& allEdges, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation);
	void getAllEdgesForVertex(EntityBrep* baseBrep, TopoDS_Vertex& aVertex, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation, std::list<opencascade::handle<TopoDS_TShape>>& allEdgesForVertex);
	std::string getVertexNameFromEdges(std::list<opencascade::handle<TopoDS_TShape>>& allEdgesForVertex, std::map<const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesFace1, std::map<const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesFace2);

protected:
	void addParametricProperty(EntityGeometry* geometryEntity, const std::string& name, double value);

	virtual void addSpecificProperties(EntityGeometry* geometryEntity) = 0;
	virtual ot::components::UiComponent::entitySelectionAction getSelectionAction() = 0;
	virtual std::string getOperationDescription() = 0;
	virtual std::string getVisibleTreeItemName() = 0;
	virtual std::string getHiddenTreeItemName() = 0;
	virtual std::string getShapeType() = 0;
	virtual bool operationActive(EntityGeometry* geometryEntity) = 0;
	virtual bool performActualOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation, TopoDS_Shape& shape, TopTools_ListOfShape& listOfProcessedEdges, BRepTools_History*& history) = 0;

	UpdateManager *updateManager;
};
