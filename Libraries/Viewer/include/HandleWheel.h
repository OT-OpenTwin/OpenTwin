// @otlicense
// File: HandleWheel.h
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

#include "HandlerBase.h"

#include <osg/Geode>

class HandleWheel : public HandlerBase
{
public:
	HandleWheel() = delete;
	HandleWheel(osg::Vec3d point, osg::Vec3d normal, double radius, double tubeRadius, double color[4]);
	virtual ~HandleWheel();

	virtual osg::Node *getNode(void) override;

	virtual void mouseOver(bool flag) override;
	virtual void setInteraction(Viewer *viewer, int intersectionIndex, double intersectionRatio) override;
	virtual void setInteractionReference(int intersectionIndex, double intersectionRatio) override;

	void setPosition(osg::Vec3d pos);
	osg::Vec3d getPosition(void) { return wheelPosition; }

	double getRotationAngleDeg(void) { return rotationAngleDeg; }

private:
	void createWheel(osg::Geode * _geode, double color[4], double wheelRadius, double tubeRadius, int numberSegments);
	void updateTransform(void);
	double calculateAngle(int intersectionIndex, double intersectionRatio);

	osg::ref_ptr<osg::MatrixTransform>  osgNode;
	osg::ref_ptr<osg::Switch>			osgSwitchNode;
	osg::ref_ptr<osg::Geode>			wheelNode;
	osg::ref_ptr<osg::Geode>			wheelNodeSelected;

	osg::Vec3d wheelPosition;
	osg::Vec3d wheelNormal;
	double wheelRadius;

	double segmentLength;
	int    numberSegments;
	double referenceAngleDeg;
	double rotationAngleDeg;
	double wheelRotationAngleDeg;
};

