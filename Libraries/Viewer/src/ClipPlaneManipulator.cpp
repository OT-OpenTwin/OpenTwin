// @otlicense
// File: ClipPlaneManipulator.cpp
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

#include "stdafx.h"
#include "ClipPlaneManipulator.h"

#include "Viewer.h"
#include "Model.h"
#include "HandleArrow.h"

ClipPlaneManipulator::ClipPlaneManipulator(Viewer *viewer, osg::Vec3 &point, osg::Vec3 &normal)
{
	viewer3D = viewer;
	planePoint = point;
	planeNormal = normal;

	double color[] = { 1.0, 0.0, 0.0, 1.0 };

	double radius = viewer3D->getModel()->getOSGRootNode()->computeBound().radius();

	arrowForward = new HandleArrow(point, normal, color, 0.15 * radius, 0.02 * radius);
	arrowBackward = new HandleArrow(point, -normal, color, 0.15 * radius, 0.02 * radius);

	arrowForward->addNotifier(this);
	arrowBackward->addNotifier(this);

	viewer3D->addHandler(arrowForward);
	viewer3D->addHandler(arrowBackward);
}

ClipPlaneManipulator::~ClipPlaneManipulator()
{
	viewer3D->removeHandler(arrowForward);
	viewer3D->removeHandler(arrowBackward);
}

void ClipPlaneManipulator::updateHandlers(void)
{
	arrowForward->setPosition(planePoint);
	arrowBackward->setPosition(planePoint);
}

void ClipPlaneManipulator::handlerInteraction(HandlerBase *handler)
{
	if (handler == arrowForward)
	{
		osg::Vec3 offset = arrowForward->getPosition() - planePoint;

		planePoint = planePoint + offset;

		viewer3D->updateClipPlane(planeNormal, planePoint);
		updateHandlers();
	}
	else if (handler == arrowBackward)
	{
		osg::Vec3 offset = arrowBackward->getPosition() - planePoint;

		planePoint = planePoint + offset;

		viewer3D->updateClipPlane(planeNormal, planePoint);
		updateHandlers();
	}

}
