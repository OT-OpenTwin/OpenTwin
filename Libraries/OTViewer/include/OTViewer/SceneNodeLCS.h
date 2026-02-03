// @otlicense
// File: SceneNodeLCS.h
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

#include "SceneNodeBase.h"

#include <string>
#include <vector>

#include <osg/Matrix>

class SceneNodeLCS : public SceneNodeBase
{
public:
	SceneNodeLCS() : origin(3, 0.0), xAxis(3, 0.0), zAxis(3, 0.0) {};
	virtual ~SceneNodeLCS() {};

	virtual bool isItem3D(void) const override { return true; };

	void setOrigin(double x, double y, double z) { origin[0] = x; origin[1] = y; origin[2] = z; };
	void setX(double x, double y, double z) { xAxis[0] = x; xAxis[1] = y; xAxis[2] = z; };
	void setZ(double x, double y, double z) { zAxis[0] = x; zAxis[1] = y; zAxis[2] = z; };

	void updateTransformationMatrix();

	osg::Matrix getTransformation() { return transformation; }

private:
	osg::Matrix makeOrthonormalFrame(const osg::Vec3d& origin, osg::Vec3d xAxis, osg::Vec3d zAxis);

	std::vector<double> origin;
	std::vector<double> xAxis;
	std::vector<double> zAxis;

	osg::Matrix transformation;
};
