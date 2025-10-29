// @otlicense

#pragma once

#include <string>
#include <vector>

class EntityGeometry;
class TopoDS_Shape;
class EntityCache;

class PrimitiveCone;
class PrimitiveCuboid;
class PrimitiveCylinder;
class PrimitivePyramid;
class PrimitiveSphere;
class PrimitiveTorus;

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

class PrimitiveManager
{
public:
	PrimitiveManager(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, const std::string &_serviceName, ot::serviceID_t _serviceID, EntityCache *_entityCache);
	~PrimitiveManager();

	bool isPrimitiveShape(const std::string &type);
	void updateShape(const std::string &type, EntityGeometry *geomEntity, TopoDS_Shape &shape);

	void createFromRubberbandJson(const std::string &note, const std::string& _json, std::vector<double> &_transform);

	PrimitiveCone     *getCone(void)     { return cone;     }
	PrimitiveCuboid   *getCuboid(void)   { return cuboid;   }
	PrimitiveCylinder *getCylinder(void) { return cylinder; }
	PrimitivePyramid  *getPyramid(void)  { return pyramid;  }
	PrimitiveSphere   *getSphere(void)   { return sphere;   }
	PrimitiveTorus    *getTorus(void)    { return torus;    }

	PrimitiveManager() = delete;

private:
	ot::components::UiComponent *uiComponent;
	ot::components::ModelComponent *modelComponent;
	std::string serviceName;
	EntityCache *entityCache;

	PrimitiveCone     *cone;
	PrimitiveCuboid   *cuboid;
	PrimitiveCylinder *cylinder;
	PrimitivePyramid  *pyramid;
	PrimitiveSphere   *sphere;
	PrimitiveTorus    *torus;
};

