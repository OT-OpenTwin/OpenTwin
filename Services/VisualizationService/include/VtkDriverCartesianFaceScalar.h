// @otlicense
// File: VtkDriverCartesianFaceScalar.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
