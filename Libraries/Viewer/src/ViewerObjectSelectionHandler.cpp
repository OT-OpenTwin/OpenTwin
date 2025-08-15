#include "stdafx.h"

#include "ViewerObjectSelectionHandler.h"
#include "LineIntersector.h"
#include "HandlerIntersector.h"
#include "Model.h"
#include "Viewer.h"
#include "Rubberband.h"
#include "ViewManipulator.h"
#include "HandlerBase.h"

#include <qapplication.h>

#include "OTSystem/OTAssert.h"

#include <osgViewer/Viewer>

#include <rbeCore/Step.h>
#include <rbeWrapper/RubberbandOsgWrapper.h>

#include <osgUtil/IntersectionVisitor>


int stepCount = 0;

osgUtil::Intersector *ViewerObjectSelectionHandler::createIntersector(osgUtil::Intersector::CoordinateFrame cf, double x, double y)
{
	osgUtil::Intersector *intersector = nullptr;

	if (model->getCurrentSelectionMode() == Model::EDGE)
	{
		double delta = 5.0; // this is the selection tolerance in pixes (necessary since edges are infinitely thin)

		osgUtil::PolytopeIntersector* polytopeIntersector = new osgUtil::PolytopeIntersector(cf, x - delta, y - delta, x + delta, y + delta);
		//polytopeIntersector->setDimensionMask(osgUtil::PolytopeIntersector::DimOne);

		intersector = polytopeIntersector;
	}
	else
	{
		if (model->isWireFrameMode())
		{
			LineIntersector* lineIntersector = new LineIntersector(cf, x, y);
			double size = model->getOSGRootNode()->computeBound().radius();
			lineIntersector->setOffset(size * 0.01);

			intersector = lineIntersector;
		}
		else
		{
			intersector = new osgUtil::LineSegmentIntersector(cf, x, y);
		}
	}

	return intersector;
}

