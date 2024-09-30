#pragma once

#include "Types.h"
#include "ShapesBase.h"

#include "TopoDS_TShape.hxx"

class Model;
class EntityGeometry;
class EntityBrep;
class TopoDS_Shape;
class UpdateManager;

class ChamferEdgesData
{
public:
	ChamferEdgesData() : entityID(0) {}
	virtual ~ChamferEdgesData() {}

	void setEntityID(ot::UID id) { entityID = id; }

	ot::UID getEntityID(void) { return entityID; }

	void setEdgeName(const std::string& name) { edgeName = name; }
	std::string getEdgeName(void) { return edgeName; }

private:
	ot::UID entityID;
	std::string edgeName;
};


class ChamferEdges : public ShapesBase
{
public:
	ChamferEdges(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache, ClassFactory *_classFactory) 
		: ShapesBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory) {};
	virtual ~ChamferEdges() {}

	void setUpdateManager(UpdateManager *_updateManager) { updateManager = _updateManager; };

	void enterChamferEdgesMode(void);
	void performOperation(const std::string &selectionInfo);

	void updateShape(EntityGeometry* geometryEntity, TopoDS_Shape& shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);

private:
	UpdateManager *getUpdateManager(void) { assert(updateManager != nullptr); return updateManager; }
	void storeEdgeListInProperties(std::list<ChamferEdgesData>& edgeList, EntityProperties& properties);
	std::list<ChamferEdgesData> readEdgeListFromProperties(EntityProperties& properties);
	void performOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, TopoDS_Shape& shape, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& resultFaceNames);

	UpdateManager *updateManager;
};
