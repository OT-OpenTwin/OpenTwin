#pragma once

#include "Types.h"
#include "ShapesBase.h"

class Model;
class EntityGeometry;
class TopoDS_Shape;
class UpdateManager;

class SimplifyRemoveFaceData
{
public:
	SimplifyRemoveFaceData() : entityID(0), position{ 0.0, 0.0, 0.0 } {}
	virtual ~SimplifyRemoveFaceData() {}

	void setEntityID(ot::UID id) { entityID = id; }
	void setPosition(double x, double y, double z) { position[0] = x; position[1] = y; position[2] = z; }

	ot::UID getEntityID(void) { return entityID; }

	double getX(void) { return position[0]; }
	double getY(void) { return position[1]; }
	double getZ(void) { return position[2]; }

private:
	ot::UID entityID;
	double position[3];
};

class SimplifyRemoveFaces : public ShapesBase
{
public:
	SimplifyRemoveFaces(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache, ClassFactory *_classFactory) 
		: ShapesBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory) {};
	virtual ~SimplifyRemoveFaces() {}

	void setUpdateManager(UpdateManager *_updateManager) { updateManager = _updateManager; };

	void enterRemoveFacesMode(void);
	void performOperation(const std::string &selectionInfo);

private:
	bool removeFacesFromEntity(EntityGeometry *geometryEntity, ot::UID brepID, ot::UID brepVersion, std::list<SimplifyRemoveFaceData> &faces, std::list<ot::UID> &modifiedEntities);
	bool findFaceFromPosition(TopoDS_Shape &shape, double x, double y, double z, TopoDS_Shape &face);
	UpdateManager *getUpdateManager(void) { assert(updateManager != nullptr); return updateManager; }

	UpdateManager *updateManager;
};