bool ViewerObjectSelectionHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (!active) return false;

	osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	osgGA::ViewManipulator *manipulator = dynamic_cast<osgGA::ViewManipulator *>(viewer->getCameraManipulator());

	if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		if (creator->hasHandlers())
		{

		}
		else if (creator->getRubberband()) {
			if (!creator->getRubberband()->switchToNextStep()) {
				creator->getRubberband()->applyCurrentStep();
				creator->finishRubberbandExecution();

				stepCount = 0;
			}
			else
			{
				stepCount++;
			}
		}
		else {
			bool bCtrlKeyPressed = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL;

			osg::ref_ptr<osgUtil::Intersector> intersector = createIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
			
			osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

			osgUtil::IntersectionVisitor iv(intersector.get());
			iv.setTraversalMask(model->getCurrentTraversalMask());
			viewer->getCamera()->accept(iv);

			if (!intersector->containsIntersections() && model->isWireFrameMode())
			{
				// Let's try to pick a face
				intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

				osgUtil::IntersectionVisitor iv(intersector.get());
				iv.setTraversalMask(model->getCurrentTraversalMask());
				viewer->getCamera()->accept(iv);
			}

			model->processCurrentSelectionMode(intersector, viewer->getSceneData()->getBound().radius(), bCtrlKeyPressed);
		}
	}
	else if (ea.getEventType() & osgGA::GUIEventAdapter::DOUBLECLICK && ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		// Here we move the view center
		osg::ref_ptr<osgUtil::Intersector> intersector = createIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

		osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

		osgUtil::IntersectionVisitor iv(intersector.get());
		iv.setTraversalMask(model->getFaceSelectionTraversalMask());
		viewer->getCamera()->accept(iv);

		if (!intersector->containsIntersections() && model->isWireFrameMode())
		{
			// Let's try to pick a face
			intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

			osgUtil::IntersectionVisitor iv(intersector.get());
			iv.setTraversalMask(model->getCurrentTraversalMask());
			viewer->getCamera()->accept(iv);
		}

		osgUtil::LineSegmentIntersector* lineIntersector = dynamic_cast<osgUtil::LineSegmentIntersector*>(intersector.get());

		if (intersector->containsIntersections() && lineIntersector != nullptr)
		{
			const osgUtil::LineSegmentIntersector::Intersection &firstHit = *(lineIntersector->getIntersections().begin());
			osg::Vec3d intersectionPoint = firstHit.getWorldIntersectPoint();

			manipulator->setCenter(intersectionPoint);
			model->centerMouseCursor();
		}
	}
	else if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && creator->getCurrentHandler() != nullptr)
	{
		// We have an active handler, the mouse is moving and the left mouse button is down
		// We need to calculate the intersection of the current ray with the handler selection path geometry

		osg::ref_ptr<HandlerIntersector> intersector = new HandlerIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
		intersector->setHandlerDrawable(creator->getCurrentHandler()->getSelectorDrawable());

		osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

		osgUtil::IntersectionVisitor iv(intersector.get());
		iv.setTraversalMask(model->getCurrentTraversalMask());

		creator->getCurrentHandler()->beginInteraction();
		viewer->getCamera()->accept(iv);
		creator->getCurrentHandler()->endInteraction();

		if (intersector->containsIntersections())
		{
			const osgUtil::LineSegmentIntersector::Intersection &firstHit = *(intersector->getIntersections().begin());
			int intersectionIndex = firstHit.indexList.front();
			double intersectionRatio = firstHit.ratio;

			creator->getCurrentHandler()->setInteractionReference(intersectionIndex, intersectionRatio);
		}
	}
	else if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && creator->getCurrentHandler() != nullptr)
	{
		// We have an active handler, the mouse is moving and the left mouse button is down
		// We need to calculate the intersection of the current ray with the handler selection path geometry

		osg::ref_ptr<HandlerIntersector> intersector = new HandlerIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
		intersector->setHandlerDrawable(creator->getCurrentHandler()->getSelectorDrawable());

		osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

		osgUtil::IntersectionVisitor iv(intersector.get());
		iv.setTraversalMask(model->getCurrentTraversalMask());

		creator->getCurrentHandler()->beginInteraction();
		viewer->getCamera()->accept(iv);
		creator->getCurrentHandler()->endInteraction();

		if (intersector->containsIntersections())
		{
			const osgUtil::LineSegmentIntersector::Intersection &firstHit = *(intersector->getIntersections().begin());
			int intersectionIndex = firstHit.indexList.front();
			double intersectionRatio = firstHit.ratio;

			creator->getCurrentHandler()->setInteraction(creator, intersectionIndex, intersectionRatio);
		}
	}
	else if (ea.getButtonMask() == 0)
	{
		// Mouse is moving and no button is pressed
		if (creator->getRubberband()) 
		{
			model->clearHoverView();

			osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

			// Here we assume the global coordinate system for now
			osg::Vec3 p{ 0.0, 0.0, 0.0 };
			osg::Vec3 n{ 0.0, 0.0, 1.0 };

			// Finally we need to consider the transformation into the currently active plane
			osg::Matrix transform = model->getCurrentWorkingPlaneTransform();
			transform.transpose(transform);
			p = p * transform;

			osg::Vec3 p2 = p + n;
			p2 = p2 * transform;
			n = p2 - p;
			n.normalize();

			Rubberband * rubberband = creator->getRubberband();
			if (rubberband) {
				rbeWrapper::RubberbandOsgWrapper * osgEngine = rubberband->engine();
				if (osgEngine) {
					if (osgEngine->currentStep() == 0) {
						lastHeight = 0.0;
						processRubberbandUpdate(viewer, ea.getX(), ea.getY(), IN_PLANE, n, p);
					}
					else {
						rbeCore::Step * step = osgEngine->step(osgEngine->currentStep());
						if (step) {
							switch (step->projection())
							{
							case rbeCore::Step::pUV: processRubberbandUpdate(viewer, ea.getX(), ea.getY(), IN_PLANE, n, p); break;
							case rbeCore::Step::pW: processRubberbandUpdate(viewer, ea.getX(), ea.getY(), HEIGHT, n, p); break;
							default:
								OTAssert(0, "Unknown step projection");
								break;
							}
						}
					}
				}
			}
		}
		else if (creator->hasHandlers())
		{
			// Here we need to perform face picks only
			osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

			osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

			osgUtil::IntersectionVisitor iv(intersector.get());
			iv.setTraversalMask(model->getCurrentTraversalMask());
			viewer->getCamera()->accept(iv);

			creator->processHoverViewHandlers(intersector);
		}
		else 
		{
			osg::ref_ptr<osgUtil::Intersector> intersector = createIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

			osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

			osgUtil::IntersectionVisitor iv(intersector.get());
			iv.setTraversalMask(model->getCurrentTraversalMask());
			viewer->getCamera()->accept(iv);
			
			if (!intersector->containsIntersections() && model->isWireFrameMode())
			{
				// Let's try to pick a face
				intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

				osgUtil::IntersectionVisitor iv(intersector.get());
				iv.setTraversalMask(model->getCurrentTraversalMask());
				viewer->getCamera()->accept(iv);
			}

			model->processHoverView(intersector, viewer->getSceneData()->getBound().radius());
		}
	}
	return false;
}

