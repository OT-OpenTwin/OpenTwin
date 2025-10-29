// @otlicense

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
