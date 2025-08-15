#pragma once

#include <osgGA/GUIEventHandler>

class Model;
class Viewer;

namespace osgViewer
{
	class Viewer;
}

class ViewerObjectSelectionHandler : public osgGA::GUIEventHandler {
public:
	ViewerObjectSelectionHandler(Viewer * _creator) : active(false), creator(_creator), lastHeight(0.0), model(nullptr) {};
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) override;
	void setModel(Model *m) { model = m; };
	void setActive(bool a) { active = a; };

	Model* getModel() const { return model; };
	bool getActive() const { return active; };
	Viewer* getCreator() const { return creator; };
	double getLastHeight() const { return lastHeight; };

private:
	enum projectionType {IN_PLANE, HEIGHT};

	void processRubberbandUpdate(osgViewer::Viewer *viewer, float mouseX, float mouseY, enum projectionType pType, const osg::Vec3 &n, const osg::Vec3 &p);
	void getWorldRay(osgViewer::Viewer *viewer, float mouseX, float mouseY, osg::Vec3 &rayStart, osg::Vec3 &rayEnd);
	bool intersectLinePlane(const osg::Vec3 &n, const osg::Vec3 &p, const osg::Vec3 &l1, const osg::Vec3 &l2, osg::Vec3 &ip);
	osgUtil::Intersector *createIntersector(osgUtil::Intersector::CoordinateFrame cf, double x, double y);

	Model *model;
	bool   active;
	Viewer * creator;
	osg::Vec3 lastPointInPlane;
	double lastHeight;
};