void ViewerObjectSelectionHandler::processRubberbandUpdate(osgViewer::Viewer *viewer, float mouseX, float mouseY, enum projectionType pType, 
														   const osg::Vec3 &n, const osg::Vec3 &p)
{
	// First determine the start and end points of the selection ray in world coordinates
	osg::Vec3 rayStart, rayEnd;
	getWorldRay(viewer, mouseX, mouseY, rayStart, rayEnd);

	switch (pType)
	{
		case IN_PLANE:
		{
			// We need to project on the plane

			// Calculate the intersection of the plane and the line
			osg::Vec3 ip;
			if (intersectLinePlane(n, p + n * lastHeight, rayStart, rayEnd, ip))
			{
				osg::Matrix transform = model->getCurrentWorkingPlaneTransformTransposedInverse();
				ip = ip * transform;

				ip.set(creator->snapDimension(ip.x()), creator->snapDimension(ip.y()), creator->snapDimension(ip.z()));

				// We have an intersection of the ray with the plane
				//assert(fabs(ip.z()) < 1e-5);
				creator->getRubberband()->updateCurrentPosition(ip.x(), ip.y(), 0.0);
				lastPointInPlane = ip;
			}

			break;
		}
		case HEIGHT:
		{
			// We need the height of the pick over the plane with reference
			// We create a help plane with is parallel to the given plane normal and which is oriented as perpendicular as possible to the 
			// start of the ray
			// Determine the vector between the start of the ray and the reference point (last point in plane)
			osg::Matrix transform = model->getCurrentWorkingPlaneTransform();
			transform.transpose(transform);

			osg::Vec3 referencePoint = lastPointInPlane * transform;

			osg::Vec3 ray = referencePoint - rayStart;
			ray.normalize();

			// Now we orthogonalize this direction with respect to the normal
			osg::Vec3 helpNormal = ray - n * (ray * n);
			helpNormal.normalize();

			// Calculate the intersection of the ray with the help plane
			osg::Vec3 ip;
			if (intersectLinePlane(helpNormal, referencePoint, rayStart, rayEnd, ip))
			{
				osg::Matrix transform = model->getCurrentWorkingPlaneTransformTransposedInverse();
				ip = ip * transform;

				// Now we need to determine the projection of the new point along the original plane normal
				osg::Vec3 d = ip - lastPointInPlane;
				double height = d * n;

				height = creator->snapDimension(height);
				lastHeight = height;

				creator->getRubberband()->updateCurrentPosition(referencePoint.x(), referencePoint.y(), height);
			}

			break;
		}
		default:
			assert(0); //Unknown projection type
	}
}

void ViewerObjectSelectionHandler::getWorldRay(osgViewer::Viewer *viewer, float mouseX, float mouseY, osg::Vec3 &rayStart, osg::Vec3 &rayEnd)
{
	// normalize the mouse position in the range [-1, 1] for both, x and y
	double viewPortWidth  = viewer->getCamera()->getViewport()->width();
	double viewPortHeight = viewer->getCamera()->getViewport()->height();

	double mx = 2.0 * mouseX / viewPortWidth  - 1.0;
	double my = 2.0 * mouseY / viewPortHeight - 1.0;

	// Back-project the position on the viewing frustum using the current camera
	// First, compute the matrix to unproject the mouse coords (in homogeneous space)    
	osg::Matrix VP = viewer->getCamera()->getViewMatrix() * viewer->getCamera()->getProjectionMatrix();

	osg::Matrix inverseVP;
	inverseVP.invert(VP);

	// Compute world near far in view coordinates
	osg::Vec3 nearPoint(mx, my, -1.0f);
	osg::Vec3 farPoint(mx, my, 1.0f);

	// Finally back-project the near / far points from view coordinates into world coordinates
	rayStart = nearPoint * inverseVP;
	rayEnd   = farPoint  * inverseVP;
}

// n : plane normal vector (unit vector)
// p : components of arbitrary point on the plane
// l1: components of arbitrary the line's start point
// l2: components of arbitrary the line's end point
// ip: components of intersection point between line and plane
bool ViewerObjectSelectionHandler::intersectLinePlane(const osg::Vec3 &n, const osg::Vec3 &p, const osg::Vec3 &l1, const osg::Vec3 &l2, osg::Vec3 &ip)
{
	// Calculate normalized direction vector of line
	osg::Vec3 l = l2 - l1;
	l.normalize();

	// Determine the scalar product of the plane normal and the line direction
	double sp = n * l;

	if (fabs(sp) < 1e-6) return false; // We do not have an intersection (line is parallel to plane)

	// Determine the line parameter of the intersection
	osg::Vec3 p0l0 = p - l1;

	double t = p0l0 * n / sp; 

	// Finally calculate the intersection point on the ray for the given parameter t
	ip = l1 + l * t;

	return true; 
} 


