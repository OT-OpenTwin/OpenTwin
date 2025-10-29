// @otlicense

#pragma once

#include "ShapesBase.h"

class PrimitiveTorus : public ShapesBase
{
public:
	PrimitiveTorus(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache) 
		: ShapesBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache) {};
	~PrimitiveTorus() {};

	void sendRubberbandData(void);
	void createFromRubberbandJson(const std::string& _json, std::vector<double> &_transform);
	void update(EntityGeometry *geomEntity, TopoDS_Shape &shape);
};
