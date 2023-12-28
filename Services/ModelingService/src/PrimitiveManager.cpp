#include "PrimitiveManager.h"

#include "PrimitiveCuboid.h"
#include "PrimitiveCylinder.h"
#include "PrimitiveCone.h"
#include "PrimitiveSphere.h"
#include "PrimitiveTorus.h"
#include "PrimitivePyramid.h"

PrimitiveManager::PrimitiveManager(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent,
								   const std::string &_serviceName, ot::serviceID_t _serviceID, EntityCache *_entityCache, ClassFactory *_classFactory) : 
	uiComponent(_uiComponent),
	modelComponent(_modelComponent),
	serviceName(_serviceName),
	entityCache(_entityCache),
	classFactory(_classFactory)
{
	cone     = new PrimitiveCone(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory);
	cuboid   = new PrimitiveCuboid(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory);
	cylinder = new PrimitiveCylinder(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory);
	pyramid  = new PrimitivePyramid(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory);
	sphere   = new PrimitiveSphere(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory);
	torus    = new PrimitiveTorus(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache, _classFactory);
}

PrimitiveManager::~PrimitiveManager()
{
	delete cone;     cone = nullptr;
	delete cuboid;   cuboid = nullptr;
	delete cylinder; cylinder = nullptr;
	delete pyramid;  pyramid = nullptr;
	delete sphere;   sphere = nullptr;
	delete torus;    torus = nullptr;
}

bool PrimitiveManager::isPrimitiveShape(const std::string &type)
{
	if (type == "Cuboid" || type == "Cylinder" || type == "Sphere" || type == "Torus" || type == "Cone" || type == "Pyramid")
	{
		return true;
	}

	return false;
}

void PrimitiveManager::updateShape(const std::string &type, EntityGeometry *geomEntity, TopoDS_Shape &shape)
{
	if (type == "Cuboid")
	{
		getCuboid()->update(geomEntity, shape);
	}
	else if (type == "Cylinder") 
	{
		getCylinder()->update(geomEntity, shape);
	}
	else if (type == "Sphere") 
	{
		getSphere()->update(geomEntity, shape);
	}
	else if (type == "Torus") 
	{
		getTorus()->update(geomEntity, shape);
	}
	else if (type == "Cone") 
	{
		getCone()->update(geomEntity, shape);
	}
	else if (type == "Pyramid") 
	{
		getPyramid()->update(geomEntity, shape);
	}
	else
	{
		assert(0); // Unknown type
	}
}

void PrimitiveManager::createFromRubberbandJson(const std::string &note, const std::string& _json, std::vector<double> &_transform)
{
	if (note == "ModelingService.Cuboid") {
		getCuboid()->createFromRubberbandJson(_json, _transform);
	}
	else if (note == "ModelingService.Cylinder") {
		getCylinder()->createFromRubberbandJson(_json, _transform);
	}
	else if (note == "ModelingService.Sphere") {
		getSphere()->createFromRubberbandJson(_json, _transform);
	}
	else if (note == "ModelingService.Torus") {
		getTorus()->createFromRubberbandJson(_json, _transform);
	}
	else if (note == "ModelingService.Cone") {
		getCone()->createFromRubberbandJson(_json, _transform);
	}
	else if (note == "ModelingService.Pyramid") {
		getPyramid()->createFromRubberbandJson(_json, _transform);
	}
	else {
		assert(0);
	}
}