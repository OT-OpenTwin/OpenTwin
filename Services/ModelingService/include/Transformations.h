#pragma once

namespace ot
{
	class EntityInformation;
}

#include "ShapesBase.h"

#include <list>
#include <string>

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include <gp_Trsf.hxx>

class EntityBrep;
class TopoDS_Shape;
class EntityGeometry;
class EntityCache;
class UpdateManager;

class Transformations : public ShapesBase
{
public:
	Transformations(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache, ClassFactory *_classFactory) 
		: ShapesBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory),
		updateManager(nullptr) {};
	~Transformations() {};

	void setUpdateManager(UpdateManager *_updateManager) { updateManager = _updateManager; };

	void enterTransformMode(const std::list<ot::EntityInformation> &selectedGeometryEntities);
	void transformEntities(const std::string &selectionInfo, std::map<std::string, std::string> &options);

	static gp_Trsf setTransform(EntityGeometry *geomEntity, TopoDS_Shape &shape, gp_Trsf prevTransform);

	Transformations() = delete;

private:
	void updateTransformationProperties(EntityGeometry *geometryEntity, gp_XYZ transformTranslate, gp_XYZ transformAxis, double transformAngle, gp_XYZ rotationCenter);
	UpdateManager *getUpdateManager(void) { assert(updateManager != nullptr); return updateManager; }

	UpdateManager *updateManager;
};
