// @otlicense

#pragma once

#include <string>
#include <vector>
#include <ctime>

#include "VtkDriver.h"

namespace osg
{
	class Node;
}

class VtkDriverCartesianFaceScalar : public VtkDriver
{
public:
	VtkDriverCartesianFaceScalar();
	virtual ~VtkDriverCartesianFaceScalar();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;

private:
	void buildPlane(bool primaryMesh, int iw, int nu, int nv, int nw, size_t mu, size_t mv, size_t mw, std::vector<double> &uc, std::vector<double> &vc, std::vector<double> &wc, std::vector<double> &values, size_t offset, double minValue, double maxValue, osg::Node *parent);

	int normalDirection;
	int cutIndex;
};
