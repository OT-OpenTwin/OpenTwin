// @otlicense

#pragma once

#include "OldTreeIcon.h"
#include "ShapesBase.h"

#include "BRepAlgoAPI_Defeaturing.hxx"

class Model;
class EntityGeometry;
class EntityBrep;
class TopoDS_Shape;
class UpdateManager;

class SimplifyRemoveFaceData
{
public:
	SimplifyRemoveFaceData() : entityID(0) {}
	virtual ~SimplifyRemoveFaceData() {}

	void setEntityID(ot::UID id) { entityID = id; }
	void setFaceName(const std::string & _faceName) { faceName = _faceName; }

	ot::UID getEntityID(void) { return entityID; }
	std::string getFaceName(void) { return faceName; }

private:
	ot::UID entityID;
	std::string faceName;
};

class SimplifyRemoveFaces : public ShapesBase
{
public:
	SimplifyRemoveFaces(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache) 
		: ShapesBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache), updateManager(nullptr) {};
	virtual ~SimplifyRemoveFaces() {}

	void setUpdateManager(UpdateManager *_updateManager) { updateManager = _updateManager; };

	void enterRemoveFacesMode(void);
	void performOperation(const std::string &selectionInfo);

private:
	bool removeFacesFromEntity(EntityGeometry *geometryEntity, ot::UID brepID, ot::UID brepVersion, std::list<SimplifyRemoveFaceData> &faces, std::list<ot::UID> &modifiedEntities);
	bool findFaceFromName(EntityBrep* brepEntity, TopoDS_Shape& shape, const std::string& faceName, TopoDS_Shape& face);
	UpdateManager *getUpdateManager(void) { assert(updateManager != nullptr); return updateManager; }
	void handleFaceNaming(EntityBrep* inputEntity, EntityBrep* resultEntity, BRepAlgoAPI_Defeaturing& theAlgo);

	UpdateManager *updateManager;
};
