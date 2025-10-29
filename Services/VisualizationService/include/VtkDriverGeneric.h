// @otlicense

#pragma once

#include <string>
#include <ctime>

#include "VtkDriver.h"

class VtkDriverGeneric : public VtkDriver
{
public:
	VtkDriverGeneric();
	virtual ~VtkDriverGeneric();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;

private:
	enum tType {ARROWS, CONTOUR, CARPET, ISOLINES};

	void setVisualizationType(tType type) { visualizationType = type; }
	void setNormal(double nx, double ny, double nz) { normal[0] = nx; normal[1] = ny; normal[2] = nz; };
	void setCenter(double cx, double cy, double cz) { center[0] = cx; center[1] = cy; center[2] = cz; };

	std::string dataName;
	tType visualizationType;
	double normal[3];
	double center[3];
};
