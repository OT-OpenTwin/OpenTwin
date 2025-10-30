// @otlicense
// File: HandleArrow.h
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

class Viewer;

class HandleArrow : public HandlerBase
{
public:
	HandleArrow() = delete;
	HandleArrow(osg::Vec3d point, osg::Vec3d dir, double color[4], double length, double radius);
	virtual ~HandleArrow();

	virtual osg::Node *getNode(void) override;

	virtual void mouseOver(bool flag) override;
	virtual void setInteraction(Viewer* viewer, int intersectionIndex, double intersectionRatio) override;
	virtual void setInteractionReference(int intersectionIndex, double intersectionRatio) override;

	void setPosition(osg::Vec3d pos);
	osg::Vec3d getPosition(void) { return position; }

private:
	void createArrow(osg::Geode * _geode, double color[4], double length, double radius);
	void updateTransform(void);

	osg::ref_ptr<osg::MatrixTransform>  osgNode;
	osg::ref_ptr<osg::Switch>			osgSwitchNode;
	osg::ref_ptr<osg::Geode>			arrowNode;
	osg::ref_ptr<osg::Geode>			arrowNodeSelected;

	osg::Vec3d position;
	osg::Vec3d direction;

	double intersectionLineLength;
	double referenceOffset;
};

