#pragma once

class Viewer;
class HandleArrow;

#include <osg/Vec3>

#include "HandlerBase.h"

class ClipPlaneManipulator : public HandlerNotifier
{
public:
	ClipPlaneManipulator(Viewer *viewer, osg::Vec3 &point, osg::Vec3 &normal);
	virtual ~ClipPlaneManipulator();

	virtual void handlerInteraction(HandlerBase *handler) override;


private:
	void updateHandlers(void);

	Viewer *viewer3D;

	HandleArrow *arrowForward;
	HandleArrow *arrowBackward;

	osg::Vec3 planePoint;
	osg::Vec3 planeNormal;
};

